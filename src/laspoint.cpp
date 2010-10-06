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
#include <liblas/lasschema.hpp>
#include <liblas/exception.hpp>
#include <liblas/detail/pointrecord.hpp>
// boost
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <liblas/external/property_tree/ptree.hpp>

// std
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <iosfwd>
#include <algorithm>

using namespace boost;

namespace liblas {

Point::Point()
    : m_header(HeaderPtr())
    , m_default_header(EmptyHeader::get())
{
    m_format_data.resize(ePointSize3);
    m_format_data.assign(ePointSize3, 0);
}

Point::Point(HeaderPtr hdr)
    : m_header(hdr)
    , m_default_header(EmptyHeader::get())
{
    m_format_data.resize(ePointSize3);
    m_format_data.assign(ePointSize3, 0);
}

Point::Point(Point const& other)
    : m_format_data(other.m_format_data)
    , m_header(other.m_header)
    , m_default_header(EmptyHeader::get())
{
}

Point& Point::operator=(Point const& rhs)
{
    if (&rhs != this)
    {
        m_format_data = rhs.m_format_data;
        m_header = rhs.m_header;
    }
    return *this;
}


void Point::SetCoordinates(double const& x, double const& y, double const& z)
{
    SetX(x);
    SetY(y);
    SetZ(z);
}


bool Point::equal(Point const& other) const
{
    // TODO - mloskot: Default epsilon is too small.
    //                 Is 0.00001 good as tolerance or too wide?
    //double const epsilon = std::numeric_limits<double>::epsilon(); 
    double const epsilon = 0.00001;

    double const dx = GetX() - other.GetX();
    double const dy = GetY() - other.GetY();
    double const dz = GetZ() - other.GetZ();

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

void Point::SetHeaderPtr(HeaderPtr header) 
{
    m_header = header;
}

HeaderPtr Point::GetHeaderPtr() const
{
    return m_header;
}

liblas::property_tree::ptree Point::GetPTree() const
{
    using liblas::property_tree::ptree;
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
    using liblas::property_tree::ptree;
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



double Point::GetX() const
{
    boost::int32_t v = GetRawX();
    
    double output = 0;
    
    if (m_header.get() != 0 ) 
    { 
        // Scale it
        output  = (v * m_header->GetScaleX()) + m_header->GetOffsetX();
    } else {
        output  = (v * m_default_header.GetScaleX()) + m_default_header.GetOffsetX();
    }
    
    return output;
}

double Point::GetY() const
{
    boost::int32_t v = GetRawY();
    
    double output = 0;
    
    if (m_header.get() != 0 ) 
    { 
        // Scale it
        output  = (v * m_header->GetScaleY()) + m_header->GetOffsetY();
    } else {
        output  = (v * m_default_header.GetScaleY()) + m_default_header.GetOffsetY();
    }
    
    return output;
}

double Point::GetZ() const
{
    boost::int32_t v = GetRawZ();
    
    double output = 0;
    
    if (m_header.get() != 0 ) 
    { 
        // Scale it
        output  = (v * m_header->GetScaleZ()) + m_header->GetOffsetZ();
    } else {
        output  = (v * m_default_header.GetScaleZ()) + m_default_header.GetOffsetZ();
    }
    
    return output;
}

void Point::SetX( double const& value ) 
{
    boost::int32_t v = static_cast<boost::int32_t>(value);
    if (m_header.get() != 0 ) 
    {
        // descale the value given our scale/offset
        v = static_cast<boost::int32_t>(
                             detail::sround(((value - m_header->GetOffsetX()) / 
                                              m_header->GetScaleX())));

    } else 
    {
        v = static_cast<boost::int32_t>(
                             detail::sround(((value - m_default_header.GetOffsetX()) / 
                                              m_default_header.GetScaleX())));
    }

    SetRawX(v);
}

void Point::SetY( double const& value ) 
{
    boost::int32_t v = static_cast<boost::int32_t>(value);
    if (m_header.get() != 0 ) 
    {
        // descale the value given our scale/offset
        v = static_cast<boost::int32_t>(
                             detail::sround(((value - m_header->GetOffsetY()) / 
                                              m_header->GetScaleY())));
    } else 
    {
        v = static_cast<boost::int32_t>(
                             detail::sround(((value - m_default_header.GetOffsetY()) / 
                                              m_default_header.GetScaleY())));
    }
    
    SetRawY(v);
}

void Point::SetZ( double const& value ) 
{
    boost::int32_t v = static_cast<boost::int32_t>(value);
    if (m_header) 
    {
        // descale the value given our scale/offset
        v = static_cast<boost::int32_t>(
                             detail::sround(((value - m_header->GetOffsetZ()) / 
                                              m_header->GetScaleZ())));
    } else
    {
        v = static_cast<boost::int32_t>(
                             detail::sround(((value - m_default_header.GetOffsetZ()) / 
                                              m_default_header.GetScaleZ())));
    }
    
    SetRawZ(v);
}

boost::int32_t Point::GetRawX() const
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("X");
    boost::int32_t output = liblas::detail::bitsToInt<boost::int32_t>(output, m_format_data, pos);

    return output;
}

boost::int32_t Point::GetRawY() const
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Y");
    boost::int32_t output = liblas::detail::bitsToInt<boost::int32_t>(output, m_format_data, pos);

    return output;
}

boost::int32_t Point::GetRawZ() const
{
    boost::int32_t output;
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Z");
    output = liblas::detail::bitsToInt<boost::int32_t>(output, m_format_data, pos);

    return output;
}

void Point::SetRawX( boost::int32_t const& value)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("X");
    liblas::detail::intToBits<boost::int32_t>(value, m_format_data, pos);
}

