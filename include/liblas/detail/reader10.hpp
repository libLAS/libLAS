#ifndef LIBLAS_DETAIL_READER10_HPP_INCLUDED
#define LIBLAS_DETAIL_READER10_HPP_INCLUDED

#include <liblas/detail/reader.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <iosfwd>

namespace liblas { namespace detail { namespace v10 {

class ReaderImpl : public Reader
{
public:

    typedef Reader Base;
    
    ReaderImpl(std::ifstream& ifs);
    std::size_t GetVersion() const;
    bool ReadHeader(LASHeader& header);
    bool ReadNextPoint(PointRecord& record);
    bool ReadNextPoint(PointRecord& record, double& time);
    bool ReadPointAt(std::size_t n, PointRecord& record);
    bool ReadPointAt(std::size_t n, PointRecord& record, double& time);

private:

    std::ifstream& m_ifs;
};

}}} // namespace liblas::detail::v10

#endif // LIBLAS_DETAIL_READER10_HPP_INCLUDED
