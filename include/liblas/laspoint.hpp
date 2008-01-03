#ifndef LIBLAS_LASPOINT_HPP_INCLUDED
#define LIBLAS_LASPOINT_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>

namespace liblas
{

struct LASPoint
{
    int x;
    int y;
    int z;
    unsigned short intensity;
    unsigned char flags; // TODO: Replace with portable std::bitset<8>
    unsigned char classification;
    char scan_angle_rank;
    unsigned char user_data;
    unsigned short point_source_id;
};

// TODO:  Replace or wrap the raw Point structure above with more complete version below

//class LASPoint
//{
//public:
//
//    enum ClassificationType
//    {
//        eCreated = 0,
//        eUnclassified,
//        eGround,
//        eLowVegetation,
//        eMediumVegetation,
//        eHighVegetation,
//        eBuilding,
//        eLowPoint,
//        eModelKeyPoint,
//        eWater = 9,
//        // = 10 // reserved for ASPRS Definition
//        // = 11 // reserved for ASPRS Definition
//        eOverlapPoints = 12
//        // = 13-31 // reserved for ASPRS Definition
//    };
//
//    LASPoint(PointData const& data);
//
//    bool operator!() const {
//
//    double GetX() const;
//    double GetY() const;
//    double GetZ() const;
//
//    uint16_t GetIntensity() const;
//    double GetTime() const;
//    ClassificationType GetClassification() const;
//
//private:
//    
//    PointData const& m_data;
//    bool m_isset;
//};
//
//LASPoint::LASPoint(PointData const& data) : m_data(data), m_isset(false)
//{
//}
//
//inline double LASPoint::GetX() const
//{
//    return m_pt.x;
//}
//
//inline double LASPoint::GetY() const
//{
//    return m_pt.y;
//}
//
//inline double LASPoint::GetZ() const
//{
//    return m_pt.z;
//}
//
//inline uint16_t LASPoint::GetIntensity() const
//{
//    return m_intensity;
//}
//inline double LASPoint::GetTime() const
//{
//    return m_gpsTime;
//}
//inline LASPoint::ClassificationType LASPoint::GetClassification() const
//{
//    return m_class;
//}

} // namespace liblas

#endif // LIBLAS_LASPOINT_HPP_INCLUDED