void Point::SetRawY( boost::int32_t const& value)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Y");
    liblas::detail::intToBits<boost::int32_t>(value, m_format_data, pos);
}

void Point::SetRawZ( boost::int32_t const& value)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Z");
    liblas::detail::intToBits<boost::int32_t>(value, m_format_data, pos);
}

boost::uint16_t Point::GetIntensity() const
{
    boost::uint16_t output = 
        liblas::detail::bitsToInt<boost::uint16_t>(
            output, m_format_data, GetDimensionPosition("Intensity"));

    return output;
}


void Point::SetIntensity(boost::uint16_t const& intensity)
{
    liblas::detail::intToBits<boost::uint16_t>(intensity, 
                                               m_format_data, 
                                               GetDimensionPosition("Intensity"));
}

boost::uint8_t Point::GetScanFlags() const
{
    return m_format_data[GetDimensionPosition("Return Number")];
}

void Point::SetScanFlags(boost::uint8_t const& flags)
{
    m_format_data[GetDimensionPosition("Return Number")] = flags;
}

boost::uint16_t Point::GetReturnNumber() const
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Return Number");
    boost::uint8_t flags = m_format_data[pos];
    
    // Read bits 1,2,3 (first 3 bits)
    return (flags & 0x07);
}

void Point::SetReturnNumber(uint16_t const& num)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Return Number");
    
    boost::uint8_t flags = m_format_data[pos];
    
    // Store value in bits 0,1,2
    uint8_t mask = 0x7 << 0; // 0b00000111
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(num) << 0);
    m_format_data[pos] = flags;
}


void Point::SetFlightLineEdge(uint16_t const& edge)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Flightline Edge");

    boost::uint8_t flags = m_format_data[pos];
    
    // Store value in bits 7
    uint8_t mask = 0x1 << 7; // 0b10000000
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(edge) << 7);
    m_format_data[pos] = flags;

//     // Store value in bit 7
//     uint8_t mask = 0x1 << 7; // 0b10000000
//     m_flags &= ~mask;
//     m_flags |= mask & (static_cast<uint8_t>(edge) << 7);}

}
    
boost::uint16_t Point::GetFlightLineEdge() const
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Flightline Edge");

    boost::uint8_t flags = m_format_data[pos];

    // Read 8th bit
    return ((flags >> 7) & 0x01);
}


boost::uint16_t Point::GetNumberOfReturns() const
{
    boost::uint8_t flags;
    
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Number of Returns");

    flags = m_format_data[pos];

    // Read bits 4,5,6
    return ((flags >> 3) & 0x07);
}

void Point::SetNumberOfReturns(uint16_t const& num)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Number of Returns");
    
    boost::uint8_t flags = m_format_data[pos];
    
    // Store value in bits 3,4,5
    uint8_t mask = 0x7 << 3; // 0b00111000
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(num) << 3);
    m_format_data[pos] = flags;

//     // Store value in bits 3,4,5
//     uint8_t mask = 0x7 << 3; // 0b00111000
//     m_flags &= ~mask;
//     m_flags |= mask & (static_cast<uint8_t>(num) << 3);

}

void Point::SetScanDirection(uint16_t const& dir)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Scan Direction");
    
    boost::uint8_t flags = m_format_data[pos];
    
    // Store value in bits 6
    uint8_t mask = 0x1 << 6; // 0b01000000
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(dir) << 6);
    m_format_data[pos] = flags;   
}

