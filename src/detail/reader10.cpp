#include <liblas/detail/reader10.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <fstream>
#include <stdexcept>
#include <cstdlib> // std::size_t
#include <cassert>


// XXX: to be removed
#include <iostream>

namespace liblas { namespace detail { namespace v10 {

ReaderImpl::ReaderImpl(std::ifstream& ifs) : Base(), m_ifs(ifs)
{
}

std::size_t ReaderImpl::GetVersion() const
{
    return eLASVersion10;
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

    // 2. Reserved
    // This data must always contain Zeros.
    read_n(n4, m_ifs, sizeof(n4));

    // 3-6. GUID data
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

    // 7. Version major
    read_n(n1, m_ifs, sizeof(n1));
    header.SetVersionMajor(n1);

    // 8. Version minor
    read_n(n1, m_ifs, sizeof(n1));
    header.SetVersionMinor(n1);

    // 9. System ID
    read_n(buf, m_ifs, sizeof(buf));
    header.SetSystemId(buf);

    // 10. Generating Software ID
    read_n(buf, m_ifs, sizeof(buf));
    header.SetSoftwareId(buf);

    // 11. Flight Date Julian
    read_n(n2, m_ifs, sizeof(n2));
    header.SetCreationDOY(n2);

    // 12. Year
    read_n(n2, m_ifs, sizeof(n2));
    header.SetCreationYear(n2);

    // 13. Header Size
    // NOTE: Size of the stanard header block must always be 227 bytes
    read_n(n2, m_ifs, sizeof(n2));

    // 14. Offset to data
    read_n(n4, m_ifs, sizeof(n4));
    if (n4 < header.GetHeaderSize())
    {
        // TODO: Move this test to LASHeader::Validate()
        throw std::domain_error("offset to point data smaller than header size");
    }
    header.SetDataOffset(n4);

    // 15. Number of variable length records
    read_n(n4, m_ifs, sizeof(n4));
    header.SetRecordsCount(n4);

    // 16. Point Data Format ID
    read_n(n1, m_ifs, sizeof(n1));
    if (n1 == LASHeader::ePointFormat0)
        header.SetDataFormatId(LASHeader::ePointFormat0);
    else if (n1 == LASHeader::ePointFormat1)
        header.SetDataFormatId(LASHeader::ePointFormat1);
    else
        throw std::domain_error("invalid point data format");

    // 17. Point Data Record Length
    // NOTE: No need to set record length because it's
    // determined on basis of point data format.
    read_n(n2, m_ifs, sizeof(n2));

    // 18. Number of point records
    read_n(n4, m_ifs, sizeof(n4));
    header.SetPointRecordsCount(n4);

    // 19. Number of points by return
    std::vector<uint32_t>::size_type const srbyr = 5;
    uint32_t rbyr[srbyr] = { 0 };
    read_n(rbyr, m_ifs, sizeof(rbyr));
    for (std::size_t i = 0; i < srbyr; ++i)
    {
        header.SetPointRecordsByReturnCount(i, rbyr[i]);
    }

    // 20-22. Scale factors
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    header.SetScale(x1, y1, z1);

    // 23-25. Offsets
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    header.SetOffset(x1, y1, z1);

    // 26-27. Max/Min X
    read_n(x1, m_ifs, sizeof(x1));
    read_n(x2, m_ifs, sizeof(x2));

    // 28-29. Max/Min Y
    read_n(y1, m_ifs, sizeof(y1));
    read_n(y2, m_ifs, sizeof(y2));

    // 30-31. Max/Min Z
    read_n(z1, m_ifs, sizeof(z1));
    read_n(z2, m_ifs, sizeof(z2));

    header.SetMax(x1, y1, z1);
    header.SetMin(x2, y2, z2);

    m_offset = header.GetDataOffset();
    m_size = header.GetPointRecordsCount();


    // TODO: Under construction
    //       Testing reading of VLRecords with GeoKeys
    //ReadGeoreference(header);

    return true;
}

bool ReaderImpl::ReadGeoreference(LASHeader const& header)
{
    // TODO: Under construction

    VLRHeader vlrh = { 0 };
    std::string const uid("LASF_Projection");

    m_ifs.seekg(header.GetHeaderSize(), std::ios::beg);

    for (uint32_t i = 0; i < header.GetRecordsCount(); ++i)
    {
        read_n(vlrh, m_ifs, sizeof(VLRHeader));

        if (uid == vlrh.userId && 34735 == vlrh.recordId)
        {
            std::cout << "GeoKeyDirectoryTag: " << vlrh.recordId << std::endl;

            GeoKeysHeader gkh = { 0 };
            read_n(gkh, m_ifs, sizeof(GeoKeysHeader));

            std::cout << "--Header: " << gkh.keyDirectoryVersion << " ; " << gkh.keyRevision << " ; " << gkh.minorRevision << " ; " << gkh.numberOfKeys << std::endl;

            for (uint16_t j = 0; j < gkh.numberOfKeys; ++j)
            {
                GeoKeyEntry gke = { 0 };
                read_n(gke, m_ifs, sizeof(GeoKeyEntry));

                std::cout << "---KeyEntry: " << gke.keyId << " ; " << gke.tiffTagLocation<< " ; " << gke.count << " ; " <<  gke.valueOffset << std::endl;
            }
        }
        else if (uid == vlrh.userId && 34736 == vlrh.recordId)
        {
            std::cout << "GeoDoubleParamsTag: " << vlrh.recordId << std::endl;

            std::ifstream::pos_type const pos = m_ifs.tellg();
            m_ifs.seekg(pos + std::ifstream::pos_type(vlrh.recordLengthAfterHeader));
        }
        else if (uid == vlrh.userId && 34737 == vlrh.recordId)
        {
            std::cout << "GeoAsciiParamsTag: " << vlrh.recordId << std::endl;

            std::ifstream::pos_type const pos = m_ifs.tellg();
            m_ifs.seekg(pos + std::ifstream::pos_type(vlrh.recordLengthAfterHeader));
        }
        else
        {
            std::ifstream::pos_type const pos = m_ifs.tellg();
            m_ifs.seekg(pos + std::ifstream::pos_type(vlrh.recordLengthAfterHeader));
        }
    }

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
    assert(LASHeader::ePointSize1 == sizeof(record) + sizeof(time));

    bool eof = ReadNextPoint(record);
    if (eof)
    {
        detail::read_n(time, m_ifs, sizeof(double));
    }

    return eof;
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

    bool eof = ReadPointAt(n, record);
    if (eof)
    {
        detail::read_n(time, m_ifs, sizeof(double));
    }

    return eof;
}

}}} // namespace liblas::detail::v10
