// $Id$
//
// (C) Copyright Howard Butler 2008
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_EXCEPTION_HPP_INCLUDED
#define LIBLAS_EXCEPTION_HPP_INCLUDED

#include <stdexcept>
#include <string>

namespace liblas {

/// Exception reporting invalid point data.
/// It's usually thrown by LASPoint::Validate function.
class invalid_point_data : public std::runtime_error
{
public:

    invalid_point_data(std::string const& msg, unsigned int who)
        : std::runtime_error(msg), m_who(who)
    {}

    /// Return flags identifying invalid point data members.
    /// Flags are composed with composed with LASPoint::DataMemberFlag.
    /// Testing flags example: bool timeValid = e.who() & eTime;
    unsigned int who() const
    {
        return m_who;
    }

private:

    unsigned int m_who;
};

} // namespace liblas

#endif // LIBLAS_EXCEPTION_HPP_INCLUDED

