#include <liblas/detail/writer10.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/liblas.hpp>
// std
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
    // TODO: Call LASHeader::Write() which delegates to sth like this:
    //       detail::write_n(ofs, header.GetFileSignature().c_str(), 4);
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
    detail::write_n(m_ofs, n1, sizeof(n1));
    
    // 8. Version minor
    n1 = header.GetVersionMinor();
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
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 14. Offset to data

    // 15. Number of variable length records

    // 16. Point Data Format ID

    // 17. Point Data Record Length

    // 18. Number of point records

    // 19. Number of points by return

    // 20-22. Scale factors 

    // 23-25. Offsets

    // 26-27. Max/Min X

    // 28-29. Max/Min Y

    // 30-31. Max/Min Z

    return true;
}

bool WriterImpl::WritePoint(LASPoint const& point)
{
    // TODO: to be implemented
    // TODO: should we return anything or just declare it as void?

    return false;
}

}}} // namespace liblas::detail::v10
