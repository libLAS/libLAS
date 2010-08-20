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

#ifndef LIBLAS_LASPOINT_HPP_INCLUDED
#define LIBLAS_LASPOINT_HPP_INCLUDED

#include <liblas/lasclassification.hpp>
#include <liblas/lascolor.hpp>
#include <liblas/detail/pointrecord.hpp>
#include <liblas/detail/fwd.hpp>
// boost
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

// std
#include <stdexcept> // std::out_of_range
#include <cstdlib> // std::size_t
#include <vector> // std::vector


namespace liblas {

/// Point data record composed with X, Y, Z coordinates and attributes.
class Point
{
public:

    enum DataMemberFlag
    {
        eReturnNumber = 1,
        eNumberOfReturns = 2,
        eScanDirection = 4,
        eFlightLineEdge = 8,
        eClassification = 16,
        eScanAngleRank = 32,
        eTime = 64
    };

    enum ClassificationType
    {
        eCreated = 0,
        eUnclassified,
        eGround,
        eLowVegetation,
        eMediumVegetation,
        eHighVegetation,
        eBuilding,
        eLowPoint,
        eModelKeyPoint,
        eWater = 9,
        // = 10 // reserved for ASPRS Definition
        // = 11 // reserved for ASPRS Definition
        eOverlapPoints = 12
        // = 13-31 // reserved for ASPRS Definition
    };

    enum ScanAngleRankRange
    {
        eScanAngleRankMin = -90,
        eScanAngleRankMax = 90
    };

    Point();
    Point(Point const& other);
    Point& operator=(Point const& rhs);

    double GetX() const;
    double GetY() const;
    double GetZ() const;
    void SetCoordinates(double const& x, double const& y, double const& z);
    // void SetCoordinates(Header const& header, double x, double y, double z);
    
    void SetX(double const& value);
    void SetY(double const& value);
    void SetZ(double const& value);

    boost::uint16_t GetIntensity() const;
    void SetIntensity(boost::uint16_t const& intensity);

    /// Gets all scanning flags encoded as single byte.
    /// The flags are (mandatory):
    /// - Return Number (bits 0, 1, 2);
    /// - Number of Returns - given pulse (bits 3, 4, 5);
    /// - Scan Direction Flag (bit 6);
    /// - Edge of Flight Line (bit 7).
    boost::uint8_t GetScanFlags() const;

    /// Sets all scanning flags passed as a single byte.
    /// \sa Documentation of GetScanFlags method for flags details.
    void SetScanFlags(boost::uint8_t const& flags);
    
    boost::uint16_t GetReturnNumber() const;
    void SetReturnNumber(boost::uint16_t const& num);

    boost::uint16_t GetNumberOfReturns() const;
    void SetNumberOfReturns(boost::uint16_t const& num);

    boost::uint16_t GetScanDirection() const;
    void SetScanDirection(boost::uint16_t const& dir);
    
    boost::uint16_t GetFlightLineEdge() const;
    void SetFlightLineEdge(boost::uint16_t const& edge);

    //Classification& GetClassification();
    Classification const& GetClassification() const;
    void SetClassification(Classification const& cls);
    void SetClassification(Classification::bitset_type const& flags);
    void SetClassification(boost::uint8_t const& flags);

    boost::int8_t GetScanAngleRank() const;
    void SetScanAngleRank(boost::int8_t const& rank);

    /// Fetch value of File Marker (LAS 1.0) or User Data (LAS 1.1).
    boost::uint8_t GetUserData() const;

    /// Set value of File Marker (LAS 1.0) or User Data (LAS 1.1).
    void SetUserData(boost::uint8_t const& data);

    /// Fetch value of User Bit Field (LAS 1.0) or Point Source ID (LAS 1.1).
    boost::uint16_t GetPointSourceID() const;

    /// Set value of User Bit Field (LAS 1.0) or Point Source ID (LAS 1.1).
    void SetPointSourceID(boost::uint16_t const& id);

    /// Fetch color value associated with this point (LAS 1.2)
    Color const& GetColor() const;

    /// Set color value associated with this point (LAS 1.2)
    void SetColor(Color const& value);

    double GetTime() const;
    void SetTime(double const& time);

    /// Index operator providing access to XYZ coordinates of point record.
    /// Valid index values are 0, 1 or 2.
    /// \exception std::out_of_range if requested index is out of range (> 2).
    double& operator[](std::size_t const& index);

    /// Const version of index operator providing access to XYZ coordinates of point record.
    /// Valid index values are 0, 1 or 2.
    /// \exception std::out_of_range if requested index is out of range (> 2).
    double const& operator[](std::size_t const& index) const;

