#include <liblas/laswriter.hpp>
#include <liblas/detail/writer.hpp>
// std
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>

namespace liblas
{

LASWriter::LASWriter(std::ofstream& ofs, LASHeader const& header) :
    m_pimpl(detail::WriterFactory::Create(ofs, header)), m_header(header)
{
    m_pimpl->WriteHeader(m_header);
}

LASWriter::~LASWriter()
{
    m_pimpl->UpdateHeader(m_header);
}

std::size_t LASWriter::GetVersion() const
{
    return m_pimpl->GetVersion();
}

bool LASWriter::WritePoint(LASPoint const& point)
{
    // TODO: Move composition of point record deep into writer implementation
    m_record.x = static_cast<uint32_t>((point.GetX() - m_header.GetOffsetX()) / m_header.GetScaleX());
    m_record.y = static_cast<uint32_t>((point.GetY() - m_header.GetOffsetY()) / m_header.GetScaleY());
    m_record.z = static_cast<uint32_t>((point.GetZ() - m_header.GetOffsetZ()) / m_header.GetScaleZ());
    m_record.intensity = point.GetIntensity();
    m_record.flags = point.GetScanFlags();
    m_record.classification = point.GetClassification();

    if (m_header.GetDataFormatId() == LASHeader::ePointFormat0)
        m_pimpl->WritePointRecord(m_record);
    else
        m_pimpl->WritePointRecord(m_record, point.GetTime());

    return true;
}

} // namespace liblas

