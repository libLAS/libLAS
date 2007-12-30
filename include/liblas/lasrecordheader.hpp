#ifndef LIBLAS_LASRECORDHEADER_HPP_INCLUDED
#define LIBLAS_LASRECORDHEADER_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <string>

namespace liblas
{

class LASRecordHeader
{
public:

    uint16_t GetReserved() const;
    std::string const& GetUserId() const;
    uint16_t GetRecordId() const;
    uint16_t GeRecordLength() const;
    std::string const& GetDescription() const;

private:

    uint16_t m_reserved;
    std::string m_userId; // [16]
    uint16_t m_recordId;
    uint16_t m_recordLength; // after header
    std::string m_desc; // [32]
};

} // namespace liblas

#endif // LIBLAS_LASRECORDHEADER_HPP_INCLUDED
