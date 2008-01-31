#include <liblas/laspoint.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
// std
#include <cstring>

namespace liblas {

LASPoint::LASPoint() :
    m_intensity(0), m_flags(0), m_class(0), m_gpsTime(0)
{
    std::memset(m_coords, 0, sizeof(m_coords));
}

LASPoint::LASPoint(LASPoint const& other) :
    m_intensity(other.m_intensity),
        m_flags(other.m_flags), m_class(other.m_class),
            m_gpsTime(other.m_gpsTime)
{
    std::memcpy(m_coords, other.m_coords, sizeof(m_coords));
}

LASPoint& LASPoint::operator=(LASPoint const& rhs)
{
    printf("in LASPoint& LASPoint::operator=...\n");
    if (&rhs != this)
    {
        m_coords[0] = rhs.m_coords[0];
        m_coords[1] = rhs.m_coords[1];
        m_coords[2] = rhs.m_coords[2];
        m_intensity = rhs.m_intensity;
        m_flags = rhs.m_flags;
        m_class = rhs.m_class;
        m_gpsTime = rhs.m_gpsTime;
    }
    return *this;
}

void LASPoint::SetCoordinates(LASHeader const& header, double x, double y, double z)
{
    double const cx = x * header.GetScaleX() + header.GetOffsetX();
    double const cy = y * header.GetScaleY() + header.GetOffsetY();
    double const cz = z * header.GetScaleZ() + header.GetOffsetZ();

    SetCoordinates(cx, cy, cz);
}

bool LASPoint::equal(LASPoint const& other) const
{
    double const epsilon = std::numeric_limits<double>::epsilon(); 

    double const dx = m_coords[0] - other.m_coords[0];
    double const dy = m_coords[1] - other.m_coords[1];
    double const dz = m_coords[2] - other.m_coords[2];

    // TODO: Should we compare other data members, besides the coordinates?

    if (((dx <= epsilon) && (dx >= -epsilon ))
        || ((dy <= epsilon) && (dy >= -epsilon ))
        || ((dz <= epsilon) && (dz >= -epsilon )))
    {
        return true;
    }

    return false;
}

} // namespace liblas
