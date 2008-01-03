#ifndef LIBLAS_DETAIL_READER_HPP_INCLUDED
#define LIBLAS_DETAIL_READER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <iosfwd>
#include <string>

namespace liblas { namespace detail {

class Reader
{
public:

    Reader();
    virtual ~Reader() {}
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

namespace v10 {
class ReaderImpl : public Reader
{
public:

    typedef Reader Base;
    
    ReaderImpl(std::ifstream& ifs);
    std::size_t GetVersion() const;
    bool ReadHeader(LASHeader& header);
    bool ReadPoint(LASPointRecord& point);

private:

    std::ifstream& m_ifs;
};
} // namespace v10

namespace v11 {
class ReaderImpl// : public Reader
{
public:
    ReaderImpl();
};
} // namespace v11


}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_READER_HPP_INCLUDED