boost::uint16_t Point::GetScanDirection() const
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Scan Direction");
    
    boost::uint8_t flags = m_format_data[pos];

    // Read 6th bit
    return ((flags >> 6) & 0x01);
}


std::vector<boost::uint8_t>::size_type Point::GetDimensionPosition(std::string const& name) const
{
    SizesArray s;
    if (m_header) {
        s = m_header->GetSchema().GetSizes(name);
    } else {
        s = m_default_header.GetSchema().GetSizes(name);
    }   
    return s[0];
}

void Point::SetScanAngleRank(int8_t const& rank)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Scan Angle Rank");
    
    m_format_data[pos] = rank;

}
boost::int8_t Point::GetScanAngleRank() const
{
    
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Scan Angle Rank");
    
    return m_format_data[pos];
}

boost::uint8_t Point::GetUserData() const
{
    return m_format_data[GetDimensionPosition("User Data")];
}

void Point::SetUserData(boost::uint8_t const& flags)
{
    m_format_data[GetDimensionPosition("User Data")] = flags;
}

Classification Point::GetClassification() const
{
    boost::uint8_t kls = m_format_data[GetDimensionPosition("Classification")];    
    return Classification(kls);
}

void Point::SetClassification(Classification const& cls)
{
    m_format_data[GetDimensionPosition("Classification")] = cls.GetClass();
}

void Point::SetClassification(Classification::bitset_type const& flags)
{
    m_format_data[GetDimensionPosition("Classification")] = Classification(flags).GetClass();
}

void Point::SetClassification(boost::uint8_t const& flags)
{
    m_format_data[GetDimensionPosition("Classification")] = Classification(flags).GetClass();
}

boost::uint16_t Point::GetPointSourceID() const
{
    boost::uint16_t output;
    output = liblas::detail::bitsToInt<boost::uint16_t>(output, 
                                                        m_format_data, 
                                                        GetDimensionPosition("Point Source ID"));

    return output;
}


void Point::SetPointSourceID(boost::uint16_t const& id)
{
    liblas::detail::intToBits<boost::uint16_t>(id, 
                                               m_format_data, 
                                               GetDimensionPosition("Point Source ID"));
}

Color Point::GetColor() const
{
    boost::uint16_t red(0);
    boost::uint16_t green(0);
    boost::uint16_t blue(0);

    red = liblas::detail::bitsToInt<boost::uint16_t>(red, 
                                                     m_format_data, 
                                                     GetDimensionPosition("Red"));
    green = liblas::detail::bitsToInt<boost::uint16_t>(green, 
                                                     m_format_data, 
                                                     GetDimensionPosition("Green"));
    blue = liblas::detail::bitsToInt<boost::uint16_t>(blue, 
                                                     m_format_data, 
                                                     GetDimensionPosition("Blue"));

  return Color(red, green, blue);
}

void Point::SetColor(Color const& value)
{
    liblas::detail::intToBits<boost::uint16_t>(value.GetRed(), 
                                               m_format_data, 
                                               GetDimensionPosition("Red"));
    liblas::detail::intToBits<boost::uint16_t>(value.GetGreen(), 
                                               m_format_data, 
                                               GetDimensionPosition("Green"));
    liblas::detail::intToBits<boost::uint16_t>(value.GetBlue(), 
                                               m_format_data, 
                                               GetDimensionPosition("Blue"));
}

void Point::SetTime(double const& t)
{
    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Time");
    const boost::uint8_t* x_b =  reinterpret_cast<const boost::uint8_t*>(&t);
#if defined(LIBLAS_BIG_ENDIAN)
        for (boost::int32_t n = sizeof( double )-1; n >= 0; n--)
#else
        for (boost::uint32_t n = 0; n < sizeof( double ); n++)
#endif 
            m_format_data[pos+n] = x_b[n];

}

double Point::GetTime() const
{

    std::vector<boost::uint8_t>::size_type pos = GetDimensionPosition("Time");
    boost::uint8_t* data = new boost::uint8_t[8];
    
#if defined(LIBLAS_BIG_ENDIAN)
        for (boost::uint32_t n = 0; n < sizeof( double ); n++)
#else
        for (boost::int32_t n = sizeof( double )-1; n >= 0; n--)
#endif  
            data[n] = m_format_data[pos+n];

    const double* output = reinterpret_cast<const double*>(data);
    double out = *output;
    delete[] data;
    return out;
}


boost::any Point::GetValue(DimensionPtr d) const
{

    boost::any output;

    GetDimensionPosition(d->GetName());
 
    
    return output;
}


} // namespace liblas
