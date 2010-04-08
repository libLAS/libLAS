/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Point Reader implementation for C++ libLAS 
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Howard Butler
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
 
#include <liblas/detail/reader/reader.hpp>
#include <liblas/detail/reader/point.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>

#include <sstream> 

namespace liblas { namespace detail { namespace reader {

void Point::setup()
{

}

Point::Point(std::istream& ifs, const liblas::Header& header) :
    m_ifs(ifs), m_header(header), m_point(liblas::Point()), m_transform(0),m_format(header.GetPointFormat())
{
    setup();
}

Point::Point(   std::istream& ifs, 
                const liblas::Header& header, 
                OGRCoordinateTransformationH transform) :
    m_ifs(ifs), m_header(header), m_point(liblas::Point()), m_transform(transform), m_format(header.GetPointFormat())
{
    setup();
}

Point::~Point()
{

}

std::istream& Point::GetStream() const
{
    return m_ifs;
}

void Point::read()
{
    // accounting to keep track of the fact that the DataRecordLength 
    // might not map to ePointSize0 or ePointSize1 (see http://liblas.org/ticket/142)
    std::size_t bytesread(0);

    double gpst(0);
    liblas::uint16_t red(0);
    liblas::uint16_t blue(0);
    liblas::uint16_t green(0);
    
    detail::PointRecord record;
    // TODO: Replace with compile-time assert
    assert(liblas::ePointSize0 == sizeof(record));

    // const PointFormat& format = m_header.GetPointFormat();
    
    try
    {
        detail::read_n(record, m_ifs, sizeof(PointRecord));
        bytesread += sizeof(PointRecord);
    }
    catch (std::out_of_range const& e) // we reached the end of the file
    {
        std::cerr << e.what() << std::endl;
    }

    fill(record);
    // Reader::FillPoint(record, m_point, m_header);
    m_point.SetCoordinates(m_header, m_point.GetX(), m_point.GetY(), m_point.GetZ());

    if (m_format.HasTime()) 
    {

        detail::read_n(gpst, m_ifs, sizeof(double));
        m_point.SetTime(gpst);
        bytesread += sizeof(double);
        
        if (m_format.HasColor()) 
        {
            detail::read_n(red, m_ifs, sizeof(uint16_t));
            detail::read_n(green, m_ifs, sizeof(uint16_t));
            detail::read_n(blue, m_ifs, sizeof(uint16_t));

            liblas::Color color(red, green, blue);
            m_point.SetColor(color);
            
            bytesread += 3 * sizeof(uint16_t);
        }
    } else {
        if (m_format.HasColor()) 
        {
            detail::read_n(red, m_ifs, sizeof(uint16_t));
            detail::read_n(green, m_ifs, sizeof(uint16_t));
            detail::read_n(blue, m_ifs, sizeof(uint16_t));

            liblas::Color color(red, green, blue);
            m_point.SetColor(color);
            
            bytesread += 3 * sizeof(uint16_t);
        }        
    }
    
    if (bytesread != m_format.GetByteSize()) {
        std::ostringstream msg; 
        msg <<  "The number of bytes that were read ("<< bytesread <<") does not " 
                "match the number of bytes the point's format "
                "says it should have (" << 
                m_format.GetByteSize() << ")";
        throw std::runtime_error(msg.str());
        
    }
    if (m_format.GetByteSize() != m_header.GetDataRecordLength())
    {
        std::size_t bytesleft = m_header.GetDataRecordLength() - bytesread;

        std::vector<uint8_t> data;
        data.resize(bytesleft);

        detail::read_n(data.front(), m_ifs, bytesleft);
        
        m_point.SetExtraData(data); 
        
        // off_type const pos(static_cast<off_type>(m_header.GetDataRecordLength() - bytesread));
        // m_ifs.seekg(pos, std::ios::cur);
    }
}

void Point::project()
{
#ifdef HAVE_GDAL
    
    int ret = 0;
    double x = m_point.GetX();
    double y = m_point.GetY();
    double z = m_point.GetZ();
    
    ret = OCTTransform(m_transform, 1, &x, &y, &z);    
    if (!ret)
    {
        std::ostringstream msg; 
        msg << "Could not project point for Reader::" << CPLGetLastErrorMsg() << ret;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }

    m_point.SetX(x);
    m_point.SetY(y);
    m_point.SetZ(z);
#else
    detail::ignore_unused_variable_warning(m_point);
#endif
}

void Point::fill(PointRecord& record) 
{

    if (m_transform) {
        m_point.SetCoordinates(m_header, record.x, record.y, record.z);
        project();
        
        int32_t x = static_cast<int32_t>((m_point.GetX() - m_header.GetOffsetX()) / m_header.GetScaleX());
        int32_t y = static_cast<int32_t>((m_point.GetY() - m_header.GetOffsetY()) / m_header.GetScaleY());
        int32_t z = static_cast<int32_t>((m_point.GetZ() - m_header.GetOffsetZ()) / m_header.GetScaleZ());
        m_point.SetX(x);
        m_point.SetY(y);
        m_point.SetZ(z);
        
    } else {
        m_point.SetX(record.x);
        m_point.SetY(record.y);
        m_point.SetZ(record.z);
    }

    m_point.SetIntensity(record.intensity);
    m_point.SetScanFlags(record.flags);
    m_point.SetClassification((record.classification));
    m_point.SetScanAngleRank(record.scan_angle_rank);
    m_point.SetUserData(record.user_data);
    m_point.SetPointSourceID(record.point_source_id);
}
}}} // namespace liblas::detail::reader
