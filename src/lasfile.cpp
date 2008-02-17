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
    printf("CREATING LASFile default constructor\n");
}

LASFile::LASFile(std::string const& filename)
    : m_pimpl(new detail::FileImpl(filename))
{
    printf("CREATING LASFile from filename...\n");
}

LASFile::LASFile(std::string const& filename, LASHeader const& header)
    : m_pimpl(new detail::FileImpl(filename, header))
{
    printf("CREATING LASFile from filename and header...\n");
}

LASFile::LASFile(LASFile const& other)
    : m_pimpl(other.m_pimpl)
{
    printf("CREATING LASFile with copy constructor...\n");
}

LASFile& LASFile::operator=(LASFile const& rhs)
{
    m_pimpl = rhs.m_pimpl;
    printf("LASFile operator= ...\n");
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

LASFile::~LASFile()
{
    printf("DELETING LASFile (count=%d)\n", m_pimpl.use_count());
}
} // namespace liblas
