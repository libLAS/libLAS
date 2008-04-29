// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_LASWRITER_HPP_INCLUDED
#define LIBLAS_LASWRITER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <iosfwd> // std::ostream
#include <string>
#include <memory>
#include <cstdlib> // std::size_t

namespace liblas
{

/// \todo To be documented.
class LASWriter
{
public:

    LASWriter(std::ostream& ofs, LASHeader const& header);
    ~LASWriter();
    
    std::size_t GetVersion() const;
    LASHeader const& GetHeader() const;
    bool WritePoint(LASPoint const& point);

    // Allow fetching of the stream
    std::ostream& GetStream();
    
    // Allow in-place writing of header
    void WriteHeader(LASHeader const& header);
    
private:
    
    // Blocked copying operations, declared but not defined.
    LASWriter(LASWriter const& other);
    LASWriter& operator=(LASWriter const& rhs);

    const std::auto_ptr<detail::Writer> m_pimpl;

    LASHeader m_header;
    detail::PointRecord m_record;
};

} // namespace liblas

#endif // ndef LIBLAS_LASWRITER_HPP_INCLUDED
