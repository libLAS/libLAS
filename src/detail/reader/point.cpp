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
// boost
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
// std
#include <cstring>
#include <sstream> 

using namespace boost;
using namespace std;

namespace liblas { namespace detail { namespace reader {

Point::Point(std::istream& ifs, HeaderPtr header)
    : m_ifs(ifs)
    , m_header(header)

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

void Point::fill(PointRecord& record) 
{

    m_point.SetX(record.x);
    m_point.SetY(record.y);
    m_point.SetZ(record.z);

    m_point.SetIntensity(record.intensity);
    m_point.SetScanFlags(record.flags);
    m_point.SetClassification((record.classification));
    m_point.SetScanAngleRank(record.scan_angle_rank);
    m_point.SetUserData(record.user_data);
    m_point.SetPointSourceID(record.point_source_id);
}

void Point::read()
{
    double gpst(0);
    uint16_t red(0);
    uint16_t blue(0);
    uint16_t green(0);

    // accounting to keep track of the fact that the DataRecordLength 
    // might not map to ePointSize0 or ePointSize1 (see http://liblas.org/ticket/142)
    std::vector<uint8_t>::size_type i = 0; 

    // Set the header for the point early because 
    // SetCoordinates will use it later to scale the 
    // point
    m_point.SetHeader(m_header);
    
    try
    {
        detail::read_n(m_raw_data.front(), m_ifs, m_raw_data.size());
    }
    catch (std::out_of_range const& e) // we reached the end of the file
    {
        std::cerr << e.what() << std::endl;
    }    
    
    detail::PointRecord record;

    memcpy(&record.x, &(m_raw_data[i]), sizeof(int32_t)); i+=sizeof(int32_t);
    memcpy(&record.y, &(m_raw_data[i]), sizeof(int32_t)); i+=sizeof(int32_t);
    memcpy(&record.z, &(m_raw_data[i]), sizeof(int32_t)); i+=sizeof(int32_t);
    memcpy(&record.intensity, &(m_raw_data[i]), sizeof(uint16_t)); i+=sizeof(uint16_t);
    memcpy(&record.flags, &(m_raw_data[i]), sizeof(uint8_t)); i+=sizeof(uint8_t);
    memcpy(&record.classification, &(m_raw_data[i]), sizeof(uint8_t)); i+=sizeof(uint8_t);
    memcpy(&record.scan_angle_rank, &(m_raw_data[i]), sizeof(int8_t)); i+=sizeof(int8_t);
    memcpy(&record.user_data, &(m_raw_data[i]), sizeof(uint8_t)); i+=sizeof(uint8_t);
    memcpy(&record.point_source_id, &(m_raw_data[i]), sizeof(uint16_t)); i+=sizeof(uint16_t);
   
    fill(record);
    m_point.SetCoordinates(m_point.GetX(), m_point.GetY(), m_point.GetZ());

    if (m_header->GetSchema().HasTime()) 
    {
        memcpy(&gpst, &(m_raw_data[i]), sizeof(double));
        
        m_point.SetTime(gpst);
        i += sizeof(double);
        
        if (m_header->GetSchema().HasColor()) 
        {
            memcpy(&red, &(m_raw_data[i]), sizeof(uint16_t));
            i += sizeof(uint16_t);
            memcpy(&green, &(m_raw_data[i]), sizeof(uint16_t));
            i += sizeof(uint16_t);
            memcpy(&blue, &(m_raw_data[i]), sizeof(uint16_t));
            i += sizeof(uint16_t);
    
            liblas::Color color(red, green, blue);
            m_point.SetColor(color);
            
        }
    } else {
        if (m_header->GetSchema().HasColor()) 
        {
            memcpy(&red, &(m_raw_data[i]), sizeof(uint16_t));
            i += sizeof(uint16_t);
            memcpy(&green, &(m_raw_data[i]), sizeof(uint16_t));
            i += sizeof(uint16_t);
            memcpy(&blue, &(m_raw_data[i]), sizeof(uint16_t));
            i += sizeof(uint16_t);
    
            liblas::Color color(red, green, blue);
            m_point.SetColor(color);
            
        }        
    }
    
    if (m_header->GetSchema().GetBaseByteSize() != m_header->GetSchema().GetByteSize())
    {
        std::size_t bytesleft = m_header->GetDataRecordLength() - i;
    
        
        std::vector<uint8_t> data;
        data.resize(bytesleft);
        
        memcpy(&(data.front()), &(m_raw_data[i]), bytesleft);
        m_point.SetExtraData(data); 
        
        i = i + bytesleft;
    }
    
    if (i != m_header->GetSchema().GetByteSize()) {
        std::ostringstream msg; 
        msg <<  "The number of bytes that were read ("<< i <<") does not " 
                "match the number of bytes the point's format "
                "says it should have (" << 
                m_header->GetSchema().GetByteSize() << ")";
        throw std::runtime_error(msg.str());
        
    }

    // Put the data on the point
    m_point.SetData(m_raw_data);
}

void Point::setup()
{
    m_raw_data.resize(m_header->GetSchema().GetByteSize());
}

}}} // namespace liblas::detail::reader
