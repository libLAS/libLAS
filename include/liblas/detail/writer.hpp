#ifndef LIBLAS_DETAIL_WRITER_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER_HPP_INCLUDED

#include <iosfwd>

namespace liblas {

// Forward declarations
class LASHeader;
class LASPoint;
struct LASPointRecord;
    
namespace detail {

class Writer
{
public:

    Writer();
    virtual ~Writer();
    virtual std::size_t GetVersion() const = 0;
    virtual void WriteHeader(LASHeader const& header) = 0;
    virtual void UpdateHeader(LASHeader const& header) = 0;
    virtual void WritePointRecord(LASPointRecord const& record) = 0;
    virtual void WritePointRecord(LASPointRecord const& record, double const& time) = 0;

private:
};

class WriterFactory
{
public:

    static Writer* Create(std::ofstream& ofs, LASHeader const& header);
    static void Destroy(Writer* p);
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_WRITER_HPP_INCLUDED
