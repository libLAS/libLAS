#include <liblas/laspoint.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
// std
#include <cstring>

namespace liblas {

LASPoint::LASPoint() :
    m_intensity(0), m_flags(0), m_class(0), m_gpsTime(0)
{
    std::memset(m_coords, 0, coords_size);
}

LASPoint::LASPoint(LASPoint const& other) :
    m_intensity(other.m_intensity),
        m_flags(other.m_flags), m_class(other.m_class),
            m_gpsTime(other.m_gpsTime)
{
    std::memcpy(m_coords, other.m_coords, coords_size);
}

LASPoint& LASPoint::operator=(LASPoint const& rhs)
{
    if (&rhs != this)
    {
        std::memcpy(m_coords, rhs.m_coords, coords_size);
        m_intensity = rhs.m_intensity;
        m_flags = rhs.m_flags;
        m_class = rhs.m_class;
        m_gpsTime = rhs.m_gpsTime;
    }
    return *this;
}

} // namespace liblas
