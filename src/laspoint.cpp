#include <liblas/laspoint.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>

namespace liblas {

LASPoint::LASPoint() :
    m_pt(), m_intensity(0), m_flags(0), m_class(0), m_gpsTime(0)
{
    // constructed
}

LASPoint::LASPoint(LASPoint const& other) :
    m_pt(other.m_pt), m_intensity(other.m_intensity),
        m_flags(other.m_flags), m_class(other.m_class),
            m_gpsTime(other.m_gpsTime)
{
    // copied
}

LASPoint& LASPoint::operator=(LASPoint const& rhs)
{
    if (&rhs != this)
    {
        m_pt = rhs.m_pt;
        m_intensity = rhs.m_intensity;
        m_flags = rhs.m_flags;
        m_class = rhs.m_class;
        m_gpsTime = rhs.m_gpsTime;
    }
    return *this;
}

} // namespace liblas
