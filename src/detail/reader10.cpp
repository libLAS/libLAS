#include <liblas/detail/reader10.hpp>
#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <fstream>
#include <stdexcept>
#include <cstdlib> // std::size_t
#include <cassert>

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
    header.Read(m_ifs);

    // TODO: which one is better:
    // if (4 != header.GetFileSignature().size())
    if (header.GetFileSignature().empty())
        return false;

    m_offset = header.GetDataOffset();
    m_size = header.GetPointRecordsCount();
    return true;
}

bool ReaderImpl::ReadPoint(LASPointRecord& point)
{
    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(m_offset, std::ios::beg);
    }

    if (m_current < m_size)
    {
        // TODO: Replace with compile-time assert
        assert(20 == sizeof(LASPointRecord));

        detail::read_n(point, m_ifs, sizeof(LASPointRecord));
        ++m_current;

        return true;
    }

    return false;
}

}}} // namespace liblas::detail::v10
