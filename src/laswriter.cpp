#include <liblas/laswriter.hpp>
#include <liblas/detail/writer.hpp>
// std
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>

namespace liblas
{

LASWriter::LASWriter(std::ofstream& ofs, LASHeader const& header) :
    m_pimpl(detail::WriterFactory::Create(ofs, header)), m_header(header)
{
    m_pimpl->WriteHeader(m_header);
}

LASWriter::~LASWriter()
{
    // empty, but required so we can implement PIMPL using
    // std::auto_ptr with incomplete type (Reader).
}

std::size_t LASWriter::GetVersion() const
{
    return m_pimpl->GetVersion();
}

} // namespace liblas

