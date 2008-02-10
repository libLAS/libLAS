// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/detail/reader11.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <fstream>
#include <cstdlib> // std::size_t
#include <cassert>
#include <stdexcept>

namespace liblas { namespace detail { namespace v11 {

ReaderImpl::ReaderImpl(std::ifstream& ifs) : Base(), m_ifs(ifs)
{
}

std::size_t ReaderImpl::GetVersion() const
{
    return eLASVersion11;
}

bool ReaderImpl::ReadHeader(LASHeader& header)
{
    using detail::read_n;

    // Helper variables
    uint8_t n1 = 0;
    uint16_t n2 = 0;
    uint32_t n4 = 0;
    double x1 = 0;
    double y1 = 0;
    double z1 = 0;
    double x2 = 0;
    double y2 = 0;
    double z2 = 0;
    char buf[32] = { 0 };
    char fsig[4] = { 0 };

    m_ifs.seekg(0);

    // 1. File Signature
    read_n(fsig, m_ifs, 4);
    header.SetFileSignature(fsig);

    // 2. File Source ID
    read_n(n2, m_ifs, sizeof(n2));
    header.SetFileSourceId(n2);

    // 3. Reserved
    // This data must always contain Zeros.
    read_n(n2, m_ifs, sizeof(n2));

    // 4-7. Project ID
    uint32_t d1 = 0;
    uint16_t d2 = 0;
    uint16_t d3 = 0;
    uint8_t d4[8] = { 0 };
    read_n(d1, m_ifs, sizeof(d1));
    read_n(d2, m_ifs, sizeof(d2));
    read_n(d3, m_ifs, sizeof(d3));
    read_n(d4, m_ifs, sizeof(d4));
    liblas::guid g(d1, d2, d3, d4);
    header.SetProjectId(g);

    // 8. Version major
    read_n(n1, m_ifs, sizeof(n1));
    header.SetVersionMajor(n1);

    // 9. Version minor
    read_n(n1, m_ifs, sizeof(n1));
    header.SetVersionMinor(n1);

    // 10. System ID
    read_n(buf, m_ifs, sizeof(buf));
    header.SetSystemId(buf);

    // 11. Generating Software ID
    read_n(buf, m_ifs, sizeof(buf));
    header.SetSoftwareId(buf);

    // 12. File Creation Day of Year
    read_n(n2, m_ifs, sizeof(n2));
    header.SetCreationDOY(n2);

    // 13. File Creation Year
    read_n(n2, m_ifs, sizeof(n2));
    header.SetCreationYear(n2);

    // 14. Header Size
    // NOTE: Size of the stanard header block must always be 227 bytes
    read_n(n2, m_ifs, sizeof(n2));

    // 15. Offset to data
    read_n(n4, m_ifs, sizeof(n4));
    if (n4 < header.GetHeaderSize())
    {
        // TODO: Move this test to LASHeader::Validate()
        throw std::domain_error("offset to point data smaller than header size");
    }
    header.SetDataOffset(n4);

    // 16. Number of variable length records
    read_n(n4, m_ifs, sizeof(n4));
    header.SetRecordsCount(n4);

    // 17. Point Data Format ID
    read_n(n1, m_ifs, sizeof(n1));
    if (n1 == LASHeader::ePointFormat0)
        header.SetDataFormatId(LASHeader::ePointFormat0);
    else if (n1 == LASHeader::ePointFormat1)
        header.SetDataFormatId(LASHeader::ePointFormat1);
    else
        throw std::domain_error("invalid point data format");

    // 18. Point Data Record Length
    // NOTE: No need to set record length because it's
    // determined on basis of point data format.
    read_n(n2, m_ifs, sizeof(n2));

    // 19. Number of point records
    read_n(n4, m_ifs, sizeof(n4));
    header.SetPointRecordsCount(n4);

    // 20. Number of points by return
    std::vector<uint32_t>::size_type const srbyr = 5;
    uint32_t rbyr[srbyr] = { 0 };
    read_n(rbyr, m_ifs, sizeof(rbyr));
    for (std::size_t i = 0; i < srbyr; ++i)
    {
        header.SetPointRecordsByReturnCount(i, rbyr[i]);
    }

    // 21-23. Scale factors
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    header.SetScale(x1, y1, z1);

    // 24-26. Offsets
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    header.SetOffset(x1, y1, z1);

    // 27-28. Max/Min X
    read_n(x1, m_ifs, sizeof(x1));
    read_n(x2, m_ifs, sizeof(x2));

    // 29-30. Max/Min Y
    read_n(y1, m_ifs, sizeof(y1));
    read_n(y2, m_ifs, sizeof(y2));

    // 31-32. Max/Min Z
    read_n(z1, m_ifs, sizeof(z1));
    read_n(z2, m_ifs, sizeof(z2));

    header.SetMax(x1, y1, z1);
    header.SetMin(x2, y2, z2);

    m_offset = header.GetDataOffset();
    m_size = header.GetPointRecordsCount();

    return true;
}

bool ReaderImpl::ReadNextPoint(detail::PointRecord& record)
{
    // Read point data record format 0

    // TODO: Replace with compile-time assert
    assert(LASHeader::ePointSize0 == sizeof(record));

    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(m_offset, std::ios::beg);
    }

    if (m_current < m_size)
    {
        detail::read_n(record, m_ifs, sizeof(PointRecord));
        ++m_current;

        return true;
    }

    return false;
}

bool ReaderImpl::ReadNextPoint(detail::PointRecord& record, double& time)
{
    // Read point data record format 1

    // TODO: Replace with compile-time assert
    assert(28 == sizeof(record) + sizeof(time));

    bool hasData = ReadNextPoint(record);
    if (hasData)
    {
        detail::read_n(time, m_ifs, sizeof(double));
    }

    return hasData;
}

bool ReaderImpl::ReadPointAt(std::size_t n, PointRecord& record)
{
    // Read point data record format 0

    // TODO: Replace with compile-time assert
    assert(LASHeader::ePointSize0 == sizeof(record));

    if (m_size <= n)
        return false;

    std::streamsize pos = (static_cast<std::streamsize>(n) * LASHeader::ePointSize0) + m_offset;

    m_ifs.clear();
    m_ifs.seekg(pos, std::ios::beg);
    detail::read_n(record, m_ifs, sizeof(record));

    return true;
}

bool ReaderImpl::ReadPointAt(std::size_t n, PointRecord& record, double& time)
{
    // Read point data record format 1

    // TODO: Replace with compile-time assert
    assert(LASHeader::ePointSize1 == sizeof(record) + sizeof(time));

    bool hasData = ReadPointAt(n, record);
    if (hasData)
    {
        detail::read_n(time, m_ifs, sizeof(double));
    }

    return hasData;
}

}}} // namespace liblas::detail::v11
