#ifndef LIBLAS_DETAIL_WRITER10_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER10_HPP_INCLUDED

#include <liblas/detail/writer.hpp>
// std
#include <iosfwd>

namespace liblas {

// Forward declarations
class LASHeader;

namespace detail { namespace v10 {

class WriterImpl : public Writer
{
public:

    typedef Writer Base;
    
    WriterImpl(std::ofstream& ofs);
    std::size_t GetVersion() const;
    bool WriteHeader(LASHeader const& header);
    bool WritePoint(LASPoint const& point);

private:

    std::ofstream& m_ofs;
};

}}} // namespace liblas::detail::v10

#endif // LIBLAS_DETAIL_WRITER10_HPP_INCLUDED
