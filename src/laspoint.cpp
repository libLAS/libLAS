#include <liblas/laspoint.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>

namespace liblas {

LASPoint::LASPoint(LASHeader const& header) : m_header(header)
{
}

} // namespace liblas
