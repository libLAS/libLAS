// $Id$
//
// (C) Copyright Howard Butler 2008
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/lasfile.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/detail/file.hpp>

#include <string>
#include <stdexcept>

namespace liblas {

LASFile::LASFile()
{
}

LASFile::LASFile(std::string const& filename)
    : m_pimpl(new detail::FileImpl(filename))
{
}

LASFile::LASFile(std::string const& filename, LASHeader const& header, Mode mode)
    : m_pimpl(new detail::FileImpl(filename, header, mode))
{
}

LASFile::LASFile(LASFile const& other)
    : m_pimpl(other.m_pimpl)
{
}

LASFile& LASFile::operator=(LASFile const& rhs)
{
    m_pimpl = rhs.m_pimpl;
    return (*this);
}

bool LASFile::IsNull() const
{
     return (0 == m_pimpl.get());
}

std::string LASFile::GetName() const
{
    return m_pimpl->GetName();
}

LASFile::Mode LASFile::GetMode() const
{
    int const mode = m_pimpl->GetMode();
    if (mode == 0)
        return eRead;
    else if (mode == 1)
        return eWrite;
    else if (mode == 2)
        return eAppend;

    assert("SHOULD NEVER GET HERE");
    return eRead;
}

LASHeader const& LASFile::GetHeader() const
{
    return m_pimpl->GetHeader();
}

LASReader& LASFile::GetReader()
{
    return m_pimpl->GetReader();
}

LASWriter& LASFile::GetWriter()
{
    return m_pimpl->GetWriter();
}

} // namespace liblas
