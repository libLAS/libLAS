// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_DETAIL_READER11_HPP_INCLUDED
#define LIBLAS_DETAIL_READER11_HPP_INCLUDED

#include <liblas/detail/reader.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <iosfwd>

namespace liblas { namespace detail { namespace v11 {

class ReaderImpl : public Reader
{
public:

    typedef Reader Base;
    
    ReaderImpl(std::istream& ifs);
    std::size_t GetVersion() const;
    bool ReadHeader(LASHeader& header);
    bool ReadNextPoint(detail::PointRecord& record);
    bool ReadNextPoint(detail::PointRecord& record, double& time);
    bool ReadPointAt(std::size_t n, PointRecord& record);
    bool ReadPointAt(std::size_t n, PointRecord& record, double& time);

private:

    std::istream& m_ifs;
};

}}} // namespace liblas::detail::v11

#endif // LIBLAS_DETAIL_READER11_HPP_INCLUDED
