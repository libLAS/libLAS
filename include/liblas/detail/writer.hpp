#ifndef LIBLAS_DETAIL_WRITER_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>

namespace liblas { namespace detail {

class Writer
{
public:

    // TODO: prototypes
    Writer();
    virtual ~Writer();
    virtual std::size_t GetVersion() const = 0;
    virtual bool WriteHeader(LASHeader const& header) = 0;
    virtual bool WritePoint(LASPoint const& point) = 0;

private:
};

class WriterFactory
{
public:

    // TODO: prototypes
    static Writer* Create(std::ofstream& ofs, LASHeader const& header);
    static void Destroy(Writer* p);

private:
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_WRITER_HPP_INCLUDED
