#include <liblas/lasheader.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
//std
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring> // std::memset, std::memcpy, std::strncpy
#include <cassert>

namespace liblas
{

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
    std::memcpy(m_projectId4, other.m_projectId4, sizeof(m_projectId4)); 
    std::memcpy(m_systemId, other.m_systemId, sizeof(m_systemId));
    std::memcpy(m_softwareId, other.m_softwareId, sizeof(m_softwareId));

    std::vector<uint32_t>(other.m_pointRecordsByReturn).swap(m_pointRecordsByReturn);
}

LASHeader& LASHeader::operator=(LASHeader const& rhs)
{
    if (&rhs != this)
    {
        void* p = 0;
        p = std::memcpy(m_signature, rhs.m_signature, sizeof(m_signature));
        assert(p == m_signature);
        m_sourceId = rhs.m_sourceId;
        m_reserved = rhs.m_reserved;
        m_projectId1 = rhs.m_projectId1;
        m_projectId2 = rhs.m_projectId2;
        m_projectId3 = rhs.m_projectId3;
        m_versionMajor = rhs.m_versionMajor;
        m_versionMinor = rhs.m_versionMinor;
        p = std::memcpy(m_systemId, rhs.m_systemId, sizeof(m_systemId));
        assert(p == m_systemId);
        p = std::memcpy(m_softwareId, rhs.m_softwareId, sizeof(m_softwareId));
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
    if (0 != v.compare(0, eSignatureSize, "LASF"))
        throw std::invalid_argument("invalid file signature");

    std::strncpy(m_signature, v.c_str(), eSignatureSize);
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

std::string LASHeader::GetSoftwareId() const
{
    return m_softwareId;
}

uint16_t LASHeader::GetCreationDOY() const
{
    return m_createDOY;
}

uint16_t LASHeader::GetCreationYear() const
{
    return m_createYear;
}

uint16_t LASHeader::GetHeaderSize() const
{
    return m_headerSize;
}

uint32_t LASHeader::GetDataOffset() const
{
    return m_dataOffset;
}
uint32_t LASHeader::GetRecordsCount() const
{
    return m_recordsCount;
}

uint8_t LASHeader::GetDataFormatId() const
{
    return m_dataFormatId;
}
uint16_t LASHeader::GetDataRecordLength() const
{
    return m_dataRecordLen;
}
uint32_t LASHeader::GetPointRecordsCount() const
{
    return m_pointRecordsCount;
}

std::vector<uint32_t> const& LASHeader::GetPointRecordsByReturnCount() 
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
    read_n(m_headerSize, ifs, 2);
    read_n(m_dataOffset, ifs, 4);
    read_n(m_recordsCount, ifs, 4);
    read_n(m_dataFormatId, ifs, 1);
    read_n(m_dataRecordLen, ifs, 2);
    read_n(m_pointRecordsCount, ifs, 4);

    std::size_t const nrbyr = 5;
    uint32_t rbyr[nrbyr];
    read_n(rbyr, ifs, 20);
    std::vector<uint32_t>(rbyr, rbyr + nrbyr).swap(m_pointRecordsByReturn);

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
    m_versionMajor = m_versionMinor = m_dataFormatId = uint8_t();
    m_dataRecordLen = m_createDOY = m_createYear = m_headerSize = uint16_t();
    m_sourceId = m_reserved = m_projectId2 = m_projectId3 = uint16_t();
    m_projectId1 = uint32_t();
    m_dataOffset = m_recordsCount = m_pointRecordsCount = uint32_t();

    std::memset(m_signature, 0, sizeof(m_signature));
    std::memset(m_projectId4, 0, sizeof(m_projectId4)); 
    std::memset(m_systemId, 0, sizeof(m_systemId));
    std::memset(m_softwareId, 0, sizeof(m_softwareId));
}

} // namespace liblas
