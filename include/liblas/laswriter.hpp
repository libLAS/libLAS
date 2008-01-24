#ifndef LIBLAS_LASWRITER_HPP_INCLUDED
#define LIBLAS_LASWRITER_HPP_INCLUDED

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/fwd.hpp>
// std
#include <iosfwd> // std::ofstream
#include <string>
#include <memory>
#include <cstdlib> // std::size_t

namespace liblas
{

class LASWriter
{
public:

    LASWriter(std::ofstream& ofs, LASHeader const& header);
    ~LASWriter();
    
    std::size_t GetVersion() const;
    bool WritePoint(LASPoint const& point);

private:
    
    // Blocked copying operations, declared but not defined.
    LASWriter(LASWriter const& other);
    LASWriter& operator=(LASWriter const& rhs);

    const std::auto_ptr<detail::Writer> m_pimpl;

    LASHeader m_header;
    detail::PointRecord m_record;
};

} // namespace liblas

#endif // ndef LIBLAS_LASWRITER_HPP_INCLUDED
