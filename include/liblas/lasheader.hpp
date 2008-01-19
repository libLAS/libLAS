#ifndef LIBLAS_LASHEADER_HPP_INCLUDED
#define LIBLAS_LASHEADER_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
//std
#include <iosfwd>
#include <string>
#include <vector>

namespace liblas {

class LASHeader
{
public:

    LASHeader();
    LASHeader(LASHeader const& other);
    LASHeader& operator=(LASHeader const& rhs);

    std::string GetFileSignature() const;
    void SetFileSignature(std::string const& v);

    uint16_t GetFileSourceId() const;
    void SetFileSourceId(uint16_t const& v);

    uint16_t GetReserved() const;

    // TODO: Add Set* functions
    uint32_t GetProjectId1() const;
    uint16_t GetProjectId2() const;
    uint16_t GetProjectId3() const;
    std::string GetProjectId4() const;

    uint8_t GetVersionMajor() const;
    void SetVersionMajor(uint8_t const& v);

    uint8_t GetVersionMinor() const;
    void SetVersionMinor(uint8_t const& v);

    // TODO: Add Set* functions
    std::string GetSystemId() const;
    std::string GetSoftwareId() const;

    uint16_t GetCreationDOY() const;
    uint16_t GetCreationYear() const;

    uint16_t GetHeaderSize() const;
    uint32_t GetDataOffset() const;
    uint32_t GetRecordsCount() const;
    uint8_t  GetDataFormatId() const;
    uint16_t GetDataRecordLength() const;
    uint32_t GetPointRecordsCount() const;
    
    std::vector<uint32_t> const& GetPointRecordsByReturnCount() ;
    
    double GetScaleX() const;
    double GetScaleY() const;
    double GetScaleZ() const;
    double GetOffsetX() const;
    double GetOffsetY() const;
    double GetOffsetZ() const;
    double GetMaxX() const;
    double GetMinX() const;
    double GetMaxY() const;
    double GetMinY() const;
    double GetMaxZ() const;
    double GetMinZ() const;

    void Read(std::ifstream& ifs);

private:
    
    typedef detail::Point<double> PointScales;
    typedef detail::Point<double> PointOffsets;
    typedef detail::Extents<double> PointExtents;

    enum
    {
        eVersionMinorMin = 0,
        eVersionMinorMax = 1,
        eVersionMajorMin = 1,
        eVersionMajorMax = 2,
        eSignatureSize = 4,
        ePointsByReturnSize = 5,
        eProjectId4Size = 8,
        eSystemIdSize = 32,
        eSoftwareIdSize = 32
    };
    
    // TODO: replace static-size char arrays with std::string
    //       and return const-reference to string object.
    
    //
    // Private function members
    //
    void Init();

    //
    // Private data members
    //
    char m_signature[eSignatureSize];
    uint16_t m_sourceId;
    uint16_t m_reserved;
    uint32_t m_projectId1;
    uint16_t m_projectId2;
    uint16_t m_projectId3;
    uint8_t m_projectId4[eProjectId4Size];
    uint8_t m_versionMajor;
    uint8_t m_versionMinor;
    char m_systemId[eSystemIdSize];
    char m_softwareId[eSoftwareIdSize];
    uint16_t m_createDOY;
    uint16_t m_createYear;
    uint16_t m_headerSize;
    uint32_t m_dataOffset;
    uint32_t m_recordsCount;
    uint8_t m_dataFormatId;
    uint16_t m_dataRecordLen;
    uint32_t m_pointRecordsCount;
    std::vector<uint32_t> m_pointRecordsByReturn;
    PointScales m_scales;
    PointOffsets m_offsets;
    PointExtents m_extents;
};

} // namespace liblas

#endif // LIBLAS_LASHEADER_HPP_INCLUDED
