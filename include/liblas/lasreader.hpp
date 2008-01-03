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

namespace detail {
    class Reader; // Forward declaration
}

class LASReader
{
public:
    LASReader(std::string const& file);
    LASReader(std::ifstream& ifs);
    ~LASReader();
    
    std::size_t GetVersion() const;

    LASHeader const& GetHeader() const;
    LASPoint const& GetPoint() const;
    
    bool ReadPoint();

private:
    
    const std::auto_ptr<detail::Reader> m_pimpl;
    std::ifstream m_ifs; // used only if constructed with file path

    LASHeader m_header;
    LASPoint m_point;
};

} // namespace liblas

#endif // ndef LIBLAS_LASREADER_HPP_INCLUDED
