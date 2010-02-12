/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Point Writer implementation for C++ libLAS 
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
 
#include <liblas/detail/writer/writer.hpp>
#include <liblas/detail/writer/point.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/lasformat.hpp>

#include <sstream> 

namespace liblas { namespace detail { namespace writer {

void Point::setup()
{

}

Point::Point(   std::ostream& ofs, 
                liblas::uint32_t& count, 
                const LASHeader& header) : 
    Base(ofs, count), 
    m_ofs(ofs), 
    m_header(header), 
    m_point(LASPoint()), 
    m_transform(0)
{
    setup();
}

Point::Point(   std::ostream& ofs, 
                liblas::uint32_t& count,
                const LASHeader& header, 
                OGRCoordinateTransformationH transform) : Base(ofs, count),
    m_ofs(ofs), m_header(header), m_point(LASPoint()), m_transform(transform)

{
    setup();
}

Point::~Point()
{

}


void Point::write(const LASPoint& point)
{
    double t = 0;
    uint16_t red = 0;
    uint16_t blue = 0;
    uint16_t green = 0;
    LASColor color;
    
    // std::size_t byteswritten(0);
    
    m_point = point;
    fill();
    
    detail::write_n(m_ofs, m_record, sizeof(m_record));
    // byteswritten += sizeof(PointRecord);

    if (m_header.GetDataFormatId() == liblas::ePointFormat1)
    {
        t = point.GetTime();
        detail::write_n(m_ofs, t, sizeof(double));

        // byteswritten += sizeof(double);
    }
    else if (m_header.GetDataFormatId() == liblas::ePointFormat2)
    {
        color = point.GetColor();
        red = color.GetRed();
        green = color.GetGreen();
        blue = color.GetBlue();
        detail::write_n(m_ofs, red, sizeof(uint16_t));
        detail::write_n(m_ofs, green, sizeof(uint16_t));
        detail::write_n(m_ofs, blue, sizeof(uint16_t));
        
        // byteswritten += 3 * sizeof(uint16_t);
    }
    else if (m_header.GetDataFormatId() == liblas::ePointFormat3)
    {
        t = point.GetTime();
        detail::write_n(m_ofs, t, sizeof(double));
        // byteswritten += sizeof(double);
        
        color = point.GetColor();
        red = color.GetRed();
        green = color.GetGreen();
        blue = color.GetBlue();
        detail::write_n(m_ofs, red, sizeof(uint16_t));
        detail::write_n(m_ofs, green, sizeof(uint16_t));
        detail::write_n(m_ofs, blue, sizeof(uint16_t));
        
        // byteswritten += 3 * sizeof(uint16_t);
    }

    liblas::uint32_t& count = GetPointCount();
    count++;
    SetPointCount(count);

    // write in our extra data that the user set on the 
    // point up to the header's specified DataRecordLength
    if (m_header.GetPointFormat().GetByteSize() != m_header.GetDataRecordLength()) {

        std::vector<uint8_t> const& data = point.GetExtraData();
        std::streamsize const size = static_cast<std::streamsize>(m_header.GetDataRecordLength() - data.size());
        detail::write_n(GetStream(), data.front(), size);
    }
}

void Point::project()
{
#ifdef HAVE_GDAL
    
    int ret = 0;
    LASPoint& p = m_point;
    
    double x = p.GetX();
    double y = p.GetY();
    double z = p.GetZ();
    
    ret = OCTTransform(m_transform, 1, &x, &y, &z);
    
    if (!ret) {
        std::ostringstream msg; 
        msg << "Could not project point for Writer::" << CPLGetLastErrorMsg() << ret;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }
    
    p.SetX(x);
    p.SetY(y);
    p.SetZ(z);
#endif
}

void Point::fill() 
{
    LASPoint& p = m_point;
    if (m_transform) {
        
        project();
        m_record.x = static_cast<int32_t>((p.GetX() - m_header.GetOffsetX()) / m_header.GetScaleX());
        m_record.y = static_cast<int32_t>((p.GetY() - m_header.GetOffsetY()) / m_header.GetScaleY());
        m_record.z = static_cast<int32_t>((p.GetZ() - m_header.GetOffsetZ()) / m_header.GetScaleZ());
    } else {
        m_record.x = static_cast<int32_t>((p.GetX() - m_header.GetOffsetX()) / m_header.GetScaleX());
        m_record.y = static_cast<int32_t>((p.GetY() - m_header.GetOffsetY()) / m_header.GetScaleY());
        m_record.z = static_cast<int32_t>((p.GetZ() - m_header.GetOffsetZ()) / m_header.GetScaleZ());
    }

    LASClassification::bitset_type clsflags(p.GetClassification());
    m_record.classification = static_cast<uint8_t>(clsflags.to_ulong());

    m_record.intensity = p.GetIntensity();
    m_record.flags = p.GetScanFlags();
    m_record.scan_angle_rank = p.GetScanAngleRank();
    m_record.user_data = p.GetUserData();
    m_record.point_source_id = p.GetPointSourceID();
}
}}} // namespace liblas::detail::reader
