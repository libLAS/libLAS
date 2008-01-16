#ifndef LIBLAS_DETAIL_READER_HPP_INCLUDED
#define LIBLAS_DETAIL_READER_HPP_INCLUDED

#include <liblas/cstdint.hpp>
// std
#include <iosfwd>

namespace liblas {

// Forward declarations
class LASHeader;
class LASPoint;
struct LASPointRecord;

namespace detail {

class Reader
{
public:

    Reader();
    virtual ~Reader();
    virtual std::size_t GetVersion() const = 0;
    virtual bool ReadHeader(LASHeader& header) = 0;
    virtual bool ReadPoint(LASPointRecord& point) = 0;

protected:
    
    std::streamoff m_offset;
    uint32_t m_size;
    uint32_t m_current;
};

class ReaderFactory
{
public:

    // TODO: prototypes
    static Reader* Create(std::ifstream& ifs);
    static void Destroy(Reader* p);
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_READER_HPP_INCLUDED
