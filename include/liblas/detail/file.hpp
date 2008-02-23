// $Id$
//
// (C) Copyright Howard Butler 2008
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_DETAIL_FILE_HPP_INCLUDED
#define LIBLAS_DETAIL_FILE_HPP_INCLUDED

#include <liblas/lasreader.hpp>
#include <liblas/laswriter.hpp>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cassert>

namespace liblas { namespace detail {

class FileImpl
{
public:

    FileImpl(std::string const& filename);
    FileImpl(std::string const& filename, LASHeader const& header, int mode);
    ~FileImpl();

    std::string GetName() const;
    int GetMode() const;
    LASHeader const& GetHeader() const;
    LASReader& GetReader();
    LASWriter& GetWriter();

private:

    // Blocked copying operations, declared but not defined.
    FileImpl(FileImpl const& other);
    FileImpl& operator=(FileImpl const& rhs);

    int m_mode;
    std::string m_filename;
    std::istream* m_istrm;
    std::ostream* m_ostrm;
    LASReader* m_reader;
    LASWriter* m_writer;
    LASHeader m_header;

    void throw_no_file_error() const;
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_FILE_HPP_INCLUDED
