#ifndef LIBLAS_LASPOINT_HPP_INCLUDED
#define LIBLAS_LASPOINT_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <stdexcept> // std::out_of_range
#include <cstdlib> // std::size_t

namespace liblas {

// TODO: Move to implementation details namespace
struct LASPointRecord
{
    LASPointRecord() :
        x(0), y(0), z(0),
        intensity(0),
        flags(0),
        classification(0),
        scan_angle_rank(0),
        user_data(0),
        point_source_id(0)
    {}

    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint16_t intensity;
    uint8_t flags; // TODO: Replace with portable std::bitset<8>
    uint8_t classification;
    int8_t scan_angle_rank;
    uint8_t user_data;
    uint16_t point_source_id;
};

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
    void SetCoordinates(double x, double y, double z);

    uint16_t GetIntensity() const;
    void SetIntensity(uint16_t intensity);

    uint8_t GetScanFlags() const;
    void SetScanFlags(uint8_t flags);
    
    uint16_t GetReturnNumber() const;
    uint16_t GetNumberOfReturns() const;
    uint16_t GetScanDirection() const;
    uint16_t GetFlightLineEdge() const;

    uint8_t GetClassification() const;
    void SetClassification(uint8_t classify);
    
    double GetTime() const;
    void SetTime(double time);

    double& operator[](std::size_t n);
    double const& operator[](std::size_t n) const;

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

inline void LASPoint::SetCoordinates(double x, double y, double z)
{
    m_coords[0] = x;
    m_coords[1] = y;
    m_coords[2] = z;
}

inline double LASPoint::GetX() const
{
    return m_coords[0];
}

inline double LASPoint::GetY() const
{
    return m_coords[1];
}

inline double LASPoint::GetZ() const
{
    return m_coords[2];
}

inline uint16_t LASPoint::GetIntensity() const
{
    return m_intensity;
}

inline void LASPoint::SetIntensity(uint16_t intensity)
{
    m_intensity = intensity;
}

inline uint16_t LASPoint::GetReturnNumber() const
{
    // Read bits 0,1,2
    return (m_flags & 0x07);
}

inline uint16_t LASPoint::GetNumberOfReturns() const
{
    // Read bits 3,4,5
    return ((m_flags >> 3) & 0x07);
}

inline uint16_t LASPoint::GetScanDirection() const
{
    // Read bit 6
    return ((m_flags >> 6) & 0x01);
}

inline uint16_t LASPoint::GetFlightLineEdge() const
{
    // Read bit 7
    return ((m_flags >> 7) & 0x01);
}

inline uint8_t LASPoint::GetScanFlags() const
{
    return m_flags;
}

inline void LASPoint::SetScanFlags(uint8_t flags)
{
    m_flags = flags;
}

inline uint8_t LASPoint::GetClassification() const
{
    return m_class;
}

inline void LASPoint::SetClassification(uint8_t classify)
{
    m_class = classify;
}

inline double LASPoint::GetTime() const
{
    return m_gpsTime;
}

inline void LASPoint::SetTime(double time)
{
    m_gpsTime = time;
}

inline double& LASPoint::operator[](std::size_t n)
{
    if (3 <= n)
        throw_out_of_range();

    return m_coords[n];
}

inline double const& LASPoint::operator[](std::size_t n) const
{
    if (3 <= n)
        throw_out_of_range();

    return m_coords[n];
}

} // namespace liblas

#endif // LIBLAS_LASPOINT_HPP_INCLUDED
