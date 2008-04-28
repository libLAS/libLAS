// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_LASPOINT_HPP_INCLUDED
#define LIBLAS_LASPOINT_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <stdexcept> // std::out_of_range
#include <cstdlib> // std::size_t

#include <iostream>

namespace liblas {

/// Definition of point data record.
///
/// \todo TODO: Think about last 1-byte field in record Point Source ID (LAS 1.1)
class LASPoint
{
public:

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

    LASPoint();
    LASPoint(LASPoint const& other);
    LASPoint& operator=(LASPoint const& rhs);

    double GetX() const;
    double GetY() const;
    double GetZ() const;
    void SetCoordinates(double const& x, double const& y, double const& z);
    void SetCoordinates(LASHeader const& header, double x, double y, double z);
    
    void SetX(double const& value);
    void SetY(double const& value);
    void SetZ(double const& value);

    uint16_t GetIntensity() const;
    void SetIntensity(uint16_t const& intensity);

    uint8_t GetScanFlags() const;
    void SetScanFlags(uint8_t const& flags);
    
    uint16_t GetReturnNumber() const;
    void SetReturnNumber(uint16_t const& num);

    uint16_t GetNumberOfReturns() const;
    void SetNumberOfReturns(uint16_t const& num);

    uint16_t GetScanDirection() const;
    void SetScanDirection(uint16_t const& dir);
    
    uint16_t GetFlightLineEdge() const;
    void SetFlightLineEdge(uint16_t const& edge);

    uint8_t GetClassification() const;
    void SetClassification(uint8_t const& classify);

    int8_t GetScanAngleRank() const;
    void SetScanAngleRank(int8_t const& rank);

    uint8_t GetUserData() const;
    void SetUserData(uint8_t const& data);
    
    double GetTime() const;
    void SetTime(double const& time);

    double& operator[](std::size_t const& n);
    double const& operator[](std::size_t const& n) const;

    bool equal(LASPoint const& other) const;

    bool Validate() const;
    
private:

    static std::size_t const coords_size = 3;
    double m_coords[coords_size];
    uint16_t m_intensity;
    uint8_t m_flags;
    uint8_t m_class;
    int8_t m_angleRank;
    uint8_t m_userData;
    double m_gpsTime;

    void throw_out_of_range() const
    {
        throw std::out_of_range("coordinate subscript out of range");
    }
};

/// \todo To be documented.
inline bool operator==(LASPoint const& lhs, LASPoint const& rhs)
{
    return lhs.equal(rhs);
}

/// \todo To be documented.
inline bool operator!=(LASPoint const& lhs, LASPoint const& rhs)
{
    return (!(lhs == rhs));
}

inline void LASPoint::SetCoordinates(double const& x, double const& y, double const& z)
{
    m_coords[0] = x;
    m_coords[1] = y;
    m_coords[2] = z;
}

inline double LASPoint::GetX() const
{
    return m_coords[0];
}

inline void LASPoint::SetX( double const& value ) 
{
    m_coords[0] = value;
}

inline double LASPoint::GetY() const
{
    return m_coords[1];
}

inline void LASPoint::SetY( double const& value ) 
{
    m_coords[1] = value;
}

inline double LASPoint::GetZ() const
{
    return m_coords[2];
}

inline void LASPoint::SetZ( double const& value ) 
{
    m_coords[2] = value;
}

inline uint16_t LASPoint::GetIntensity() const
{
    return m_intensity;
}

inline void LASPoint::SetIntensity(uint16_t const& intensity)
{
    m_intensity = intensity;
}

inline uint16_t LASPoint::GetReturnNumber() const
{
    // Read bits 1,2,3 (first 3 bits)
    return (m_flags & 0x07);
}

inline uint16_t LASPoint::GetNumberOfReturns() const
{
    // Read bits 4,5,6
    return ((m_flags >> 3) & 0x07);
}

inline uint16_t LASPoint::GetScanDirection() const
{
    // Read 7th bit
    return ((m_flags >> 6) & 0x01);
}

inline uint16_t LASPoint::GetFlightLineEdge() const
{
    // Read 8th bit
    return ((m_flags >> 7) & 0x01);
}

inline uint8_t LASPoint::GetScanFlags() const
{
    return m_flags;
}

inline void LASPoint::SetScanFlags(uint8_t const& flags)
{
    m_flags = flags;
}

inline uint8_t LASPoint::GetClassification() const
{
    return m_class;
}

inline void LASPoint::SetClassification(uint8_t const& classify)
{
    m_class = classify;
}

inline int8_t LASPoint::GetScanAngleRank() const
{
    return m_angleRank;
}

inline uint8_t LASPoint::GetUserData() const
{
    return m_userData;
}

inline double LASPoint::GetTime() const
{
    return m_gpsTime;
}

inline void LASPoint::SetTime(double const& time)
{
    m_gpsTime = time;
}

inline double& LASPoint::operator[](std::size_t const& n)
{
    if (coords_size <= n)
        throw_out_of_range();

    return m_coords[n];
}

inline double const& LASPoint::operator[](std::size_t const& n) const
{
    if (coords_size <= n)
        throw_out_of_range();

    return m_coords[n];
}

} // namespace liblas

#endif // LIBLAS_LASPOINT_HPP_INCLUDED
