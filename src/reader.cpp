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

#include <liblas/version.hpp>
#include <liblas/reader.hpp>
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
    m_pimpl(new detail::ReaderImpl(ifs))
{
    Init();
}


Reader::Reader(ReaderIPtr reader) :
    m_pimpl(reader)
{
    Init();
}

Reader::Reader(Reader const& other) 
    : m_pimpl(other.m_pimpl)
{
}

Reader& Reader::operator=(Reader const& rhs)
{
    if (&rhs != this)
    {
        m_pimpl = rhs.m_pimpl;
    }
    return *this;
}

Reader::~Reader()
{

}

Header const& Reader::GetHeader() const
{
    return m_pimpl->GetHeader();
}

void Reader::SetHeader(Header const& header)
{
    m_pimpl->SetHeader(header);
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

std::vector<liblas::FilterPtr>  Reader::GetFilters() const
{
    return m_pimpl->GetFilters();
}

void Reader::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    m_pimpl->SetTransforms(transforms);
}

std::vector<liblas::TransformPtr>  Reader::GetTransforms() const
{
    return m_pimpl->GetTransforms();
}

} // namespace liblas

