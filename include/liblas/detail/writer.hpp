#ifndef LIBLAS_DETAIL_WRITER_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>

namespace liblas { namespace detail {

class Writer
{
public:

    // TODO: prototypes
    virtual ~Writer();
    virtual void WritePoint(LASPoint const& point) = 0;

private:
};

class WriterFactory
{
public:

    // TODO: prototypes
    static Writer* create(std::string const& file, LASHeader const& header);
    static Writer* destroy(Writer* p);

private:
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_WRITER_HPP_INCLUDED
