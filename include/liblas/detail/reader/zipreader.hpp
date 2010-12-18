/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  laszip reader implementation for C++ libLAS 
 * Author:   Michael P. Gerlek (mpog@flaxen.com)
 *
 ******************************************************************************
 * Copyright (c) 2010, Michael P. Gerlek
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

#ifndef LIBLAS_DETAIL_ZIPREADERIMPL_HPP_INCLUDED
#define LIBLAS_DETAIL_ZIPREADERIMPL_HPP_INCLUDED

#ifdef HAVE_LASZIP

#include <liblas/detail/fwd.hpp>
#include <liblas/detail/reader/point.hpp>
#include <liblas/detail/reader/header.hpp>
#include <liblas/liblas.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <iosfwd>
#include <boost/shared_ptr.hpp>

// liblaszip
class LASitem;
class LASunzipper;

namespace liblas { namespace detail { 

typedef boost::shared_ptr< reader::Point > PointReaderPtr;
typedef boost::shared_ptr< reader::Header > HeaderReaderPtr;

class ZipReaderImpl : public ReaderI
{
public:
    ZipReaderImpl(std::istream& ifs);
    ~ZipReaderImpl();

    void ReadHeader();
    liblas::Header const& GetHeader() const {return *m_header;}
    void SetHeader(liblas::Header const& header);
    liblas::Point const& GetPoint() const { return *m_point; }
    void ReadNextPoint();
    liblas::Point const& ReadPointAt(std::size_t n);
    void Seek(std::size_t n);
    
    void Reset();

    void SetFilters(std::vector<liblas::FilterPtr> const& filters);
    void SetTransforms(std::vector<liblas::TransformPtr> const& transforms);

protected:
    bool FilterPoint(liblas::Point const& p);
    void TransformPoint(liblas::Point& p);

    typedef std::istream::off_type off_type;
    typedef std::istream::pos_type pos_type;
    
    std::istream& m_ifs;
    boost::uint32_t m_size;
    boost::uint32_t m_current;
    
    //PointReaderPtr m_point_reader;
    HeaderReaderPtr m_header_reader;
    
    HeaderPtr m_header;
    
    PointPtr m_point;

    std::vector<liblas::FilterPtr> m_filters;
    std::vector<liblas::TransformPtr> m_transforms;

private:
    void ConstructItems();
    void ReadIdiom();

    LASunzipper* m_unzipper;
    unsigned int m_num_items;
    LASitem* m_items;
    unsigned char** m_lz_point;
    unsigned char* m_lz_point_data;
    unsigned int m_lz_point_size;

    // Blocked copying operations, declared but not defined.
    ZipReaderImpl(ZipReaderImpl const& other);
    ZipReaderImpl& operator=(ZipReaderImpl const& rhs);
};

}} // namespace liblas::detail

#endif // HAVE_LASZIP

#endif // LIBLAS_DETAIL_ZIPREADERIMPL_HPP_INCLUDED
