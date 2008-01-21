#include <liblas/lasheader.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
//std
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring> // std::memset, std::memcpy, std::strncpy
#include <cassert>

namespace liblas
{

char const* const LASHeader::FileSignature = "LASF";
char const* const LASHeader::SystemIdentifier = "libLAS";
char const* const LASHeader::SoftwareIdentifier = "libLAS 1.0";

LASHeader::LASHeader()
{
    Init();
}

LASHeader::LASHeader(LASHeader const& other) :
    m_sourceId(other.m_sourceId),
    m_reserved(other.m_reserved),
    m_projectId1(other.m_projectId1),
    m_projectId2(other.m_projectId2),
    m_projectId3(other.m_projectId3),
    m_versionMajor(other.m_versionMajor),
    m_versionMinor(other.m_versionMinor),
    m_createDOY(other.m_createDOY),
    m_createYear(other.m_createYear),
    m_headerSize(other.m_headerSize),
    m_dataOffset(other.m_dataOffset),
    m_recordsCount(other.m_recordsCount),
    m_dataFormatId(other.m_dataFormatId),
    m_dataRecordLen(other.m_dataRecordLen),
    m_pointRecordsCount(other.m_pointRecordsCount),
    m_scales(other.m_scales),
    m_offsets(other.m_offsets),
    m_extents(other.m_extents)
{
    void* p = 0;
    p = std::memcpy(m_signature, other.m_signature, eFileSignatureSize);
    assert(p == m_signature);
    p = std::memcpy(m_projectId4, other.m_projectId4, eProjectId4Size); 
    assert(p == m_projectId4);
    p = std::memcpy(m_systemId, other.m_systemId, eSystemIdSize);
    assert(p == m_systemId);
    p = std::memcpy(m_softwareId, other.m_softwareId, eSoftwareIdSize);
    assert(p == m_softwareId);

    std::vector<uint32_t>(other.m_pointRecordsByReturn).swap(m_pointRecordsByReturn);
    assert(ePointsByReturnSize >= m_pointRecordsByReturn.size());
}

LASHeader& LASHeader::operator=(LASHeader const& rhs)
{
    if (&rhs != this)
    {
        void* p = 0;
        p = std::memcpy(m_signature, rhs.m_signature, eFileSignatureSize);
        assert(p == m_signature);
        m_sourceId = rhs.m_sourceId;
        m_reserved = rhs.m_reserved;
        m_projectId1 = rhs.m_projectId1;
        m_projectId2 = rhs.m_projectId2;
        m_projectId3 = rhs.m_projectId3;
        p = std::memcpy(m_projectId4, rhs.m_projectId4, eProjectId4Size); 
        assert(p == m_projectId4);
        m_versionMajor = rhs.m_versionMajor;
        m_versionMinor = rhs.m_versionMinor;
        p = std::memcpy(m_systemId, rhs.m_systemId, eSystemIdSize);
        assert(p == m_systemId);
        p = std::memcpy(m_softwareId, rhs.m_softwareId, eSoftwareIdSize);
        assert(p == m_softwareId);
        m_createDOY = rhs.m_createDOY;
        m_createYear = rhs.m_createYear;
        m_headerSize = rhs.m_headerSize;
        m_dataOffset = rhs.m_dataOffset;
        m_recordsCount = rhs.m_recordsCount;
        m_dataFormatId = rhs.m_dataFormatId;
        m_dataRecordLen = rhs.m_dataRecordLen;
        m_pointRecordsCount = rhs.m_pointRecordsCount;
        
        std::vector<uint32_t>(rhs.m_pointRecordsByReturn).swap(m_pointRecordsByReturn);
        assert(ePointsByReturnSize >= m_pointRecordsByReturn.size());

        m_scales = rhs.m_scales;
        m_offsets = rhs.m_offsets;
        m_extents = rhs.m_extents;
    }
    return *this;
}

std::string LASHeader::GetFileSignature() const
{
    return m_signature;
}

void LASHeader::SetFileSignature(std::string const& v)
{
    if (0 != v.compare(0, eFileSignatureSize, FileSignature))
        throw std::invalid_argument("invalid file signature");

    std::strncpy(m_signature, v.c_str(), eFileSignatureSize);
}

uint16_t LASHeader::GetFileSourceId() const
{
    return m_sourceId;
}

void LASHeader::SetFileSourceId(uint16_t const& v)
{
    if (0 > v || v > 35535)
        throw std::out_of_range("file source id out of range");

    m_sourceId = v;
}

uint16_t LASHeader::GetReserved() const
{
    return m_reserved;
}

uint32_t LASHeader::GetProjectId1() const
{
    return m_projectId1;
}

uint16_t LASHeader::GetProjectId2() const
{
    return m_projectId2;
}

uint16_t LASHeader::GetProjectId3() const
{
    return m_projectId3;
}

std::string LASHeader::GetProjectId4() const
{
    char const* p = reinterpret_cast<char const*>(m_projectId4);
    // TODO: should we force size even if c points to empty array?
    // std::string tmp(c, 8);
    return p;
}

uint8_t LASHeader::GetVersionMajor() const
{
    return m_versionMajor;
}

void LASHeader::SetVersionMajor(uint8_t const& v)
{
    if (eVersionMajorMin > v || v > eVersionMajorMax)
        throw std::out_of_range("version major out of range");

    m_versionMajor = v;
}

uint8_t LASHeader::GetVersionMinor() const
{
    return m_versionMinor;
}

void LASHeader::SetVersionMinor(uint8_t const& v)
{
    if (eVersionMinorMin > v || v > eVersionMinorMax)
        throw std::out_of_range("version minor out of range");

    m_versionMinor = v;
}

std::string LASHeader::GetSystemId() const
{
    return m_systemId;
}

void LASHeader::SetSystemId(std::string const& v)
{
    if (v.size() > eSystemIdSize)
        throw std::invalid_argument("system id too long");

    std::fill(m_systemId, m_systemId + eSystemIdSize, 0);
    std::strncpy(m_systemId, v.c_str(), eSystemIdSize);
}

std::string LASHeader::GetSoftwareId() const
{
    return m_softwareId;
}

void LASHeader::SetSoftwareId(std::string const& v)
{
    if (v.size() > eSoftwareIdSize)
        throw std::invalid_argument("generating software id too long");

    std::fill(m_softwareId, m_softwareId + eSoftwareIdSize, 0);
    std::strncpy(m_softwareId, v.c_str(), eSoftwareIdSize);
}

uint16_t LASHeader::GetCreationDOY() const
{
    return m_createDOY;
}

void LASHeader::SetCreationDOY(uint16_t const& v)
{
    if (1 > v || v > 366)
        throw std::out_of_range("day of year out of range");

    m_createDOY = v;
}

uint16_t LASHeader::GetCreationYear() const
{
    return m_createYear;
}

void LASHeader::SetCreationYear(uint16_t const& v)
{
    // mloskot: I've taken these values arbitrarily
    if (1960 > v || v > 9999)
        throw std::out_of_range("year out of range");

    m_createYear = v;
}

uint16_t LASHeader::GetHeaderSize() const
{
    return 227; // m_headerSize;
}

uint32_t LASHeader::GetDataOffset() const
{
    return m_dataOffset;
}
uint32_t LASHeader::GetRecordsCount() const
{
    return m_recordsCount;
}

LASHeader::PointFormat LASHeader::GetDataFormatId() const
{
    if (ePointFormat0 == m_dataFormatId)
        return ePointFormat0;
    else
        return ePointFormat1;
}

void LASHeader::SetDataFormatId(LASHeader::PointFormat const& v)
{
    m_dataFormatId = v;
}

uint16_t LASHeader::GetDataRecordLength() const
{
    // NOTE: assertions below are used to check if our assumption is correct,
    // for debugging purpose only.

    if (ePointFormat0 == m_dataFormatId)
    {
        assert(ePointDataRecordSize0 == m_dataRecordLen);
        return ePointDataRecordSize0;
    }
    else
    {
        assert(ePointDataRecordSize1 == m_dataRecordLen);
        return ePointDataRecordSize1;
    }
}

uint32_t LASHeader::GetPointRecordsCount() const
{
    return m_pointRecordsCount;
}

void LASHeader::SetPointRecordsCount(uint32_t const& v)
{
    m_pointRecordsCount = v;
}

std::vector<uint32_t> const& LASHeader::GetPointRecordsByReturnCount() const
{
    return m_pointRecordsByReturn;
}

double LASHeader::GetScaleX() const
{
    return m_scales.x;
}

double LASHeader::GetScaleY() const
{
    return m_scales.y;
}

double LASHeader::GetScaleZ() const
{
    return m_scales.z;
}

void LASHeader::SetScale(double x, double y, double z)
{
    double const minscale = 0.01;
    m_scales.x = (0 == x) ? minscale : x;
    m_scales.y = (0 == y) ? minscale : y;
    m_scales.z = (0 == z) ? minscale : z;
}

double LASHeader::GetOffsetX() const
{
    return m_offsets.x;
}

double LASHeader::GetOffsetY() const
{
    return m_offsets.y;
}

double LASHeader::GetOffsetZ() const
{
    return m_offsets.z;
}

void LASHeader::SetOffset(double x, double y, double z)
{
    m_offsets = PointOffsets(x, y, z);
}

double LASHeader::GetMaxX() const
{
    return m_extents.max.x;
}

double LASHeader::GetMinX() const
{
    return m_extents.min.x;
}

double LASHeader::GetMaxY() const
{
    return m_extents.max.y;
}

double LASHeader::GetMinY() const
{
    return m_extents.min.y;
}

double LASHeader::GetMaxZ() const
{
    return m_extents.max.z;
}

double LASHeader::GetMinZ() const
{
    return m_extents.min.z;
}

void LASHeader::SetMax(double x, double y, double z)
{
    m_extents.max = detail::Point<double>(x, y, z);
}

void LASHeader::SetMin(double x, double y, double z)
{
    m_extents.min = detail::Point<double>(x, y, z);
}

void LASHeader::Read(std::ifstream& ifs)
{
    using detail::read_n;

    if (!ifs)
        throw std::runtime_error("input stream state is invalid");  

    ifs.seekg(0);
    read_n(m_signature, ifs, sizeof(m_signature));
    read_n(m_sourceId, ifs, 2);
    read_n(m_reserved, ifs, 2);
    read_n(m_projectId1, ifs, 4);
    read_n(m_projectId2, ifs, 2);
    read_n(m_projectId3, ifs, 2);
    read_n(m_projectId4, ifs, 8);
    read_n(m_versionMajor, ifs, 1);
    read_n(m_versionMinor, ifs, 1);
    read_n(m_systemId, ifs, 32);
    read_n(m_softwareId, ifs, 32);
    read_n(m_createDOY, ifs, 2);
    read_n(m_createYear, ifs, 2);
    read_n(m_headerSize, ifs, 2); // TODO: should we validate == 227?
    read_n(m_dataOffset, ifs, 4);
    // TODO: Move this test to LASHeader::Validate()
    if (m_dataOffset < m_headerSize)
    {
        throw std::domain_error("offset to point data smaller than header size");
    }
    read_n(m_recordsCount, ifs, 4);
    read_n(m_dataFormatId, ifs, 1);
    read_n(m_dataRecordLen, ifs, 2);
    read_n(m_pointRecordsCount, ifs, 4);

    std::vector<uint32_t>::size_type const srbyr = 5;
    uint32_t rbyr[srbyr] = { 0 };
    read_n(rbyr, ifs, sizeof(rbyr));
    std::vector<uint32_t>(rbyr, rbyr + srbyr).swap(m_pointRecordsByReturn);

    read_n(m_scales.x, ifs, 8);
    read_n(m_scales.y, ifs, 8);
    read_n(m_scales.z, ifs, 8);
    read_n(m_offsets.x, ifs, 8);
    read_n(m_offsets.y, ifs, 8);
    read_n(m_offsets.z, ifs, 8);
    read_n(m_extents.max.x, ifs, 8);
    read_n(m_extents.min.x, ifs, 8);
    read_n(m_extents.max.y, ifs, 8);
    read_n(m_extents.min.y, ifs, 8);
    read_n(m_extents.max.z, ifs, 8);
    read_n(m_extents.min.z, ifs, 8);
}

void LASHeader::Init()
{
    // Initialize public header block with default values

    m_versionMajor = 1;
    m_versionMinor = 0;
    m_dataFormatId = ePointFormat0;
    m_dataRecordLen = ePointDataRecordSize0;

    // TODO: Use current date
    m_createDOY = 0;
    m_createYear = 0;
    
    m_headerSize = eHeaderSize;

    // TODO: Replace with liblas::guid, unique or null GUID.
    m_sourceId = m_reserved = m_projectId2 = m_projectId3 = uint16_t();
    m_projectId1 = uint32_t();
    std::memset(m_projectId4, 0, sizeof(m_projectId4)); 

    m_dataOffset = eHeaderSize + eDataSignatureSize;
    m_recordsCount = 0;
    m_pointRecordsCount = 0;

    std::memset(m_signature, 0, eFileSignatureSize);
    std::strncpy(m_signature, FileSignature, eFileSignatureSize);

    std::memset(m_systemId, 0, eSystemIdSize);
    std::strncpy(m_systemId, SystemIdentifier, eSystemIdSize);

    std::memset(m_softwareId, 0, eSoftwareIdSize);
    std::strncpy(m_softwareId, SoftwareIdentifier, eSoftwareIdSize);

    // Zero scale value is useless, so we need to use a small value.
    SetScale(0.01, 0.01, 0.01);
}

} // namespace liblas
