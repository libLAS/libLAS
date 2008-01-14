#include <liblas/lasheader.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
//std
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring> // std::memset, std::memcpy
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
    std::memcpy(m_pointRecordsByReturn, other.m_pointRecordsByReturn,
                sizeof(m_pointRecordsByReturn));
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
        p = std::memcpy(m_pointRecordsByReturn, rhs.m_pointRecordsByReturn,
                        sizeof(m_pointRecordsByReturn));
        assert(p == m_pointRecordsByReturn);
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

uint16_t LASHeader::GetFileSourceId() const
{
    return m_sourceId;
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

uint8_t LASHeader::GetVersionMinor() const
{
    return m_versionMinor;
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

// TODO: design it in user-friendly way
//std::vector<uint32_t> const& GetPointRecordsByReturnCount() const


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
    using detail::bytes_of;

    if (!ifs.is_open())
        throw std::runtime_error("input stream not open");

    ifs.seekg(0);
    ifs.read(bytes_of(m_signature), sizeof(m_signature));
    ifs.read(bytes_of(m_sourceId), 2);
    ifs.read(bytes_of(m_reserved), 2);
    ifs.read(bytes_of(m_projectId1), 4);
    ifs.read(bytes_of(m_projectId2), 2);
    ifs.read(bytes_of(m_projectId3), 2);
    ifs.read(bytes_of(m_projectId4), 8);
    ifs.read(bytes_of(m_versionMajor), 1);
    ifs.read(bytes_of(m_versionMinor), 1);
    ifs.read(bytes_of(m_systemId), 32);
    ifs.read(bytes_of(m_softwareId), 32);
    ifs.read(bytes_of(m_createDOY), 2);
    ifs.read(bytes_of(m_createYear), 2);
    ifs.read(bytes_of(m_headerSize), 2);
    ifs.read(bytes_of(m_dataOffset), 4);
    ifs.read(bytes_of(m_recordsCount), 4);
    ifs.read(bytes_of(m_dataFormatId), 1);
    ifs.read(bytes_of(m_dataRecordLen), 2);
    ifs.read(bytes_of(m_pointRecordsCount), 4);
    ifs.read(bytes_of(m_pointRecordsByReturn), 20);
    ifs.read(bytes_of(m_scales.x), 8);
    ifs.read(bytes_of(m_scales.y), 8);
    ifs.read(bytes_of(m_scales.z), 8);
    ifs.read(bytes_of(m_offsets.x), 8);
    ifs.read(bytes_of(m_offsets.y), 8);
    ifs.read(bytes_of(m_offsets.z), 8);
    ifs.read(bytes_of(m_extents.max.x), 8);
    ifs.read(bytes_of(m_extents.min.x), 8);
    ifs.read(bytes_of(m_extents.max.y), 8);
    ifs.read(bytes_of(m_extents.min.y), 8);
    ifs.read(bytes_of(m_extents.max.z), 8);
    ifs.read(bytes_of(m_extents.min.z), 8);
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
