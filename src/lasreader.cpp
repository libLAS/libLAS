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
    bool ret = false;
    double time = 0;
    
    if (m_header.GetDataFormatId() == LASHeader::ePointFormat0)
        ret = m_pimpl->ReadNextPoint(m_record);
    else
        ret = m_pimpl->ReadNextPoint(m_record, time);

    MakePoint(time);

    return ret;
}

bool LASReader::ReadPointAt(std::size_t n)
{
    bool ret = false;
    double time = 0;

    if (m_header.GetDataFormatId() == LASHeader::ePointFormat0)
        ret = m_pimpl->ReadPointAt(n, m_record);
    else
        ret = false; // m_pimpl->ReadNextPoint(m_record, time);

    MakePoint(time);

    return ret;
}

void LASReader::Init()
{
    bool ret = m_pimpl->ReadHeader(m_header);

    if (!ret)
        throw std::runtime_error("public header block reading failure");
}

void LASReader::MakePoint(double const& time)
{
    double const x = m_record.x * m_header.GetScaleX() + m_header.GetOffsetX();
    double const y = m_record.y * m_header.GetScaleY() + m_header.GetOffsetY();
    double const z = m_record.z * m_header.GetScaleZ() + m_header.GetOffsetZ();

    m_point.SetCoordinates(x, y, z);
    m_point.SetIntensity(m_record.intensity);
    m_point.SetScanFlags(m_record.flags);
    m_point.SetClassification(m_record.classification);
    m_point.SetTime(time);
}

} // namespace liblas

