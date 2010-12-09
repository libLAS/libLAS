/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS reader class 
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ******************************************************************************
 * Copyright (c) 2008, Mateusz Loskot
 * Copyright (c) 2008, Phil Vachon
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

#include <liblas/lasversion.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/detail/reader/reader.hpp>
#include <liblas/detail/reader/cachedreader.hpp>
#include <liblas/utility.hpp>

// boost
#include <boost/cstdint.hpp>

// std
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>
#include <iostream>

using namespace boost;

namespace liblas
{

Reader::Reader(std::istream& ifs) :
    m_pimpl(new detail::CachedReaderImpl(ifs,3))
{
    Init();
}

Reader::Reader(std::istream& ifs, uint32_t cache_size) :
    m_pimpl(new detail::CachedReaderImpl(ifs,3))
{
    Init();
}

Reader::Reader(std::istream& ifs, uint32_t cache_size, Header const& header) :
    m_pimpl(new detail::CachedReaderImpl(ifs,3))
{
    // if we have a custom header, create a slot for it and then copy 
    // the header we were given

    Init();
}

Reader::Reader(ReaderI* reader) :
    m_pimpl(reader)
{
    Init();
}

Reader::Reader(std::istream& ifs, Header const& header) :
    m_pimpl(new detail::CachedReaderImpl(ifs,3))
{
    Init();
    m_pimpl->SetHeader(header);
}

Reader::~Reader()
{
    // empty, but required so we can implement PIMPL using
    // std::auto_ptr with incomplete type (Reader).
    // delete m_empty_point;
    
}

Header const& Reader::GetHeader() const
{
    return m_pimpl->GetHeader();
}

Point const& Reader::GetPoint() const
{
    return m_pimpl->GetPoint();
}


bool Reader::ReadNextPoint()
{  
    try
    {
        m_pimpl->ReadNextPoint();
        return true;
    }
    catch (std::out_of_range const&)
    {
    }

    return false;
}

bool Reader::ReadPointAt(std::size_t n)
{
    if (m_pimpl->GetHeader().GetPointRecordsCount() <= n)
    {
        throw std::out_of_range("point subscript out of range");
    }
    
    try
    {
        m_pimpl->ReadPointAt(n);
        return true;
    }
    catch (std::out_of_range const&)
    {
    }
    return false;
}

bool Reader::Seek(std::size_t n)
{
    try
    {
        assert(n < m_pimpl->GetHeader().GetPointRecordsCount());

        m_pimpl->Seek(n);
        return true;
    }
    catch (std::out_of_range const&)
    {
    }
    return false;
}
Point const& Reader::operator[](std::size_t n)
{
    if (m_pimpl->GetHeader().GetPointRecordsCount() <= n)
    {
        throw std::out_of_range("point subscript out of range");
    }
    
    bool read = ReadPointAt(n);

    if (read == false) 
    {
        throw std::out_of_range("no point record at given position");
    }

    
    return m_pimpl->GetPoint();
}

void Reader::Init()
{   
    // Copy our existing header in case we have already set a custom 
    // one.  We will use this instead of the one from the stream if 
    // the constructor with the header was used.
    
    // Header custom_header;
    // if (m_header != 0) 
    // {
    //     custom_header = *m_header;
    // }
    // 
    // m_pimpl->ReadHeader();
    // 
    // m_header = HeaderPtr(new liblas::Header(m_pimpl->GetHeader()));
    // 
    //     // throw std::runtime_error("public header block reading failure");
    // 
    // m_pimpl->Reset(m_header);
    // 
    // if (bCustomHeader) {
    //     custom_header.SetDataOffset(m_header->GetDataOffset());
    //     *m_header = custom_header;
    // }
    // 
    // This is yucky, but we need to ensure that we have a reference 
    // to a real point existing as soon as we are constructed.  
    // This is for someone who tries to GetPoint without first reading 
    // a point, and it will ensure they get something valid.  We just 
    // keep it around until the reader closes down and then deletes.  
    // 
    // m_point = m_empty_point.get();
    
    m_pimpl->ReadHeader();

}


void Reader::Reset() 
{
    Init();
}

void Reader::SetFilters(std::vector<liblas::FilterPtr> const& filters)
{
    m_pimpl->SetFilters(filters);
}

void Reader::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    m_pimpl->SetTransforms(transforms);
}

} // namespace liblas

