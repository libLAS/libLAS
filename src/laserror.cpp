// $Id$
//
// (C) Copyright Howard Butler 2007
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/laserror.hpp>
#include <liblas/cstdint.hpp>
//std
#include <string>

namespace liblas
{

LASError::LASError(int code, std::string const& message, std::string const& method) :
    m_code(code),
    m_message(message),
    m_method(method)
{
}

LASError::LASError(LASError const& other) :
    m_code(other.m_code),
    m_message(other.m_message),
    m_method(other.m_method)
{
}

LASError& LASError::operator=(LASError const& rhs)
{
    if (&rhs != this)
    {
        m_code = rhs.m_code;
        m_message = rhs.m_message;
        m_method = rhs.m_method;

    }
    return *this;
}

} // namespace liblas
