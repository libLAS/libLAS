// $Id$
//
// (C) Copyright Howard Butler 2008
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_LASFILE_HPP_INCLUDED
#define LIBLAS_LASFILE_HPP_INCLUDED

#include <liblas/detail/file.hpp> // complete type required
#include <liblas/detail/sharedptr.hpp>
#include <string>
#include <cassert>

namespace liblas {

class LASFile
{
public:

    enum Mode
    {
        eRead = 0,
        eWrite = 1
    };

    LASFile();
    LASFile(std::string const& filename);
    LASFile(std::string const& filename, LASHeader const& header);
    LASFile(LASFile const& other);
    LASFile& operator=(LASFile const& rhs);

    bool IsNull() const;
    std::string GetName() const;
    Mode GetMode() const;
    LASHeader const& GetHeader() const;
    LASReader& GetReader();
    LASWriter& GetWriter();

private:

    detail::SharedPtr<detail::FileImpl> m_pimpl;
};

} // namespace liblas

#endif // LIBLAS_LASFILE_HPP_INCLUDED
