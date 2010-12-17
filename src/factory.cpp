/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS factories 
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2008, Mateusz Loskot
 * Copyright (c) 2010, Howard Butler
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/

#include <liblas/factory.hpp>
#include <liblas/detail/reader/reader.hpp>
#include <liblas/detail/reader/cachedreader.hpp>
#include <liblas/detail/writer/writer.hpp>
#include <liblas/utility.hpp>

// boost
#include <boost/cstdint.hpp>

// std
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring> // std::memset
#include <cassert>
#include <iostream>

using namespace boost;

namespace liblas
{

Reader ReaderFactory::CreateWithImpl(ReaderIPtr r)
{
    liblas::Reader reader(r);
    return reader;
}

Reader ReaderFactory::CreateCached(std::istream& stream, boost::uint32_t cache_size)
{
    ReaderIPtr r = ReaderIPtr(new detail::CachedReaderImpl(stream, cache_size) );
    return liblas::Reader(r);
}

Reader ReaderFactory::CreateWithStream(std::istream& stream)
{
    ReaderIPtr r = ReaderIPtr(new detail::ReaderImpl(stream) );
    return liblas::Reader(r);
}


Writer WriterFactory::CreateWithImpl(WriterIPtr w)
{
    liblas::Writer writer(w);
    return writer;
}


Writer WriterFactory::CreateWithStream(std::ostream& stream)
{

    WriterIPtr w  = WriterIPtr(new detail::WriterImpl(stream));
    return liblas::Writer(w);
}


} // namespace liblas

