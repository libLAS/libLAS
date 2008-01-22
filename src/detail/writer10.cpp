#include <liblas/detail/writer10.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/liblas.hpp>
// std
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstdlib> // std::size_t
#include <cassert>

namespace liblas { namespace detail { namespace v10 {

WriterImpl::WriterImpl(std::ofstream& ofs) : Base(), m_ofs(ofs)
{
}

std::size_t WriterImpl::GetVersion() const
{
    return eLASVersion10;
}

bool WriterImpl::WriteHeader(LASHeader const& header)
{
    // TODO: should we return anything or just declare it as void?

    uint8_t n1 = 0;
    uint16_t n2 = 0;
    uint32_t n4 = 0;

    // 1. File Signature
    detail::write_n(m_ofs, header.GetFileSignature().c_str(), 4);
    
    // 2. Reserved
    n4 = header.GetReserved();
    detail::write_n(m_ofs, n4, sizeof(n4));

    // 3. GUID data 1
    n4 = header.GetProjectId1();
    detail::write_n(m_ofs, n4, sizeof(n4));
    
    // 4. GUID data 2
    n2 = header.GetProjectId2();
    detail::write_n(m_ofs, n2, sizeof(n2));
    
    // 5. GUID data 3
    n2 = header.GetProjectId3();
    detail::write_n(m_ofs, n2, sizeof(n2));
    
    // 6. GUID data 4
    detail::write_n(m_ofs, header.GetProjectId4().c_str(), 8);
    
    // 7. Version major
    n1 = header.GetVersionMajor();
    assert(1 == n1);
    detail::write_n(m_ofs, n1, sizeof(n1));
    
    // 8. Version minor
    n1 = header.GetVersionMinor();
    assert(0 == n1);
    detail::write_n(m_ofs, n1, sizeof(n1));

    // 9. System ID
    detail::write_n(m_ofs, header.GetSystemId().c_str(), 32);
    
    // 10. Generating Software ID
    detail::write_n(m_ofs, header.GetSoftwareId().c_str(), 32);

    // 11. Flight Date Julian
    n2 = header.GetCreationDOY();
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 12. Year
    n2 = header.GetCreationYear();
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 13. Header Size
    n2 = header.GetHeaderSize();
    assert(227 <= n2);
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 14. Offset to data
    // At this point, no variable length records are written, so 
    // data offset is equal to (header size + data start signature size):
    // 227 + 2 = 229
    // TODO: This value must be updated after new variable length record is added.
    uint32_t const dataSignatureSize = 2;
    n4 = header.GetHeaderSize() + dataSignatureSize;
    assert(229 <= n4);
    detail::write_n(m_ofs, n4, sizeof(n4));

    // 15. Number of variable length records
    // TODO: This value must be updated after new variable length record is added.
    n4 = header.GetRecordsCount();
    detail::write_n(m_ofs, n4, sizeof(n4));

    // 16. Point Data Format ID
    n1 = header.GetDataFormatId();
    detail::write_n(m_ofs, n1, sizeof(n1));

    // 17. Point Data Record Length
    n2 = header.GetDataRecordLength();
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 18. Number of point records
    // TODO: This value must be updated after all point data records are added.
    n4 = header.GetPointRecordsCount();
    detail::write_n(m_ofs, n4, sizeof(n4));

    // 19. Number of points by return
    std::vector<uint32_t>::size_type const srbyr = 5;
    std::vector<uint32_t> const& vpbr = header.GetPointRecordsByReturnCount();
    assert(vpbr.size() <= srbyr);
    uint32_t pbr[srbyr] = { 0 };
    std::copy(vpbr.begin(), vpbr.end(), pbr);
    detail::write_n(m_ofs, pbr, sizeof(pbr));

    // 20-22. Scale factors
    detail::write_n(m_ofs, header.GetScaleX(), sizeof(double));
    detail::write_n(m_ofs, header.GetScaleY(), sizeof(double));
    detail::write_n(m_ofs, header.GetScaleZ(), sizeof(double));

    // 23-25. Offsets
    detail::write_n(m_ofs, header.GetOffsetX(), sizeof(double));
    detail::write_n(m_ofs, header.GetOffsetY(), sizeof(double));
    detail::write_n(m_ofs, header.GetOffsetZ(), sizeof(double));

    // 26-27. Max/Min X
    detail::write_n(m_ofs, header.GetMaxX(), sizeof(double));
    detail::write_n(m_ofs, header.GetMinX(), sizeof(double));

    // 28-29. Max/Min Y
    detail::write_n(m_ofs, header.GetMaxY(), sizeof(double));
    detail::write_n(m_ofs, header.GetMinY(), sizeof(double));

    // 30-31. Max/Min Z
    detail::write_n(m_ofs, header.GetMaxZ(), sizeof(double));
    detail::write_n(m_ofs, header.GetMinZ(), sizeof(double));

    return true;
}

bool WriterImpl::WritePoint(LASPoint const& point)
{
    // TODO: to be implemented
    // TODO: should we return anything or just declare it as void?

    return false;
}

}}} // namespace liblas::detail::v10