    /// \todo TODO: Should we compare other data members, but not only coordinates?
    bool equal(Point const& other) const;

    bool Validate() const;
    bool IsValid() const;
    
    std::vector<boost::uint8_t> const& GetExtraData() const { return m_extra_data; }
    void SetExtraData(std::vector<boost::uint8_t> const& v) { m_extra_data = v;}

    std::vector<boost::uint8_t> const& GetData() const {return m_format_data; }
    void SetData(std::vector<boost::uint8_t> const& v) { m_format_data = v;}
    
    void SetHeader(HeaderPtr header);
    HeaderPtr GetHeaderPtr() const;
    
    boost::property_tree::ptree GetPTree() const;

private:

    detail::PointRecord m_record;
    std::vector<boost::uint8_t> m_extra_data;
    std::vector<boost::uint8_t> m_format_data;
    boost::array<double, 3> m_coords;
    Color m_color;
    double m_gps_time;
    boost::uint16_t m_intensity;
    boost::uint16_t m_source_id;
    boost::uint8_t m_flags;
    boost::uint8_t m_user_data;
    boost::int8_t m_angle_rank;
    Classification m_class;
    HeaderPtr m_header;

    void throw_out_of_range() const;
};

/// Equal-to operator implemented in terms of Point::equal method.
inline bool operator==(Point const& lhs, Point const& rhs)
{
    return lhs.equal(rhs);
}

/// Not-equal-to operator implemented in terms of Point::equal method.
inline bool operator!=(Point const& lhs, Point const& rhs)
{
    return (!(lhs == rhs));
}

// inline void Point::SetCoordinates(double const& x, double const& y, double const& z)
// {
//     m_coords[0] = x;
//     m_coords[1] = y;
//     m_coords[2] = z;
// }

inline double Point::GetX() const
{
    return m_coords[0];
}

inline void Point::SetX( double const& value ) 
{
    m_coords[0] = value;
}

inline double Point::GetY() const
{
    return m_coords[1];
}

inline void Point::SetY( double const& value ) 
{
    m_coords[1] = value;
}

inline double Point::GetZ() const
{
    return m_coords[2];
}

inline void Point::SetZ( double const& value ) 
{
    m_coords[2] = value;
}

inline boost::uint16_t Point::GetIntensity() const
{
    return m_intensity;
}

inline void Point::SetIntensity(boost::uint16_t const& intensity)
{
    m_intensity = intensity;
}

inline boost::uint16_t Point::GetReturnNumber() const
{
    // Read bits 1,2,3 (first 3 bits)
    return (m_flags & 0x07);
}

inline boost::uint16_t Point::GetNumberOfReturns() const
{
    // Read bits 4,5,6
    return ((m_flags >> 3) & 0x07);
}

inline boost::uint16_t Point::GetScanDirection() const
{
    // Read 7th bit
    return ((m_flags >> 6) & 0x01);
}

inline boost::uint16_t Point::GetFlightLineEdge() const
{
    // Read 8th bit
    return ((m_flags >> 7) & 0x01);
}

inline boost::uint8_t Point::GetScanFlags() const
{
    return m_flags;
}

inline void Point::SetScanFlags(boost::uint8_t const& flags)
{
    m_flags = flags;
}

inline boost::int8_t Point::GetScanAngleRank() const
{
    return m_angle_rank;
}

inline boost::uint8_t Point::GetUserData() const
{
    return m_user_data;
}

inline boost::uint16_t Point::GetPointSourceID() const
{
    return m_source_id;
}

inline void Point::SetPointSourceID(boost::uint16_t const& id)
{
    m_source_id = id;
}

inline double Point::GetTime() const
{
    return m_gps_time;
}

inline void Point::SetTime(double const& time)
{
    // assert(time >= 0); // TODO: throw? --mloskot  // negative times are legit, I think -- hobu
    m_gps_time = time;
}

inline Color const& Point::GetColor() const
{
    return m_color;
}

inline void Point::SetColor(Color const& value)
{
    m_color = value;
}

inline double& Point::operator[](std::size_t const& index)
{
    if (index > m_coords.size() - 1)
        throw_out_of_range();

    return m_coords[index];
}

inline double const& Point::operator[](std::size_t const& index) const
{
    if (index > m_coords.size() - 1)
        throw_out_of_range();

    return m_coords[index];
}


std::ostream& operator<<(std::ostream& os, liblas::Point const&);


} // namespace liblas

#endif // LIBLAS_LASPOINT_HPP_INCLUDED
