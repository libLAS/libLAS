#include <liblas/lasreader.hpp>
#include <liblas/detail/reader.hpp>
// std
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>

namespace liblas
{

LASReader::LASReader(std::string const& file) :
    m_ifs(file.c_str(), std::ios::in | std::ios::binary),
        m_pimpl(detail::ReaderFactory::Create(m_ifs))

{
    std::memset(&m_point, 0, sizeof(LASPoint));

    bool ret = m_pimpl->ReadHeader(m_header);
    // TODO: Throw instead
    assert(ret);
}

LASReader::LASReader(std::ifstream& ifs) :
    m_pimpl(detail::ReaderFactory::Create(ifs))
{
    std::memset(&m_point, 0, sizeof(LASPoint));

    bool ret = m_pimpl->ReadHeader(m_header);
    // TODO: Throw instead
    assert(ret);
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
    return m_pimpl->ReadPoint(m_point);
}

} // namespace liblas

