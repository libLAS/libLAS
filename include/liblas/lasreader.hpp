// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_LASREADER_HPP_INCLUDED
#define LIBLAS_LASREADER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <iosfwd>
#include <string>
#include <memory>
#include <cstdlib> // std::size_t

namespace liblas
{

/// \todo To be documented.
class LASReader
{
public:

    LASReader(std::istream& ifs);
    ~LASReader();
    
    std::size_t GetVersion() const;
    LASHeader const& GetHeader() const;
    LASPoint const& GetPoint() const;
    
    bool ReadNextPoint();
    bool ReadPointAt(std::size_t n);

    // The operator is not const because it updates file stream position.
    LASPoint const& operator[](std::size_t n);

private:

    // Blocked copying operations, declared but not defined.
    LASReader(LASReader const& other);
    LASReader& operator=(LASReader const& rhs);

    void Init(); // throws on error
    void MakePoint(double const& time);

    const std::auto_ptr<detail::Reader> m_pimpl;
    LASHeader m_header;
    LASPoint m_point;
    detail::PointRecord m_record;

};

} // namespace liblas

#endif // ndef LIBLAS_LASREADER_HPP_INCLUDED
