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

#include <liblas/point.hpp>
#include <liblas/header.hpp>
#include <liblas/schema.hpp>
#include <liblas/exception.hpp>
#include <liblas/detail/binary.hpp>
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
#include <numeric>

using namespace boost;

namespace liblas {

Point::Point(Header const* hdr)
    : 
     m_header(hdr)
    , m_default_header(DefaultHeader::get())
{
    m_data.resize(hdr->GetDataRecordLength());
    m_data.assign(hdr->GetDataRecordLength(), 0);
}

Point::Point(Point const& other)
    : m_data(other.m_data)
    , m_header(other.GetHeader())
    , m_default_header(DefaultHeader::get())
{
}

Point& Point::operator=(Point const& rhs)
{
    if (&rhs != this)
    {
        m_data = rhs.m_data;
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

void Point::SetHeader(Header const* header)
{

    if (!header)
    {
        throw liblas_error("header reference for SetHeader is void");
    }
    
    // If we don't have a header initialized, set the point's to the 
    // one we were given.
    if (!m_header) m_header = header;

    // This is hopefully faster than copying everything if we don't have 
    // any data set and nothing to worry about.
    const liblas::Schema* schema;
    uint16_t wanted_length = header->GetDataRecordLength();
    schema = &(header->GetSchema());
    uint32_t sum = std::accumulate(m_data.begin(), m_data.end(), 0);
    
    if (!sum) {
        std::vector<uint8_t> data;
        data.resize(wanted_length);
        data.assign(wanted_length, 0);
        m_data = data;
        m_header = header;
        return;
    }
    
    bool bApplyNewScaling = true;
    

    
    if (detail::compare_distance(header->GetScaleX(), m_header->GetScaleX()) &&
        detail::compare_distance(header->GetScaleY(), m_header->GetScaleY()) &&
        detail::compare_distance(header->GetScaleZ(), m_header->GetScaleZ()) &&
        detail::compare_distance(header->GetOffsetX(), m_header->GetOffsetX()) &&
        detail::compare_distance(header->GetOffsetY(), m_header->GetOffsetY()) &&
        detail::compare_distance(header->GetOffsetZ(), m_header->GetOffsetZ()))
        bApplyNewScaling = false;
    else
        bApplyNewScaling = true;


    
    if (wanted_length != m_data.size())
    {
        // Manually copy everything but the header ptr
        // We can't just copy the raw data because its 
        // layout is likely changing as a result of the 
        // schema change.
        Point p(*this);
        m_header = header;

        std::vector<uint8_t> data;
        data.resize(wanted_length);
        data.assign(wanted_length, 0);
        m_data = data;
        m_header = header;
    
        SetX(p.GetX());
        SetY(p.GetY());
        SetZ(p.GetZ());
        
        SetIntensity(p.GetIntensity());
        SetScanFlags(p.GetScanFlags());
        SetClassification(p.GetClassification());
        SetScanAngleRank(p.GetScanAngleRank());
        SetUserData(p.GetUserData());
        SetPointSourceID(p.GetPointSourceID());
        
        boost::optional< Dimension const& > t = schema->GetDimension("Time");
        if (t)
            SetTime(p.GetTime());

        boost::optional< Dimension const& > c = schema->GetDimension("Red");
        if (c)
            SetColor(p.GetColor());

        // FIXME: copy other custom dimensions here?  resetting the 
        // headerptr can be catastrophic in a lot of cases.  
    } 


    // The header's scale/offset can change the raw storage of xyz.  
    // SetHeader can result in a rescaling of the data.
    double x;
    double y;
    double z;

    if (bApplyNewScaling)
    {
        x = GetX();
        y = GetY();
        z = GetZ();        
    }

    m_header = header;

    if (bApplyNewScaling)
    {
        SetX(x);
        SetY(y);
        SetZ(z);
    }    

    
}
Header const* Point::GetHeader() const
{
    if (m_header) return m_header; else return &m_default_header;
}

liblas::property_tree::ptree Point::GetPTree() const
{
    using liblas::property_tree::ptree;
    ptree pt;

    pt.put("x", GetX());
    pt.put("y", GetY());
    pt.put("z", GetZ());

    pt.put("rawx", GetRawX());
    pt.put("rawy", GetRawY());
    pt.put("rawz", GetRawZ());
    
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

// I hate you windows
#ifdef _MSC_VER
#ifdef GetClassName
#undef GetClassName
#endif
#endif
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
    os << "  Return Number: \t" << tree.get<uint32_t>("returnnumber") << std::endl;
    os << "  Return Count: \t" << tree.get<uint32_t>("numberofreturns") << std::endl;
    os << "  Flightline Edge: \t" << tree.get<uint32_t>("flightlineedge") << std::endl;
    os << "  Intensity: \t\t" << tree.get<uint32_t>("intensity") << std::endl;
    os << "  Scan Direction: \t" << tree.get<uint32_t>("scandirection") << std::endl;
    os << "  Scan Angle Rank: \t" << tree.get<int32_t>("scanangle") << std::endl;
    os << "  Classification: \t" << tree.get<std::string>("classification.name") << std::endl;
    os << "         witheld: \t" << tree.get<std::string>("classification.withheld") << std::endl;
    os << "        keypoint: \t" << tree.get<std::string>("classification.keypoint") << std::endl;
    os << "       synthetic: \t" << tree.get<std::string>("classification.synthetic") << std::endl;
    os << "  RGB Color: \t\t" << tree.get<uint32_t>("color.red") << " " 
                              << tree.get<uint32_t>("color.green") << " "
                              << tree.get<uint32_t>("color.blue") << std::endl;
    os << "---------------------------------------------------------" << std::endl;

    return os;
}


double Point::GetX() const
{
    int32_t v = GetRawX();
    
    double output  = (v * GetHeader()->GetScaleX()) + GetHeader()->GetOffsetX();
    
    return output;
}

double Point::GetY() const
{
    int32_t v = GetRawY();
    
    double output = (v * GetHeader()->GetScaleY()) + GetHeader()->GetOffsetY();
    return output;
}

double Point::GetZ() const
{
    int32_t v = GetRawZ();
    
    double output = 0;
    
    output  = (v * GetHeader()->GetScaleZ()) + GetHeader()->GetOffsetZ();
    
    return output;
}

void Point::SetX( double const& value ) 
{
    int32_t v;
    double scale;
    double offset;

    scale = GetHeader()->GetScaleX();
    offset = GetHeader()->GetOffsetX();

    // descale the value given our scale/offset
    v = static_cast<int32_t>(
                         detail::sround((value - offset) / scale));
    SetRawX(v);
}

void Point::SetY( double const& value ) 
{
    int32_t v;
    double scale;
    double offset;

    scale = GetHeader()->GetScaleY();
    offset = GetHeader()->GetOffsetY();


    // descale the value given our scale/offset
    v = static_cast<int32_t>(
                         detail::sround((value - offset) / scale));
    SetRawY(v);
}

void Point::SetZ( double const& value ) 
{
    int32_t v;
    double scale;
    double offset;

    scale = GetHeader()->GetScaleZ();
    offset = GetHeader()->GetOffsetZ();

    // descale the value given our scale/offset
    v = static_cast<int32_t>(
                         detail::sround((value - offset) / scale));
    SetRawZ(v);
}

int32_t Point::GetRawX() const
{
    // std::vector<uint8_t>::size_type pos = GetDimensionPosition("X");
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(0);
    std::vector<uint8_t>::size_type pos = 0;
    
#ifdef LIBLAS_ENDIAN_AWARE
    int32_t output = liblas::detail::bitsToInt<int32_t>(output, m_data, pos);
    return output;
#else
    uint8_t* data = const_cast<uint8_t*>(&m_data[0] + pos);
    int32_t* output = reinterpret_cast<int32_t*>(data);
    return *output;
#endif
}

int32_t Point::GetRawY() const
{
    // std::vector<uint8_t>::size_type pos = GetDimensionPosition("Y");
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(1);
    std::vector<uint8_t>::size_type pos = 4;

#ifdef LIBLAS_ENDIAN_AWARE
    int32_t output = liblas::detail::bitsToInt<int32_t>(output, m_data, pos);
    return output;
#else
    uint8_t* data = const_cast<uint8_t*>(&m_data[0] + pos);
    int32_t* output = reinterpret_cast<int32_t*>(data);
    return *output;
#endif
}

int32_t Point::GetRawZ() const
{
    // std::vector<uint8_t>::size_type pos = GetDimensionPosition("Z");
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(2);
    std::vector<uint8_t>::size_type pos = 8;

#ifdef LIBLAS_ENDIAN_AWARE
    int32_t output = liblas::detail::bitsToInt<int32_t>(output, m_data, pos);
    return output;
#else
    uint8_t* data = const_cast<uint8_t*>(&m_data[0] + pos);
    int32_t* output = reinterpret_cast<int32_t*>(data);
    return *output;
#endif
}

void Point::SetRawX( int32_t const& value)
{
    // std::vector<uint8_t>::size_type pos = GetDimensionPosition("X");
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(0);   
    std::vector<uint8_t>::size_type pos = 0;     
    liblas::detail::intToBits<int32_t>(value, m_data, pos);
}

void Point::SetRawY( int32_t const& value)
{
    // std::vector<uint8_t>::size_type pos = GetDimensionPosition("Y");
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(1);
    std::vector<uint8_t>::size_type pos = 4;
    liblas::detail::intToBits<int32_t>(value, m_data, pos);
}

void Point::SetRawZ( int32_t const& value)
{
    // std::vector<uint8_t>::size_type pos = GetDimensionPosition("Z");
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(2);
    std::vector<uint8_t>::size_type pos = 8;    
    liblas::detail::intToBits<int32_t>(value, m_data, pos);
}

uint16_t Point::GetIntensity() const
{
    // Intensity's position is always the 4th dimension
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(3);
    std::vector<uint8_t>::size_type pos = 12;

#ifdef LIBLAS_ENDIAN_AWARE
    uint16_t output = liblas::detail::bitsToInt<uint16_t>(output, m_data, pos);
    return output;
#else
    uint8_t* data = const_cast<uint8_t*>(&m_data[0] + pos);
    uint16_t* output = reinterpret_cast<uint16_t*>(data);
    return *output;
#endif

}


void Point::SetIntensity(uint16_t const& intensity)
{
    // Intensity's position is always the 4th dimension
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(3);
    std::vector<uint8_t>::size_type pos = 12;    
    liblas::detail::intToBits<uint16_t>(intensity, 
                                               m_data, 
                                               pos);
}

uint8_t Point::GetScanFlags() const
{
    // Scan Flag's position is always the 5th dimension 
    // (the entire byte composed of "Return Number", "Number of Returns", 
    // "Scan Direction", and "Flightline Edge")
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(4);
    std::vector<uint8_t>::size_type pos = 14;    
    return m_data[pos];
}

void Point::SetScanFlags(uint8_t const& flags)
{
    // Scan Flag's position is always the 5th dimension 
    // (the entire byte composed of "Return Number", "Number of Returns", 
    // "Scan Direction", and "Flightline Edge")
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(4);       
    std::vector<uint8_t>::size_type pos = 14;    
    m_data[pos] = flags;
}

uint16_t Point::GetReturnNumber() const
{
    // "Return Number" is always the 5th dimension
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(4);
    std::vector<uint8_t>::size_type pos = 14;    
    
    uint8_t flags = m_data[pos];
    
    // Read bits 1,2,3 (first 3 bits)
    return (flags & 0x07);
}

void Point::SetReturnNumber(uint16_t const& num)
{
    // "Return Number" is always the 5th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(4);
    std::vector<uint8_t>::size_type pos = 14;    
    uint8_t flags = m_data[pos];
    
    // Store value in bits 0,1,2
    uint8_t mask = 0x7 << 0; // 0b00000111
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(num) << 0);
    m_data[pos] = flags;
}

uint16_t Point::GetNumberOfReturns() const
{
    uint8_t flags;

    // "Number of Returns" is always the 6th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(5);
    std::vector<uint8_t>::size_type pos = 14;

    flags = m_data[pos];

    // Read bits 4,5,6
    return ((flags >> 3) & 0x07);
}

void Point::SetNumberOfReturns(uint16_t const& num)
{
    // "Number of Returns" is always the 6th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(5);
    std::vector<uint8_t>::size_type pos = 14;    
    
    uint8_t flags = m_data[pos];
    
    // Store value in bits 3,4,5
    uint8_t mask = 0x7 << 3; // 0b00111000
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(num) << 3);
    m_data[pos] = flags;

}

void Point::SetScanDirection(uint16_t const& dir)
{
    // "Scan Direction" is always the 7th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(6);
    std::vector<uint8_t>::size_type pos = 14;    
    
    uint8_t flags = m_data[pos];
    
    // Store value in bits 6
    uint8_t mask = 0x1 << 6; // 0b01000000
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(dir) << 6);
    m_data[pos] = flags;   
}

