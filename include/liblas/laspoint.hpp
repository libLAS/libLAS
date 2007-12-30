#ifndef LIBLAS_LASPOINT_HPP_INCLUDED
#define LIBLAS_LASPOINT_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/utility.hpp>

namespace liblas
{

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

    LASPoint(double x, double y, double z, uint16_t intensity, double gpsTime);

    double GetX() const;
    double GetY() const;
    double GetZ() const;

    uint16_t GetIntensity() const;
    double GetTime() const;
    ClassificationType GetClassification() const;

private:
    
    Point<double> m_pt;
    uint16_t m_intensity;
    double m_gpsTime;
    ClassificationType m_class;
};

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
inline double LASPoint::GetTime() const
{
    return m_gpsTime;
}
inline LASPoint::ClassificationType LASPoint::GetClassification() const
{
    return m_class;
}

} // namespace liblas

#endif // LIBLAS_LASPOINT_HPP_INCLUDED
