#ifndef LIBLAS_DETAIL_WRITER10_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER10_HPP_INCLUDED

#include <liblas/detail/writer.hpp>
#include <liblas/cstdint.hpp>
// std
#include <iosfwd>

namespace liblas {

// Forward declarations
class LASHeader;
class LASPoint;

namespace detail { namespace v10 {

class WriterImpl : public Writer
{
public:

    typedef Writer Base;
    
    WriterImpl(std::ofstream& ofs);
    std::size_t GetVersion() const;
    void WriteHeader(LASHeader const& header);
    void UpdateHeader(LASHeader const& header);
    void WritePointRecord(LASPointRecord const& record);

private:

    std::ofstream& m_ofs;
    liblas::uint32_t m_pointCount;
};

}}} // namespace liblas::detail::v10

#endif // LIBLAS_DETAIL_WRITER10_HPP_INCLUDED