uint16_t Point::GetScanDirection() const
{
    // "Scan Direction" is always the 7th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(6);
    std::vector<uint8_t>::size_type pos = 14;        
    uint8_t flags = m_data[pos];

    // Read 6th bit
    return ((flags >> 6) & 0x01);
}

void Point::SetFlightLineEdge(uint16_t const& edge)
{
    // "Flightline Edge" is always the 8th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(7);
    std::vector<uint8_t>::size_type pos = 14;    

    uint8_t flags = m_data[pos];
    
    // Store value in bits 7
    uint8_t mask = 0x1 << 7; // 0b10000000
    flags &= ~mask;
    flags |= mask & (static_cast<uint8_t>(edge) << 7);
    m_data[pos] = flags;

}
    
uint16_t Point::GetFlightLineEdge() const
{
    // "Flightline Edge" is always the 8th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(7);
    std::vector<uint8_t>::size_type pos = 14;    
    
    uint8_t flags = m_data[pos];

    // Read 8th bit
    return ((flags >> 7) & 0x01);
}


Classification Point::GetClassification() const
{
    // "Classification" is always the 9th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(8);
    std::vector<uint8_t>::size_type pos = 15;
    
    uint8_t kls = m_data[pos];
    return Classification(kls);
}

void Point::SetClassification(Classification const& cls)
{
    // "Classification" is always the 9th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(8);
    std::vector<uint8_t>::size_type const pos = 15;
    m_data[pos] = static_cast<uint8_t>(cls.GetFlags().to_ulong());
}

