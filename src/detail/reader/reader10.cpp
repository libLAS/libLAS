/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS 1.0 reader implementation for C++ libLAS 
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

#include <liblas/detail/reader10.hpp>
#include <liblas/detail/reader/header.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>

// std
#include <fstream>
#include <istream>
#include <iostream>
#include <stdexcept>
#include <cstddef> // std::size_t
#include <cstdlib> // std::free
#include <cassert>

namespace liblas { namespace detail { namespace v10 {

ReaderImpl::ReaderImpl(std::istream& ifs) : Base(ifs)
{
}

LASVersion ReaderImpl::GetVersion() const
{
    return eLASVersion10;
}

bool ReaderImpl::ReadHeader(LASHeader& header)
{
    using detail::read_n;
    using detail::reader::Header;
    
    Header h(m_ifs);
    h.read();
    header = h.GetHeader();

    Reset(header);

    return true;
}

bool ReaderImpl::ReadNextPoint(LASPoint& point, const LASHeader& header)
{
    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(header.GetDataOffset(), std::ios::beg);
    }

    if (m_current < m_size)
    {
        // accounting to keep track of the fact that the DataRecordLength 
        // might not map to ePointSize0 or ePointSize1 (see http://liblas.org/ticket/142)
        std::size_t bytesread = 0;
        
        detail::PointRecord record;
        // TODO: Replace with compile-time assert
        assert(liblas::ePointSize0 == sizeof(record));

        try
        {
            detail::read_n(record, m_ifs, sizeof(PointRecord));
            ++m_current;
            bytesread += sizeof(PointRecord);
        }
        catch (std::out_of_range const& e) // we reached the end of the file
        {
            std::cerr << e.what() << std::endl;
            return false;
        }

        Reader::FillPoint(record, point, header);
        point.SetCoordinates(header, point.GetX(), point.GetY(), point.GetZ());

        if (header.GetDataFormatId() == liblas::ePointFormat1)
        {
            double gpst(0);

            detail::read_n(gpst, m_ifs, sizeof(double));
            point.SetTime(gpst);
            bytesread += sizeof(double);
        }

        if (bytesread != header.GetDataRecordLength())
        {
            off_type const pos(static_cast<off_type>(header.GetDataRecordLength() - bytesread));
            m_ifs.seekg(pos, std::ios::cur);
        }
        return true;
    }

    return false;
}

bool ReaderImpl::ReadPointAt(std::size_t n, LASPoint& point, const LASHeader& header)
{
    if (m_size <= n)
        return false;

    std::streamsize pos = (static_cast<std::streamsize>(n) * header.GetDataRecordLength()) + header.GetDataOffset();    

    m_ifs.clear();
    m_ifs.seekg(pos, std::ios::beg);

    // TODO: Replace with compile-time assert
    detail::PointRecord record;
    assert(liblas::ePointSize0 == sizeof(record));
    
    // accounting to keep track of the fact that the DataRecordLength 
    // might not map to ePointSize0 or ePointSize1 (see http://liblas.org/ticket/142)
    std::size_t bytesread = 0;

    detail::read_n(record, m_ifs, sizeof(record));

    bytesread += sizeof(PointRecord);

    Reader::FillPoint(record, point, header);
    point.SetCoordinates(header, point.GetX(), point.GetY(), point.GetZ());

    if (header.GetDataFormatId() == liblas::ePointFormat1)
    {
        double gpst(0);
        detail::read_n(gpst, m_ifs, sizeof(double));
        point.SetTime(gpst);
        bytesread += sizeof(double);
    }

    if (bytesread != header.GetDataRecordLength())
    {
        off_type const pos(static_cast<off_type>(header.GetDataRecordLength() - bytesread));
        m_ifs.seekg(pos, std::ios::cur);
    }

    return true;
}

}}} // namespace liblas::detail::v10

