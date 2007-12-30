#ifndef LIBLAS_READER_HPP_INCLUDED
#define LIBLAS_READER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>

namespace liblas
{

class Reader;

class ReaderFactory
{
public:

    // TODO: prototypes
    static Reader* create(std::string const& file);
    static Reader* destroy(Reader* p);

private:
};

class Reader
{
public:
    
    // TODO: prototypes
    virtual ~Reader();

private:
};

};

} // namespace liblas

#endif // LIBLAS_READER_HPP_INCLUDED