void Point::SetClassification(Classification::bitset_type const& flags)
{
    // "Classification" is always the 9th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(8);
    std::vector<uint8_t>::size_type const pos = 15;    
    m_data[pos] = static_cast<uint8_t>(flags.to_ulong());
}

void Point::SetClassification(uint8_t const& flags)
{
    // "Classification" is always the 9th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(8);
    std::vector<uint8_t>::size_type const  pos = 15;
    m_data[pos] = flags; 
}

void Point::SetScanAngleRank(int8_t const& rank)
{
    // "Scan Angle Rank" is always the 10th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(9);
    std::vector<uint8_t>::size_type pos = 16;

    m_data[pos] = rank;

}
int8_t Point::GetScanAngleRank() const
{
    // "Scan Angle Rank" is always the 10th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(9);
    std::vector<uint8_t>::size_type pos = 16;

    return m_data[pos];
}

uint8_t Point::GetUserData() const
{
    // "User Data" is always the 11th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(10);
    std::vector<uint8_t>::size_type pos = 17;
    return m_data[pos];
}

void Point::SetUserData(uint8_t const& flags)
{
    // "User Data" is always the 11th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(10);
    std::vector<uint8_t>::size_type pos = 17;
    m_data[pos] = flags;
}

uint16_t Point::GetPointSourceID() const
{

    // "Point Source ID" is always the 12th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(11);
    std::vector<uint8_t>::size_type pos = 18;

#ifdef LIBLAS_ENDIAN_AWARE
    uint16_t output = liblas::detail::bitsToInt<uint16_t>(output, m_data, pos);
    return output;
#else
    uint8_t* data = const_cast<uint8_t*>(&m_data[0] + pos);
    uint16_t* output = reinterpret_cast<uint16_t*>(data);
    return *output;
#endif

}

