// $Id$
//
// (C) Copyright Howard Butler 2008
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// (C) Copyright Phil Vachon 2008, philippe@cowpig.ca
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_HPP_INCLUDED
#define LIBLAS_HPP_INCLUDED

#include <liblas/cstdint.hpp>

/// Namespace grouping all elements of libLAS public interface.
/// \note
/// User's may notice there is namespace \em detail nested
/// in the \em liblas. The \em detail should be considered as
/// private namespace dedicated for implementation details,
/// so libLAS are not supposed to access it directly,
/// nor included headers from the \em detail subdirectory of \em liblas include folder.
namespace liblas
{

/// Version numbers of the ASPRS LAS Specification.
/// Numerical representation of versions is calculated according to 
/// following formula: <em>major * 100000 + minor</em>
enum LASFileVersion
{
    eLASVersion10 = 1 * 100000 + 0, ///< LAS Format 1.0
    eLASVersion11 = 1 * 100000 + 1, ///< LAS Format 1.1
    eLASVersion20 = 2 * 100000 + 0  ///< LAS Format 2.0
};

} // namespace liblas

#endif // LIBLAS_HPP_INCLUDED
