#ifndef LIBLAS_LASWRITER_HPP_INCLUDED
#define LIBLAS_LASWRITER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/sharedptr.hpp>
// std
#include <fstream>
#include <string>
#include <memory>
#include <cstdlib> // std::size_t

namespace liblas
{

namespace detail {
    class Writer; // Forward declaration
}

class LASWriter
{
public:

    LASWriter(std::ofstream& ofs, LASHeader const& header);
    ~LASWriter();
    
    std::size_t GetVersion() const;

private:
    
    //
    // Private data members
    //
    const std::auto_ptr<detail::Writer> m_pimpl;

    LASHeader m_header;
    LASPoint m_point;
    LASPointRecord m_record;

    //
    // Private function members
    //
    
    // Blocked copying operations, declared but not defined.
    LASWriter(LASWriter const& other);
    LASWriter& operator=(LASWriter const& rhs);

    // Throws on error
    //void Init();

};

} // namespace liblas

#endif // ndef LIBLAS_LASWRITER_HPP_INCLUDED