void Point::SetPointSourceID(uint16_t const& id)
{
    // "Point Source ID" is always the 12th dimension    
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(11);
    std::vector<uint8_t>::size_type pos = 18;    
    liblas::detail::intToBits<uint16_t>(id, m_data, pos);
}

void Point::SetTime(double const& t)
{

    // "Time" is the 13th dimension if it exists
    // std::size_t index_pos = 12;

    PointFormatName f = GetHeader()->GetDataFormatId();
    
    if ( f == ePointFormat0 || f == ePointFormat2 ) {
        std::ostringstream msg;
        msg << "Point::SetTime - Unable to set time for ePointFormat0 or ePointFormat2, "
            << "no Time dimension exists on this format";
        throw liblas::invalid_format(msg.str());
    }

    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(index_pos);
    std::vector<uint8_t>::size_type pos = 20;
    detail::binary::endian_value<double> value(t);
    value.store<detail::binary::little_endian_tag>(&m_data[0] + pos);
}

double Point::GetTime() const
{
    PointFormatName f = GetHeader()->GetDataFormatId();

    if (f == ePointFormat0 || f == ePointFormat2)
    {
        // std::ostringstream msg;
        // msg << "Point::GetTime - Unable to get time for ePointFormat0 or ePointFormat2, "
        //     << "no Time dimension exists on this format";
        // throw std::runtime_error(msg.str());
        return 0.0;
    }

    // "Time" is the 13th dimension if it exists
    // std::size_t const index_pos = 12;
    // std::vector<uint8_t>::size_type pos = GetDimensionBytePosition(index_pos);
    std::vector<uint8_t>::size_type pos = 20;   

#ifdef LIBLAS_ENDIAN_AWARE
    detail::binary::endian_value<double> output;
    output.load<detail::binary::little_endian_tag>(&m_data[0] + pos);
    return output;
#else
    uint8_t* data = const_cast<uint8_t*>(&m_data[0] + pos);
    double* output = reinterpret_cast<double*>(data);
    return *output;
#endif

}

