// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/detail/reader.hpp>
#include <liblas/detail/reader10.hpp>
#include <liblas/detail/reader11.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>

// std
#include <fstream>
#include <cassert>
#include <cstdlib> // std::size_t
#include <stdexcept>

namespace liblas { namespace detail {

Reader::Reader() : m_offset(0), m_current(0)
{
}

Reader::~Reader()
{
}

Reader* ReaderFactory::Create(std::ifstream& ifs)
{
    if (!ifs)
    {
        throw std::runtime_error("input stream state is invalid");
    }

    // Determine version of given LAS file and
    // instantiate appropriate reader.
    uint8_t verMajor = 0;
    uint8_t verMinor = 0;
    ifs.seekg(24, std::ios::beg);
    detail::read_n(verMajor, ifs, 1);
    detail::read_n(verMinor, ifs, 1);

    if (1 == verMajor && 0 == verMinor)
    {
        return new v10::ReaderImpl(ifs);
    }
    else if (1 == verMajor && 1 == verMinor)
    {
        return new v11::ReaderImpl(ifs);
    }
    else if (2 == verMajor && 0 == verMinor )
    {
        // TODO: LAS 2.0 read/write support
        throw std::runtime_error("LAS 2.0+ file detected but unsupported");
    }

    throw std::runtime_error("LAS file of unknown version");
}

void ReaderFactory::Destroy(Reader* p) 
{
    delete p;
    p = 0;
}

}} // namespace liblas::detail
