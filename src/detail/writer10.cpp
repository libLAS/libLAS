#include <liblas/detail/writer10.hpp>
#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <fstream>
#include <stdexcept>
#include <cstdlib> // std::size_t
#include <cassert>

namespace liblas { namespace detail { namespace v10 {

WriterImpl::WriterImpl(std::ofstream& ofs) : Base(), m_ofs(ofs)
{
    throw std::runtime_error("TODO: writing LAS 1.0 not finished");
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

    return false;
}

bool WriterImpl::WritePoint(LASPoint const& point)
{
    // TODO: to be implemented
    // TODO: should we return anything or just declare it as void?

    return false;
}

}}} // namespace liblas::detail::v10
