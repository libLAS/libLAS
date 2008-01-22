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

bool ReaderImpl::ReadNextPoint(detail::PointRecord& record)
{
    // Read point data record format 0

    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(m_offset, std::ios::beg);
    }

    if (m_current < m_size)
    {
        // TODO: Replace with compile-time assert
        assert(20 == sizeof(record));

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

    bool eof = ReadNextPoint(record);
    if (eof)
    {
        detail::read_n(time, m_ifs, sizeof(double));
    }

    return eof;
}

}}} // namespace liblas::detail::v10
