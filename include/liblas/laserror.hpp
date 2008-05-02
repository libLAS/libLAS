// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// (C) Copyright Phil Vachon 2007, philippe@cowpig.ca
// (C) Copyright Howard Butler 2007
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_LASERROR_HPP_INCLUDED
#define LIBLAS_LASERROR_HPP_INCLUDED

//std
#include <iosfwd>
#include <string>

namespace liblas {

/// Definition of error notification used on the level of C API.
/// This class describes details of error condition occured in
/// libLAS core. All errors are stacked by C API layer, so it's
/// possible to track problem down to its source.
class LASError
{
public:

    /// Custom constructor.
    /// This is the main and the only tool to initialize error instance.
    LASError(int code, std::string const& message, std::string const& method);

    /// Copy constructor.
    /// Error objects are copyable.
    LASError(LASError const& other);

    /// Assignment operator.
    /// Error objects are assignable.
    LASError& operator=(LASError const& rhs);

    // TODO - mloskot: What about replacing string return by copy with const char* ?
    //        char const* GetMethod() const { return m_method.c_str(); }, etc.

    int GetCode() const { return m_code; };
    std::string GetMessage() const { return m_message; };
    std::string GetMethod() const { return m_method; };    

private:

    int m_code;
    std::string m_message;
    std::string m_method;
};

} // namespace liblas

#endif
