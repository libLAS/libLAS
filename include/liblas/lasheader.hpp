#ifndef LIBLAS_LASHEADER_HPP_INCLUDED
#define LIBLAS_LASHEADER_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/utility.hpp>
#include <string>
#include <vector>

namespace liblas
{

class LASHeader
{
public:

    typedef Coord<double> PointScales;
    typedef Coord<double> PointOffsets;
    typedef Extents<double> PointExtents;

    std::string GetFileSignature() const;
    uint16_t GetFileSourceId() const;
    uint16_t GetReserved() const;
    
    uint32_t GetProjectId1() const;
    uint16_t GetProjectId2() const;
    uint16_t GetProjectId3() const;
    std::string GetProjectId4() const;

    uint8_t GetVersionMajor() const;
    uint8_t GetVersionMinor() const;

    std::string GetSystemId() const;
    std::string GetSoftwareId() const;

    uint16_t GetCreationDOY() const;
    uint16_t GetCreationYear() const;

    uint16_t GetHeaderSize() const;
    uint32_t GetDataOffset() const;
    uint32_t GetRecordsCount() const;
    uint8_t GetDataFormatId() const;
    uint16_t GetDataRecordLength() const;
    uint32_t GetPointRecordsCount() const;
    
    // TODO: design it in user-friendly way
    //std::vector<uint32_t> const& GetPointRecordsByReturnCount() const;
    
    PointScales const& GetScales() const;
    PointOffsets const& GetOffsets() const;
    PointExtents const& GetExtents() const;

private:

    // TODO: replcae static-size char arrays with std::string
    //       and return const-reference to string object.

    char m_signature[4];
    uint16_t m_sourceId;
    uint16_t m_reserved;
    uint32_t m_projectId1;
    uint16_t m_projectId2;
    uint16_t m_projectId3;
    uint8_t m_projectId4[8];
    uint8_t m_versionMajor;
    uint8_t m_versionMinor;
    char m_systemId[32];
    char m_softwareId[32];
    uint16_t m_createDOY;
    uint16_t m_createYear;
    uint16_t m_headerSize;
    uint32_t m_dataOffset;
    uint32_t m_recordsCount;
    uint8_t m_dataFormatId;
    uint16_t m_dataRecordLen;
    uint32_t m_pointRecordsCount;
    
    //uint32_t m_pointRecordsByReturn[5];
    std::vector<uint32_t> m_pointRecordsByReturn;

    PointScales m_scales;
    PointOffsets m_offsets;
    PointExtents m_extents;
};

} // namespace liblas

#endif // LIBLAS_LASHEADER_HPP_INCLUDED