Color Point::GetColor() const
{


    // "Color" starts at the 14th dimension if it exists
    // std::size_t index_pos = 13;

    Color color;
    PointFormatName f = GetHeader()->GetDataFormatId();
    
    if ( f == ePointFormat0 || f == ePointFormat1 ) {
        return color;
    }
    
    assert(!(f == ePointFormat0 || f == ePointFormat1));
    
    
    std::size_t index_pos = 20;

    if (f == ePointFormat3) 
        index_pos = index_pos + 8; // increment to include position of Time.
        


    std::vector<uint8_t>::size_type red_pos = index_pos;
    std::vector<uint8_t>::size_type green_pos = index_pos + 2;
    std::vector<uint8_t>::size_type blue_pos = index_pos + 4;

    assert(red_pos <= m_data.size());
    assert(blue_pos <= m_data.size());
    assert(green_pos <= m_data.size());

#ifdef LIBLAS_ENDIAN_AWARE
    using liblas::detail::bitsToInt;
    uint16_t red(0);
    uint16_t green(0);
    uint16_t blue(0);
    red = bitsToInt<uint16_t>(red, m_data, red_pos);
    green = bitsToInt<uint16_t>(green, m_data, green_pos);
    blue = bitsToInt<uint16_t>(blue, m_data, blue_pos);
    color[0] = red;
    color[1] = green;
    color[2] = blue;
#else
    uint8_t* red_data = const_cast<uint8_t*>(&m_data[0] + red_pos);
    uint16_t* p_red = reinterpret_cast<uint16_t*>(red_data);

    uint8_t* green_data = const_cast<uint8_t*>(&m_data[0] + green_pos);
    uint16_t* p_green = reinterpret_cast<uint16_t*>(green_data);

    uint8_t* blue_data = const_cast<uint8_t*>(&m_data[0] + blue_pos);
    uint16_t* p_blue = reinterpret_cast<uint16_t*>(blue_data);

    color[0] = *p_red;
    color[1] = *p_green;
    color[2] = *p_blue;    
#endif
    
    return color;


}

void Point::SetColor(Color const& value)
{

    // "Color" starts at the 14th dimension if it exists
    // std::size_t index_pos = 13;

    PointFormatName f = GetHeader()->GetDataFormatId();
    
    if ( f == ePointFormat0 || f == ePointFormat1 ) {
        std::ostringstream msg;
        msg << "Point::SetColor - Unable to set color for ePointFormat0 or ePointFormat1, "
            << "no Color dimension exists on this format";
        throw liblas::invalid_format(msg.str());
    }

    if ( m_data.size() == ePointFormat0 || f == ePointFormat1 ) {
        std::ostringstream msg;
        msg << "Point::SetColor - Unable to set color for ePointFormat0 or ePointFormat1, "
            << "no Color dimension exists on this format";
        throw liblas::invalid_format(msg.str());
    }
    
    using liblas::detail::intToBits;

    std::size_t index_pos = 20;

    if (f == ePointFormat3) 
        index_pos = 28; // increment to include position of Time.
    if (f == ePointFormat2) 
        index_pos = 20; // increment to include position of Time.

    
    std::vector<uint8_t>::size_type red_pos = index_pos;
    std::vector<uint8_t>::size_type green_pos = index_pos + 2;
    std::vector<uint8_t>::size_type blue_pos = index_pos + 4;

    assert(red_pos <= m_data.size());
    assert(blue_pos <= m_data.size());
    assert(green_pos <= m_data.size());
    
    intToBits<uint16_t>(value.GetRed(), m_data, red_pos);
    intToBits<uint16_t>(value.GetGreen(), m_data, green_pos);
    intToBits<uint16_t>(value.GetBlue(), m_data, blue_pos);
}

std::vector<uint8_t>::size_type Point::GetDimensionBytePosition(std::size_t dim_pos) const
{
    optional<Dimension const&> d;
    d = m_header->GetSchema().GetDimension(dim_pos);
    
    if (!d)
    {
        std::ostringstream oss;
        oss <<"Dimension at position " << dim_pos << " not found";
        throw liblas_error(oss.str());
    }
    return d->GetByteOffset();
}

boost::any Point::GetValue(Dimension const& d) const
{
    boost::any output;
    boost::ignore_unused_variable_warning(d);

    return output;
}

} // namespace liblas
