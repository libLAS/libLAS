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

LASReader::LASReader(std::string const& file) :
    m_ifs(file.c_str(), std::ios::in | std::ios::binary),
        m_pimpl(detail::ReaderFactory::Create(m_ifs)),
            m_header(), m_point(m_header)
{
    Init();
}

LASReader::LASReader(std::ifstream& ifs) :
    m_pimpl(detail::ReaderFactory::Create(ifs)),
        m_header(), m_point(m_header)
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

bool LASReader::ReadPoint()
{
    bool ret = m_pimpl->ReadPoint(m_record);

    // TODO: Check if these calls will slow down the reading operation
    //       and invent sometime better but still trying to encapsulate
    //       LAS point data with user-friendly interfaces instead of returning
    //       raw data record.
    m_point.SetCoordinates(m_record.x, m_record.y, m_record.z);
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

