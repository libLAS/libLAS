#ifndef LIBLAS_DETAIL_READER11_HPP_INCLUDED
#define LIBLAS_DETAIL_READER11_HPP_INCLUDED

#include <liblas/detail/reader.hpp>
// std
#include <iosfwd>

namespace liblas {

// Forward declarations
class LASHeader;
class LASPoint;
namespace detail {
    struct LASPointRecord;
}

namespace detail { namespace v11 {

class ReaderImpl : public Reader
{
public:

    typedef Reader Base;
    
    ReaderImpl(std::ifstream& ifs);
    std::size_t GetVersion() const;
    bool ReadHeader(LASHeader& header);
    bool ReadNextPoint(detail::PointRecord& point);
    bool ReadNextPoint(detail::PointRecord& point, double& time);

private:

    std::ifstream& m_ifs;
};

}}} // namespace liblas::detail::v11

#endif // LIBLAS_DETAIL_READER11_HPP_INCLUDED
