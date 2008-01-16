#ifndef LIBLAS_LASREADER_HPP_INCLUDED
#define LIBLAS_LASREADER_HPP_INCLUDED

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
    class Reader; // Forward declaration
}

class LASReader
{
public:

    LASReader(std::ifstream& ifs);
    ~LASReader();
    
    std::size_t GetVersion() const;

    LASHeader const& GetHeader() const;
    LASPoint const& GetPoint() const;
    
    bool ReadPoint();

private:
    
    //
    // Private data members
    //
    const std::auto_ptr<detail::Reader> m_pimpl;

    LASHeader m_header;
    LASPoint m_point;
    LASPointRecord m_record;

    //
    // Private function members
    //
    
    // Blocked copying operations, declared but not defined.
    LASReader(LASReader const& other);
    LASReader& operator=(LASReader const& rhs);

    // Throws on error
    void Init();

};

} // namespace liblas

#endif // ndef LIBLAS_LASREADER_HPP_INCLUDED
