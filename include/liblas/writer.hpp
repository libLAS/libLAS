#ifndef LIBLAS_WRITER_HPP_INCLUDED
#define LIBLAS_WRITER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>

namespace liblas
{

class Writer;

class WriterFactory
{
public:

    // TODO: prototypes
    static Writer* create(std::string const& file, LASHeader const& header);
    static Writer* destroy(Writer* p);

private:
};

class Writer
{
public:

    // TODO: prototypes
    virtual ~Writer();
    virtual void WritePoint(LASPoint const& point) = 0;

private:
};

};

} // namespace liblas

#endif // LIBLAS_WRITER_HPP_INCLUDED
