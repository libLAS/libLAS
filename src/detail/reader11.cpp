#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/reader.hpp>
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

bool ReaderImpl::ReadPoint(LASPointRecord& point)
{
    return false;
}

}}} // namespace liblas::detail::v11
