/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS point class 
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

#include <liblas/laspoint.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/exception.hpp>
#include <liblas/detail/pointrecord.hpp>
// boost
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/property_tree/ptree.hpp>

// std
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <iosfwd>

using namespace boost;

namespace liblas {

Point::Point()
    : m_extra_data(0)
    , m_format_data(0)
    , m_gps_time(0)
    , m_intensity(0)
    , m_source_id(0)
    , m_flags(0)
    , m_user_data(0)
    , m_angle_rank(0)
{
    m_coords.assign(0);
}

Point::Point(Point const& other)
    : m_extra_data(other.m_extra_data)
    , m_format_data(other.m_format_data)
    , m_coords(other.m_coords)
    , m_color(other.m_color)
    , m_gps_time(other.m_gps_time)
    , m_intensity(other.m_intensity)
    , m_source_id(other.m_source_id)
    , m_flags(other.m_flags)
    , m_user_data(other.m_user_data)
    , m_angle_rank(other.m_angle_rank)
    , m_class(other.m_class)
    , m_header(other.m_header)
{
}

Point& Point::operator=(Point const& rhs)
{
    if (&rhs != this)
    {
        m_extra_data = rhs.m_extra_data;
        m_format_data = rhs.m_format_data;
        m_coords = rhs.m_coords;
        m_color = rhs.m_color;
        m_gps_time = rhs.m_gps_time;
        m_class = rhs.m_class;
        m_intensity = rhs.m_intensity;
        m_source_id = rhs.m_source_id;
        m_flags = rhs.m_flags;
        m_user_data = rhs.m_user_data;
        m_angle_rank = rhs.m_angle_rank;
        m_header = rhs.m_header;
    }
    return *this;
}

// void Point::SetCoordinates(Header const& header, double x, double y, double z)
// {
//     double const cx = (x * header.GetScaleX()) + header.GetOffsetX();
//     double const cy = (y * header.GetScaleY()) + header.GetOffsetY();
//     double const cz = (z * header.GetScaleZ()) + header.GetOffsetZ();
// 
//     SetCoordinates(cx, cy, cz);
// }

void Point::SetCoordinates(double const& x, double const& y, double const& z)
{
    if (m_header.get() != 0 ) {
        m_coords[0] = (x * m_header->GetScaleX()) + m_header->GetOffsetX();
        m_coords[1] = (y * m_header->GetScaleY()) + m_header->GetOffsetY();
        m_coords[2] = (z * m_header->GetScaleZ()) + m_header->GetOffsetZ();   
    } else {
        m_coords[0] = x;
        m_coords[1] = y;
        m_coords[2] = z;
    }
}


void Point::SetReturnNumber(uint16_t const& num)
{
    // Store value in bits 0,1,2
    uint8_t mask = 0x7 << 0; // 0b00000111
    m_flags &= ~mask;
    m_flags |= mask & (static_cast<uint8_t>(num) << 0);

}

void Point::SetNumberOfReturns(uint16_t const& num)
{
    // Store value in bits 3,4,5
    uint8_t mask = 0x7 << 3; // 0b00111000
    m_flags &= ~mask;
    m_flags |= mask & (static_cast<uint8_t>(num) << 3);
}

void Point::SetScanDirection(uint16_t const& dir)
{
    // Store value in bit 6
    uint8_t mask = 0x1 << 6; // 0b01000000
    m_flags &= ~mask;
    m_flags |= mask & (static_cast<uint8_t>(dir) << 6);
}

void Point::SetFlightLineEdge(uint16_t const& edge)
{
    // Store value in bit 7
    uint8_t mask = 0x1 << 7; // 0b10000000
    m_flags &= ~mask;
    m_flags |= mask & (static_cast<uint8_t>(edge) << 7);}

void Point::SetScanAngleRank(int8_t const& rank)
{
    m_angle_rank = rank;
}

void Point::SetUserData(uint8_t const& data)
{
    m_user_data = data;
}

Classification const& Point::GetClassification() const
{
    return m_class;
}

void Point::SetClassification(Classification const& cls)
{
    m_class = cls;
}

void Point::SetClassification(Classification::bitset_type const& flags)
{
    m_class = Classification(flags);
}

void Point::SetClassification(boost::uint8_t const& flags)
{
    m_class = Classification(flags);
}

bool Point::equal(Point const& other) const
{
    // TODO - mloskot: Default epsilon is too small.
    //                 Is 0.00001 good as tolerance or too wide?
    //double const epsilon = std::numeric_limits<double>::epsilon(); 
    double const epsilon = 0.00001;

    double const dx = m_coords[0] - other.m_coords[0];
    double const dy = m_coords[1] - other.m_coords[1];
    double const dz = m_coords[2] - other.m_coords[2];

    // TODO: Should we compare other data members, besides the coordinates?

    if ((dx <= epsilon && dx >= -epsilon)
     && (dy <= epsilon && dy >= -epsilon)
     && (dz <= epsilon && dz >= -epsilon))
    {
        return true;
    }

    // If we do other members
    // bool compare_classification(uint8_t cls, uint8_t expected)
    // {
    //    // 31 is max index in classification lookup table
    //    clsidx = (cls & 31);
    //    assert(clsidx <= 31); 
    //    return (clsidx == expected);
    // }

    return false;
}

bool Point::Validate() const
{
    unsigned int flags = 0;

    if (this->GetReturnNumber() > 0x07)
        flags |= eReturnNumber;

    if (this->GetNumberOfReturns() > 0x07)
        flags |= eNumberOfReturns;

    if (this->GetScanDirection() > 0x01)
        flags |= eScanDirection;

    if (this->GetFlightLineEdge() > 0x01)
        flags |= eFlightLineEdge;

    if (eScanAngleRankMin > this->GetScanAngleRank()
        || this->GetScanAngleRank() > eScanAngleRankMax)
    {
        flags |= eScanAngleRank;
    }

    if (flags > 0)
    {
        throw invalid_point_data("point data members out of range", flags);
    }

    return true;
}

bool Point::IsValid() const
{
    
    if (eScanAngleRankMin > this->GetScanAngleRank() || this->GetScanAngleRank() > eScanAngleRankMax)
        return false;

    if (this->GetFlightLineEdge() > 0x01)
        return false;

    if (this->GetScanDirection() > 0x01)
        return false;

    if (this->GetNumberOfReturns() > 0x07)
        return false;

    if (this->GetReturnNumber() > 0x07)
        return false;


    return true;
}

void Point::SetHeader(HeaderPtr header) 
{
    m_header = header;
}

HeaderPtr Point::GetHeaderPtr() const
{
    return m_header;
}

boost::property_tree::ptree Point::GetPTree() const
{
    using boost::property_tree::ptree;
    ptree pt;

    pt.put("x", GetX());
    pt.put("y", GetY());
    pt.put("z", GetZ());

    pt.put("time", GetTime());
    pt.put("intensity", GetIntensity());
    pt.put("returnnumber", GetReturnNumber());
    pt.put("numberofreturns", GetNumberOfReturns());
    pt.put("scandirection", GetScanDirection());
    
    pt.put("scanangle", GetScanAngleRank());
    pt.put("flightlineedge", GetFlightLineEdge());

    pt.put("userdata", GetUserData());
    pt.put("pointsourceid", GetPointSourceID());

    ptree klasses;
    
    liblas::Classification const& c = GetClassification();
    std::string name = c.GetClassName();

    klasses.put("name", name);
    klasses.put("id", c.GetClass());
    klasses.put("withheld", c.IsWithheld());
    klasses.put("keypoint", c.IsKeyPoint());
    klasses.put("synthetic", c.IsSynthetic());

    pt.add_child("classification",klasses);

    ptree colors;
    liblas::Color const& clr = GetColor();

    colors.put("red", clr.GetRed());
    colors.put("green", clr.GetGreen());
    colors.put("blue", clr.GetBlue());
    pt.add_child("color", colors);
    
    return pt;
}

std::ostream& operator<<(std::ostream& os, liblas::Point const& p)
{
    using boost::property_tree::ptree;
    ptree tree = p.GetPTree();

    os << "---------------------------------------------------------" << std::endl;
    
    os.setf(std::ios_base::fixed, std::ios_base::floatfield);
    os.precision(6);

    os << "  X: \t\t\t" << tree.get<double>("x") << std::endl;
    os << "  Y: \t\t\t" << tree.get<double>("y") << std::endl;
    os << "  Z: \t\t\t" << tree.get<double>("z") << std::endl;
    os << "  Time: \t\t" << tree.get<double>("time") << std::endl;
    os.unsetf(std::ios_base::fixed);
    os.unsetf(std::ios_base::floatfield);
    os << "  Return Number: \t" << tree.get<boost::uint32_t>("returnnumber") << std::endl;
    os << "  Return Count: \t" << tree.get<boost::uint32_t>("numberofreturns") << std::endl;
    os << "  Flightline Edge: \t" << tree.get<boost::uint32_t>("flightlineedge") << std::endl;
    os << "  Intensity: \t\t" << tree.get<boost::uint32_t>("intensity") << std::endl;
    os << "  Scan Direction: \t" << tree.get<boost::uint32_t>("scandirection") << std::endl;
    os << "  Scan Angle Rank: \t" << tree.get<boost::int32_t>("scanangle") << std::endl;
    os << "  Classification: \t" << tree.get<std::string>("classification.name") << std::endl;
    os << "         witheld: \t" << tree.get<std::string>("classification.withheld") << std::endl;
    os << "        keypoint: \t" << tree.get<std::string>("classification.keypoint") << std::endl;
    os << "       synthetic: \t" << tree.get<std::string>("classification.synthetic") << std::endl;
    os << "  RGB Color: \t\t" << tree.get<boost::uint32_t>("color.red") << " " 
                              << tree.get<boost::uint32_t>("color.green") << " "
                              << tree.get<boost::uint32_t>("color.blue") << std::endl;
    os << "---------------------------------------------------------" << std::endl;

    return os;
}

void Point::throw_out_of_range() const
{
    throw std::out_of_range("coordinate subscript out of range");
}


} // namespace liblas
