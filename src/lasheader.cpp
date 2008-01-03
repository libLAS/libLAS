#include <liblas/lasheader.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
//std
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring> // std::memset

namespace liblas
{

LASHeader::LASHeader()
{
    Init();
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
    uint8_t const* p = m_projectId4;
    return detail::bytes_of(p);
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


LASHeader::PointScales const& LASHeader::GetScales() const
{
    return m_scales;
}

LASHeader::PointOffsets const& LASHeader::GetOffsets() const
{
    return m_offsets;
}

LASHeader::PointExtents const& LASHeader::GetExtents() const
{
    return m_extents;
}

void LASHeader::Load(std::ifstream& ifs)
{
    using detail::bytes_of;

    if (!ifs.is_open())
        throw std::runtime_error("file stream is closed");

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
