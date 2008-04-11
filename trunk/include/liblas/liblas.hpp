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
#include <fstream>
#include <string>

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

/// Open file to read in binary mode.
/// The input file is also attached to input stream.
/// \param ifs - reference to input file stream to
/// which opened file is attached
/// \param filename - name of file to open
/// \return true if file has been opened with success, false otherwise
/// \exception No throw
///
inline bool Open(std::ifstream& ifs, std::string const& filename) // throw()
{
    ifs.open(filename.c_str(), std::ios::in | std::ios::binary);
    return ifs.is_open();
}

/// Create file and open to write in binary mode.
/// The output file is also attached to output stream.
/// \param ofs - reference to output file stream to
/// which created file is attached
/// \param filename - name of file to open
/// \return true if file has been create with success, false otherwise
/// \exception No throw
///
inline bool Create(std::ofstream& ofs, std::string const& filename) // throw()
{
    ofs.open(filename.c_str(), std::ios::out | std::ios::binary);
    return ofs.is_open();
}

} // namespace liblas

#endif // LIBLAS_HPP_INCLUDED
