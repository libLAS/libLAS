#ifndef LIBLAS_LASPOINT_HPP_INCLUDED
#define LIBLAS_LASPOINT_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <stdexcept> // std::out_of_range
#include <bitset>
#include <cstdlib> // std::size_t

#include <iostream>

namespace liblas {

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
    
    double GetTime() const;
    void SetTime(double const& time);

    double& operator[](std::size_t const& n);
    double const& operator[](std::size_t const& n) const;

    bool equal(LASPoint const& other) const;

private:

    static std::size_t const coords_size = 3;
    double m_coords[coords_size];
    uint16_t m_intensity;
    uint8_t m_flags;
    uint8_t m_class;
    double m_gpsTime;

    void throw_out_of_range() const
    {
        throw std::out_of_range("coordinate subscript out of range");
    }
};

inline bool operator==(LASPoint const& lhs, LASPoint const& rhs)
{
    return lhs.equal(rhs);
}

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

inline void LASPoint::SetReturnNumber(uint16_t const& num)
{
    if (num > 0x07)
        throw std::out_of_range("return number out of range");

    // Store value in bits 1,2,3
    uint8_t val = static_cast<uint8_t>(num);
    uint16_t const begin = 1;
    uint8_t mask = ~0;
    m_flags &= ~(mask << (begin - 1)); 
    m_flags |= ((val & mask) << (begin - 1));
}

inline uint16_t LASPoint::GetNumberOfReturns() const
{
    // Read bits 4,5,6
    return ((m_flags >> 3) & 0x07);
}

inline void LASPoint::SetNumberOfReturns(uint16_t const& num)
{
    if (num > 0x07)
        throw std::out_of_range("number of returns out of range");

    // Store value in bits 4,5,6
    uint8_t val = static_cast<uint8_t>(num);
    uint16_t const begin = 4;
    uint8_t mask = ~0;
    m_flags &= ~(mask << (begin - 1)); 
    m_flags |= ((val & mask) << (begin - 1));
}

inline uint16_t LASPoint::GetScanDirection() const
{
    // Read 7th bit
    return ((m_flags >> 6) & 0x01);
}

inline void LASPoint::SetScanDirection(uint16_t const& dir)
{
    if (dir > 0x01)
        throw std::out_of_range("scan direction flag out of range");
    
    // Store value in bit 7th
    uint8_t val = static_cast<uint8_t>(dir);
    uint16_t const begin = 7;
    uint8_t mask = ~0;
    m_flags &= ~(mask << (begin - 1)); 
    m_flags |= ((val & mask) << (begin - 1));
}

inline uint16_t LASPoint::GetFlightLineEdge() const
{
    // Read 8th bit
    return ((m_flags >> 7) & 0x01);
}

inline void LASPoint::SetFlightLineEdge(uint16_t const& edge)
{
    if (edge > 0x01)
        throw std::out_of_range("edge of flight line out of range");

    // Store value in bit 8th
    uint8_t val = static_cast<uint8_t>(edge);
    uint16_t const begin = 8;
    uint8_t mask = ~0;
    m_flags &= ~(mask << (begin - 1)); 
    m_flags |= ((val & mask) << (begin - 1));
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
    if (3 <= n)
        throw_out_of_range();

    return m_coords[n];
}

inline double const& LASPoint::operator[](std::size_t const& n) const
{
    if (3 <= n)
        throw_out_of_range();

    return m_coords[n];
}

} // namespace liblas

#endif // LIBLAS_LASPOINT_HPP_INCLUDED
