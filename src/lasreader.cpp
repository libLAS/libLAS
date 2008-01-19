#include <liblas/lasreader.hpp>
#include <liblas/detail/reader.hpp>
// std
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>

namespace liblas
{

LASReader::LASReader(std::ifstream& ifs) :
    m_pimpl(detail::ReaderFactory::Create(ifs))
{
    Init();
}

LASReader::~LASReader()
{
    // empty, but required so we can implement PIMPL using
    // std::auto_ptr with incomplete type (Reader).
}

std::size_t LASReader::GetVersion() const
{
    return m_pimpl->GetVersion();
}

LASHeader const& LASReader::GetHeader() const
{
    return m_header;
}

LASPoint const& LASReader::GetPoint() const
{
    return m_point;
}

bool LASReader::ReadNextPoint()
{
    bool ret = m_pimpl->ReadNextPoint(m_record);

    double const x = m_record.x * m_header.GetScaleX() + m_header.GetOffsetX();
    double const y = m_record.y * m_header.GetScaleY() + m_header.GetOffsetY();
    double const z = m_record.z * m_header.GetScaleZ() + m_header.GetOffsetZ();

    m_point.SetCoordinates(x, y, z);
    m_point.SetIntensity(m_record.intensity);
    m_point.SetScanFlags(m_record.flags);
    m_point.SetClassification(m_record.classification);

    return ret;
}

void LASReader::Init()
{
    bool ret = m_pimpl->ReadHeader(m_header);

    if (!ret)
        throw std::runtime_error("public header block reading failure");
}

} // namespace liblas

