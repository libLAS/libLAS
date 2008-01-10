#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/reader.hpp>
// std
#include <fstream>
#include <cassert>
#include <cstdlib> // std::size_t

namespace liblas { namespace detail {

Reader::Reader() : m_offset(0), m_current(0)
{
}

Reader* ReaderFactory::Create(std::ifstream& ifs)
{
    using detail::bytes_of;

    if (!ifs.is_open())
    {
        assert(false);
    }

    // Determine version of given LAS file and
    // instantiate appropriate reader.
    uint8_t verMajor = 0;
    uint8_t verMinor = 0;
    ifs.seekg(24, std::ios::beg);
    ifs.read(bytes_of(verMajor), 1);
    ifs.read(bytes_of(verMinor), 1);

    if (1 == verMajor && 0 == verMinor)
    {
        return new v10::ReaderImpl(ifs);
    }
    else if (1 == verMajor && 1 == verMinor)
    {
        return new v11::ReaderImpl(ifs);
    }
    else if (2 == verMajor && 0 == verMinor)
    {
        // TODO: LAS 2.0 read/write support
        throw std::runtime_error("LAS 2.0 file detected but unsupported");
    }

    throw std::runtime_error("LAS file of unknown version");
}

void ReaderFactory::Destroy(Reader* p)
{
    delete p;
    p = 0;
}

namespace v10 {

ReaderImpl::ReaderImpl(std::ifstream& ifs) : Base(), m_ifs(ifs)
{
}

std::size_t ReaderImpl::GetVersion() const
{
    // TODO: use defined macro or review this calculation
    return (1 * 100000 + 0);
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
        m_ifs.seekg(m_offset, std::ios::beg);

    if (m_current < m_size)
    {
        // TODO: static assert sizeof(LASPoint) == 20
        m_ifs.read(bytes_of(point), sizeof(LASPointRecord));
        ++m_current;

        return true;
    }

    return false;
}

} // namespace v10


namespace v11 {

ReaderImpl::ReaderImpl(std::ifstream& ifs) : Base(), m_ifs(ifs)
{
}

std::size_t ReaderImpl::GetVersion() const
{
    // TODO: use defined macro or review this calculation
    return (1 * 100000 + 1);
}

bool ReaderImpl::ReadHeader(LASHeader& header)
{
    return false;
}

bool ReaderImpl::ReadPoint(LASPointRecord& point)
{
    return false;
}

} // namespace v10

}} // namespace liblas::detail
