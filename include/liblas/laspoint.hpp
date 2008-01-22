#ifndef LIBLAS_LASPOINT_HPP_INCLUDED
#define LIBLAS_LASPOINT_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>

namespace liblas {

// TODO: HIGH PRIORITY: Make record type specific to LAS version and Point Data Format
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

    uint16_t GetReturnNumber() const;
    uint16_t GetNumberOfReturns() const;
    uint16_t GetScanDirection() const;
    uint16_t GetFlightLineEdge() const;

    uint8_t GetScanFlags() const;
    void SetScanFlags(uint8_t flags);

    uint8_t GetClassification() const;
    void SetClassification(uint8_t classify);
    
    double GetTime() const;

private:

    detail::Point<double> m_pt;
    uint16_t m_intensity;
    uint8_t m_flags;
    uint8_t m_class;
    double m_gpsTime;
};

inline void LASPoint::SetCoordinates(double x, double y, double z)
{
    m_pt.x = x;
    m_pt.y = y;
    m_pt.z = z;
}

inline double LASPoint::GetX() const
{
    return m_pt.x;
}

inline double LASPoint::GetY() const
{
    return m_pt.y;
}

inline double LASPoint::GetZ() const
{
    return m_pt.z;
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

inline double LASPoint::GetTime() const
{
    return m_gpsTime;
}

inline uint8_t LASPoint::GetClassification() const
{
    return m_class;
}

inline void LASPoint::SetClassification(uint8_t classify)
{
    m_class = classify;
}

} // namespace liblas

#endif // LIBLAS_LASPOINT_HPP_INCLUDED
