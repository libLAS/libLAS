#ifndef LIBLAS_LASREADER_HPP_INCLUDED
#define LIBLAS_LASREADER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <fstream>
#include <string>
#include <memory>
#include <cstdlib> // std::size_t

namespace liblas
{

// Forward declarations
namespace detail {
    class Reader;
    struct PointRecord;
}

class LASReader
{
public:

    LASReader(std::ifstream& ifs);
    ~LASReader();
    
    std::size_t GetVersion() const;

    LASHeader const& GetHeader() const;
    LASPoint const& GetPoint() const;
    
    bool ReadNextPoint();

private:
    
    //
    // Private data members
    //
    const std::auto_ptr<detail::Reader> m_pimpl;

    LASHeader m_header;
    LASPoint m_point;
    detail::PointRecord m_record;

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
