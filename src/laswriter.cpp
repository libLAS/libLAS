// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
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

LASWriter::LASWriter(std::ostream& ofs, LASHeader const& header) :
    m_pimpl(detail::WriterFactory::Create(ofs, header)), m_header(header)
{
    m_pimpl->WriteHeader(m_header);
}

LASWriter::~LASWriter()
{
    assert(0 != m_pimpl.get());

    m_pimpl->UpdateHeader(m_header);
}

std::size_t LASWriter::GetVersion() const
{
    return m_pimpl->GetVersion();
}

LASHeader const& LASWriter::GetHeader() const
{
    return m_header;
}

bool LASWriter::WritePoint(LASPoint const& point)
{
    if (!point.IsValid())
    {
        return false;
    }

    // TODO: Move composition of point record deep into writer implementation
    m_record.x = static_cast<uint32_t>((point.GetX() - m_header.GetOffsetX()) / m_header.GetScaleX());
    m_record.y = static_cast<uint32_t>((point.GetY() - m_header.GetOffsetY()) / m_header.GetScaleY());
    m_record.z = static_cast<uint32_t>((point.GetZ() - m_header.GetOffsetZ()) / m_header.GetScaleZ());
    m_record.intensity = point.GetIntensity();
    m_record.flags = point.GetScanFlags();
    m_record.classification = point.GetClassification();
    m_record.scan_angle_rank = point.GetScanAngleRank();
    m_record.user_data = point.GetUserData();
    m_record.point_source_id = 0; // TODO: How to handle this in portable way, for LAS 1.0 and 1.1


    if (m_header.GetDataFormatId() == LASHeader::ePointFormat0)
        m_pimpl->WritePointRecord(m_record);
    else
        m_pimpl->WritePointRecord(m_record, point.GetTime());

    return true;
}

std::ostream& LASWriter::GetStream() {
    return m_pimpl->GetStream();
}

void LASWriter::WriteHeader(LASHeader const& header)
{
    m_pimpl->WriteHeader(header);
    m_header = header;
}

} // namespace liblas

