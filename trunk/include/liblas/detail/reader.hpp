// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_DETAIL_READER_HPP_INCLUDED
#define LIBLAS_DETAIL_READER_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <iosfwd>

namespace liblas { namespace detail {

struct PointRecord;

class Reader
{
public:

    Reader();
    virtual ~Reader();
    virtual std::size_t GetVersion() const = 0;
    virtual bool ReadHeader(LASHeader& header) = 0;
    virtual bool ReadNextPoint(PointRecord& record) = 0;
    virtual bool ReadNextPoint(PointRecord& record, double& time) = 0;
    virtual bool ReadPointAt(std::size_t n, PointRecord& record) = 0;
    virtual bool ReadPointAt(std::size_t n, PointRecord& record, double& time) = 0;

    virtual std::istream& GetStream() = 0;
    
protected:
    
    std::streamoff m_offset;
    uint32_t m_size;
    uint32_t m_current;

private:

    // Blocked copying operations, declared but not defined.
    Reader(Reader const& other);
    Reader& operator=(Reader const& rhs);
};

class ReaderFactory
{
public:

    // TODO: prototypes
    static Reader* Create(std::istream& ifs);
    static void Destroy(Reader* p);
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_READER_HPP_INCLUDED
