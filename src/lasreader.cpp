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
    m_pimpl(new detail::ReaderImpl(ifs)),
    m_point(0),
    m_empty_point(new Point()),
    bCustomHeader(false)
{
    Init();
}

Reader::Reader(std::istream& ifs, uint32_t cache_size) :
    m_pimpl(new detail::ReaderImpl(ifs)),
    m_point(0),
    m_empty_point(new Point()),
    bCustomHeader(false)
{
    Init();
}

Reader::Reader(std::istream& ifs, uint32_t cache_size, Header const& header) :
    m_pimpl(new detail::ReaderImpl(ifs)),
    m_header(new Header(header)),
    m_point(0),
    m_empty_point(new Point()),
    bCustomHeader(true)
{
    // if we have a custom header, create a slot for it and then copy 
    // the header we were given

    Init();
}

Reader::Reader(ReaderI* reader) :
    m_pimpl(reader),
    m_point(0),
    m_empty_point(new Point()),
    bCustomHeader(false)

{
    Init();
}

Reader::Reader(std::istream& ifs, Header const& header) :
    m_pimpl(new detail::ReaderImpl(ifs)),
    m_point(0),
    m_empty_point(new Point()),
    bCustomHeader(true)
{
    // if we have a custom header, create a slot for it and then copy 
    // the header we were given
    m_header = HeaderPtr(new Header(header));
    Init();
}

Reader::~Reader()
{
    // empty, but required so we can implement PIMPL using
    // std::auto_ptr with incomplete type (Reader).
    // delete m_empty_point;
    
}

Header const& Reader::GetHeader() const
{
    return *m_header;
}

Point const& Reader::GetPoint() const
{
    return *m_point;
}


bool Reader::ReadNextPoint()
{  
    try
    {
        // m_point = m_pimpl->ReadNextPoint(m_header).get();
        m_point = const_cast<Point*>(&(m_pimpl->ReadNextPoint(m_header)));

        // // Filter the points and continue reading until we either find 
        // // one to keep or throw an exception.
        // if (!KeepPoint(*m_point))
        // {
        //     m_point = const_cast<Point*>(&(m_pimpl->ReadNextPoint(m_header)));
        //     while (!KeepPoint(*m_point))
        //     {
        //         m_point = const_cast<Point*>(&(m_pimpl->ReadNextPoint(m_header)));
        //     }
        // }
        // 
        // if (!m_transforms.empty())
        // {
        //     // Apply the transforms to each point
        //     std::vector<liblas::TransformPtr>::const_iterator ti;
        // 
        //     for (ti = m_transforms.begin(); ti != m_transforms.end(); ++ti)
        //     {
        //         liblas::TransformPtr transform = *ti;
        //         transform->transform(*m_point);
        //     }            
        // }
        // 
        return true;
    }
    catch (std::out_of_range)
    {
        m_point = 0;
    }

    return false;
}

bool Reader::ReadPointAt(std::size_t n)
{
    if (m_header->GetPointRecordsCount() <= n)
    {
        throw std::out_of_range("point subscript out of range");
    }
    
    try
    {
        m_point = const_cast<Point*>(&(m_pimpl->ReadPointAt(n, m_header)));
        return true;
    }
    catch (std::out_of_range)
    {
        m_point = 0;
    }
    return false;
}

bool Reader::Seek(std::size_t n)
{
    try
    {
        assert(n < m_header->GetPointRecordsCount());

        m_pimpl->Seek(n, m_header);
        return true;
    }
    catch (std::out_of_range)
    {
        m_point = 0;
    }
    return false;
}
Point const& Reader::operator[](std::size_t n)
{
    if (m_header->GetPointRecordsCount() <= n)
    {
        throw std::out_of_range("point subscript out of range");
    }
    
    ReadPointAt(n);

    if (m_point == 0) 
    {
        throw std::out_of_range("no point record at given position");
    }

    return *m_point;
}

void Reader::Init()
{   
    // Copy our existing header in case we have already set a custom 
    // one.  We will use this instead of the one from the stream if 
    // the constructor with the header was used.
    
    Header custom_header;
    if (m_header != 0) 
    {
        custom_header = *m_header;
    }

    m_header = m_pimpl->ReadHeader();

        // throw std::runtime_error("public header block reading failure");

    m_pimpl->Reset(m_header);
    
    if (bCustomHeader) {
        custom_header.SetDataOffset(m_header->GetDataOffset());
        *m_header = custom_header;
    }
    
    // This is yucky, but we need to ensure that we have a reference 
    // to a real point existing as soon as we are constructed.  
    // This is for someone who tries to GetPoint without first reading 
    // a point, and it will ensure they get something valid.  We just 
    // keep it around until the reader closes down and then deletes.  
    // 
    m_point = m_empty_point.get();

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

