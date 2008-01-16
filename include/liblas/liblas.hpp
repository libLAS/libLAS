#ifndef LIBLAS_HPP_INCLUDED
#define LIBLAS_HPP_INCLUDED

#include <liblas/cstdint.hpp>

namespace liblas
{

enum LASFileVersion
{
    eLASVersion10 = 1 * 100000 + 0,
    eLASVersion11 = 1 * 100000 + 1,
    eLASVersion20 = 2 * 100000 + 0
};

} // namespace liblas

#endif // ndef LIBLAS_HPP_INCLUDED
