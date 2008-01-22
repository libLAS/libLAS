#include <liblas/detail/reader11.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <fstream>
#include <cassert>
#include <cstdlib> // std::size_t
#include <stdexcept>

namespace liblas { namespace detail { namespace v11 {

ReaderImpl::ReaderImpl(std::ifstream& ifs) : Base(), m_ifs(ifs)
{
    throw std::runtime_error("TODO: reading LAS 1.1 not finished");
}

std::size_t ReaderImpl::GetVersion() const
{
    return eLASVersion11;
}

bool ReaderImpl::ReadHeader(LASHeader& header)
{
    return false;
}

bool ReaderImpl::ReadNextPoint(detail::PointRecord& point)
{
    // Read point data record format 0

    return false;
}

bool ReaderImpl::ReadNextPoint(detail::PointRecord& record, double& time)
{
    // Read point data record format 1

    // TODO: Replace with compile-time assert
    assert(28 == sizeof(record) + sizeof(time));

    bool eof = ReadNextPoint(record);

    return eof;
}

}}} // namespace liblas::detail::v11
