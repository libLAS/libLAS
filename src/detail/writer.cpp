// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/detail/writer.hpp>
#include <liblas/detail/writer10.hpp>
#include <liblas/detail/writer11.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
#include <fstream>
#include <cassert>
#include <cstdlib> // std::size_t
#include <stdexcept>

namespace liblas { namespace detail {

Writer::Writer()
{
}

Writer::~Writer()
{
}

Writer* WriterFactory::Create(std::ofstream& ofs, LASHeader const& header)
{
    if (!ofs)
    {
        throw std::runtime_error("output stream state is invalid");
    }

    // Select writer implementation based on requested LAS version.
    uint8_t major = header.GetVersionMajor();
    uint8_t minor = header.GetVersionMinor();
    
    if (1 == major && 0 == minor)
    {
        return new v10::WriterImpl(ofs);
    }
    if (1 == major && 1 == minor)
    {
        return new v11::WriterImpl(ofs);
    }
    else if (2 == major && 0 == minor)
    {
        // TODO: LAS 2.0 read/write support
        throw std::runtime_error("LAS 2.0 file detected but unsupported");
    }

    throw std::runtime_error("LAS file of unknown version");
}

void WriterFactory::Destroy(Writer* p) 
{
    delete p;
    p = 0;
}

}} // namespace liblas::detail
