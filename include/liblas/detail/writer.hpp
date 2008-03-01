// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_DETAIL_WRITER_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER_HPP_INCLUDED

#include <liblas/detail/fwd.hpp>
// std
#include <iosfwd>

namespace liblas { namespace detail {

class Writer
{
public:

    Writer();
    virtual ~Writer();
    virtual std::size_t GetVersion() const = 0;
    virtual void WriteHeader(LASHeader const& header) = 0;
    virtual void UpdateHeader(LASHeader const& header) = 0;
    virtual void WritePointRecord(PointRecord const& record) = 0;
    virtual void WritePointRecord(PointRecord const& record, double const& time) = 0;

    // TODO: fix constness
    virtual std::ostream& GetStream() = 0;

private:

    // Blocked copying operations, declared but not defined.
    Writer(Writer const& other);
    Writer& operator=(Writer const& rhs);
};

class WriterFactory
{
public:

    static Writer* Create(std::ostream& ofs, LASHeader const& header);
    static void Destroy(Writer* p);
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_WRITER_HPP_INCLUDED
