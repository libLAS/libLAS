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

// std
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>
#include <iostream>


namespace liblas
{

LASReader::LASReader(std::istream& ifs) :
    m_pimpl(detail::ReaderFactory::Create(ifs)),
    m_point(0),
    m_empty_point(new LASPoint()),
    bCustomHeader(false),
    m_ifs(ifs)
{
    Init();
}

LASReader::LASReader(std::istream& ifs, LASHeader& header) :
    m_pimpl(detail::ReaderFactory::Create(ifs)),
    m_point(0),
    m_empty_point(new LASPoint()),
    bCustomHeader(false),
    m_ifs(ifs)

{
    m_header = header;
    bCustomHeader = true;
    Init();
}

LASReader::~LASReader()
{
    // empty, but required so we can implement PIMPL using
    // std::auto_ptr with incomplete type (Reader).
    delete m_empty_point;
}

LASHeader const& LASReader::GetHeader() const
{
    return m_header;
}

LASPoint const& LASReader::GetPoint() const
{
    return *m_point;
}

bool LASReader::ReadNextPoint()
{
    try {
        m_point = const_cast<LASPoint*>(&(m_pimpl->ReadNextPoint(m_header)));
        return true;
    } catch (std::out_of_range& e) {
        m_point = 0;
        return false;
    }

}

bool LASReader::ReadPointAt(std::size_t n)
{
    try {
        m_point = const_cast<LASPoint*>(&(m_pimpl->ReadPointAt(n, m_header)));
        return true;
    } catch (std::out_of_range& e) {
        m_point = 0;
        return false;
    }

}

LASPoint const& LASReader::operator[](std::size_t n)
{
    if (m_header.GetPointRecordsCount() <= n)
    {
        throw std::out_of_range("point subscript out of range");
    }

    m_point = const_cast<LASPoint*>(&(m_pimpl->ReadPointAt(n, m_header)));
    if (m_point == 0) 
    {
        throw std::out_of_range("no point record at given position");
    }

    return *m_point;
}

void LASReader::Init()
{   
    // Copy our existing header in case we have already set a custom 
    // one.  We will use this instead of the one from the stream if 
    // the constructor with the header was used.
    LASHeader custom_header(m_header);

    m_header = m_pimpl->ReadHeader();

        // throw std::runtime_error("public header block reading failure");

    m_pimpl->Reset(m_header);
    
    if (bCustomHeader) {
        custom_header.SetDataOffset(m_header.GetDataOffset());
        m_header = custom_header;
    }
    
    // This is yucky, but we need to ensure that we have a reference 
    // to a real point existing as soon as we are constructed.  
    // This is for someone who tries to GetPoint without first reading 
    // a point, and it will ensure they get something valid.  We just 
    // keep it around until the reader closes down and then deletes.  
    // 
    m_point = m_empty_point;
}

std::istream& LASReader::GetStream() const
{
    return m_ifs;
}

void LASReader::Reset() 
{
    Init();
}

bool LASReader::IsEOF() const
{
    return GetStream().eof();
}

bool LASReader::SetSRS(const LASSpatialReference& srs)
{
    m_pimpl->SetOutputSRS(srs, m_header);
    return true;
}

bool LASReader::SetInputSRS(const LASSpatialReference& srs)
{
    m_pimpl->SetInputSRS(srs);
    return true;
}

bool LASReader::SetOutputSRS(const LASSpatialReference& srs)
{
    m_pimpl->SetOutputSRS(srs, m_header);
    return true;
}

} // namespace liblas

