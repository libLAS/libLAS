#ifndef LIBLAS_LASHEADER_HPP_INCLUDED
#define LIBLAS_LASHEADER_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/guid.hpp>
#include <liblas/detail/utility.hpp>
//std
#include <iosfwd>
#include <string>
#include <vector>

namespace liblas {

class LASHeader
{
public:

    enum PointFormat
    {
        ePointFormat0 = 0,
        ePointFormat1 = 1
    };

    LASHeader();
    LASHeader(LASHeader const& other);
    LASHeader& operator=(LASHeader const& rhs);

    std::string GetFileSignature() const;
    void SetFileSignature(std::string const& v);

    uint16_t GetFileSourceId() const;
    void SetFileSourceId(uint16_t const& v);

    uint16_t GetReserved() const;

    // TODO: Replace GUID data calls with single Get/Set using liblas::guid type
    guid GetProjectId() const;
    void SetProjectId(guid const& v);

    uint8_t GetVersionMajor() const;
    void SetVersionMajor(uint8_t const& v);

    uint8_t GetVersionMinor() const;
    void SetVersionMinor(uint8_t const& v);

    std::string GetSystemId() const;
    void SetSystemId(std::string const& v);

    std::string GetSoftwareId() const;
    void SetSoftwareId(std::string const& v);

    // TODO: Replace these 2 calls with more general version: Set/GetCreationDate(date)
    uint16_t GetCreationDOY() const;
    void SetCreationDOY(uint16_t const& v);

    uint16_t GetCreationYear() const;
    void SetCreationYear(uint16_t const&v);

    uint16_t GetHeaderSize() const;
    uint32_t GetDataOffset() const;
    uint32_t GetRecordsCount() const;
    
    PointFormat GetDataFormatId() const;
    void SetDataFormatId(PointFormat const& v);

    uint16_t GetDataRecordLength() const;
    
    uint32_t GetPointRecordsCount() const;
    void SetPointRecordsCount(uint32_t const& v);
    
    std::vector<uint32_t> const& GetPointRecordsByReturnCount() const;
    
    double GetScaleX() const;
    double GetScaleY() const;
    double GetScaleZ() const;
    void SetScale(double x, double y, double z);

    double GetOffsetX() const;
    double GetOffsetY() const;
    double GetOffsetZ() const;
    void SetOffset(double x, double y, double z);

    double GetMaxX() const;
    double GetMinX() const;
    double GetMaxY() const;
    double GetMinY() const;
    double GetMaxZ() const;
    double GetMinZ() const;

    void SetMax(double x, double y, double z);
    void SetMin(double x, double y, double z);

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
        eDataSignatureSize = 2,
        eFileSignatureSize = 4,
        ePointsByReturnSize = 5,
        eProjectId4Size = 8,
        ePointDataRecordSize0 = 20,
        ePointDataRecordSize1 = 28,
        eSystemIdSize = 32,
        eSoftwareIdSize = 32,
        eHeaderSize = 227
    };
    
    static char const* const FileSignature; // = "LASF";
    static char const* const SystemIdentifier; // = "libLAS";
    static char const* const SoftwareIdentifier; // = "libLAS 1.0";

    // TODO (low-priority): replace static-size char arrays
    // with std::string and return const-reference to string object.
    
    //
    // Private function members
    //
    void Init();

    //
    // Private data members
    //
    char m_signature[eFileSignatureSize];
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
