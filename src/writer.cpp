/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS writer class 
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ******************************************************************************
 * Copyright (c) 2008, Mateusz Loskot
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/

#include <liblas/version.hpp>
#include <liblas/writer.hpp>
#include <liblas/detail/writer/writer.hpp>
#include <liblas/detail/writer/zipwriter.hpp>
#include <liblas/factory.hpp>

// std
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>

namespace liblas
{

Writer::Writer(std::ostream& ofs, Header const& header) :
#ifdef HAVE_LASZIP
    m_pimpl(WriterIPtr( WriterFactory::CreateWithStream(ofs, header) ) )
#else
    m_pimpl(WriterIPtr(new detail::WriterImpl(ofs)))
#endif

{
    m_pimpl->SetHeader(header);
    m_pimpl->WriteHeader();
}


Writer::Writer(Writer const& other) 
    : m_pimpl(other.m_pimpl)
{
}

Writer& Writer::operator=(Writer const& rhs)
{
    if (&rhs != this)
    {
        m_pimpl = rhs.m_pimpl;
    }
    return *this;
}

Writer::Writer(WriterIPtr ptr) :
    m_pimpl(ptr)

{
}

Writer::~Writer()
{

}

Header const& Writer::GetHeader() const
{
    return m_pimpl->GetHeader();
}

void Writer::SetHeader(Header const& header)
{
    return m_pimpl->SetHeader(header);
}

bool Writer::WritePoint(Point const& point)
{
    m_pimpl->WritePoint(point);
    return true;
}

void Writer::WriteHeader()
{
    // The writer may update our header as part of its 
    // writing process (change VLRs for SRS's, for instance).
    m_pimpl->WriteHeader();
}

void Writer::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    m_pimpl->SetTransforms(transforms);
}
std::vector<liblas::FilterPtr>  Writer::GetFilters() const
{
    return m_pimpl->GetFilters();
}

void Writer::SetFilters(std::vector<liblas::FilterPtr> const& filters)
{
    m_pimpl->SetFilters(filters);
}    
std::vector<liblas::TransformPtr>  Writer::GetTransforms() const
{
    return m_pimpl->GetTransforms();
}
} // namespace liblas

