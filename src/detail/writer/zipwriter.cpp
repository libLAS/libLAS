/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS 1.0 writer implementation for C++ libLAS 
 * Author:   Michael P. Gerlek (mpg@flaxen.com)
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

#ifdef HAVE_LASZIP

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/liblas.hpp>
#include <liblas/detail/writer/zipwriter.hpp>
#include <liblas/detail/writer/header.hpp>
#include <liblas/detail/writer/point.hpp>
#include <liblas/detail/private_utility.hpp>
// laszip
#include <laszip/laszipper.hpp>
// std
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstdlib> // std::size_t
#include <cassert>

namespace liblas { namespace detail { 

ZipWriterImpl::ZipWriterImpl(std::ostream& ofs) :
    m_ofs(ofs), 
    //m_point_writer(PointWriterPtr( )), 
    m_header_writer(HeaderWriterPtr()), 
    m_pointCount(0),
    m_zipper(NULL)
{
    return;
}


void ZipWriterImpl::WriteHeader()
{
    m_header_writer = HeaderWriterPtr(new writer::Header(m_ofs,m_pointCount, *m_header) );
    
    m_header_writer->write();
    
    m_header = HeaderPtr(new liblas::Header(m_header_writer->GetHeader()));
}

void ZipWriterImpl::UpdatePointCount(boost::uint32_t count)
{
    boost::uint32_t out = m_pointCount;
    
    if ( count != 0 ) { out = count; }
    
    if (!m_ofs.good() ) return;
    // Skip to first byte of number of point records data member
    std::streamsize const dataPos = 107; 
    m_ofs.seekp(dataPos, std::ios::beg);
    detail::write_n(m_ofs, out , sizeof(out));
}

// BUG: duplicated in ZipReaderImpl
void ZipWriterImpl::ConstructItems()
{
    PointFormatName format = m_header->GetDataFormatId();

    switch (format)
    {
    case ePointFormat0:
        m_num_items = 1;
        m_items = new LASitem[1];
        m_items[0].set(LASitem::POINT10);
        break;

    case ePointFormat1:
        m_num_items = 2;
        m_items = new LASitem[2];
        m_items[0].set(LASitem::POINT10);
        m_items[1].set(LASitem::GPSTIME);
        break;

    case ePointFormat2:
        m_num_items = 2;
        m_items = new LASitem[2];
        m_items[0].set(LASitem::POINT10);
        m_items[1].set(LASitem::RGB);
        break;

    case ePointFormat3:
        m_num_items = 3;
        m_items = new LASitem[3];
        m_items[0].set(LASitem::POINT10);
        m_items[1].set(LASitem::GPSTIME);
        m_items[2].set(LASitem::RGB);
        break;

    case ePointFormat4:
        m_num_items = 3;
        m_items = new LASitem[3];
        m_items[0].set(LASitem::POINT10);
        m_items[1].set(LASitem::GPSTIME);
        m_items[2].set(LASitem::WAVEPACKET);
        break;

    default:
        throw 0;
    }

    // construct the object that will hold a laszip point

    // compute the point size
    m_lz_point_size = 0;
    for (unsigned int i = 0; i < m_num_items; i++) 
        m_lz_point_size += m_items[i].size;

    // create the point data
    unsigned int point_offset = 0;
    m_lz_point = new unsigned char*[m_num_items];
    m_lz_point_data = new unsigned char[m_lz_point_size];
    for (unsigned i = 0; i < m_num_items; i++)
    {
        m_lz_point[i] = &(m_lz_point_data[point_offset]);
        point_offset += m_items[i].size;
    }

    return;
}


void ZipWriterImpl::WritePoint(liblas::Point const& point)
{
    //if (m_point_writer.get() == 0) {
    //    m_point_writer = PointWriterPtr(new writer::Point(m_ofs, m_pointCount, m_header));
    //} 
    //m_point_writer->write(point);

    if (m_zipper==NULL)
    {
        m_zipper = new LASzipper();

        ConstructItems();

        bool ok = m_zipper->open(&m_ofs, m_num_items, m_items, LASZIP_COMPRESSION_NONE);
        if (!ok) throw 0; // BUG: status code
    }

    const std::vector<boost::uint8_t>& v = point.GetData();
    for (unsigned int i=0; i<m_lz_point_size; i++)
        m_lz_point_data[i] = v[i];

    bool ok = m_zipper->write(m_lz_point);
    if (!ok) throw 0;

    return;
}

ZipWriterImpl::~ZipWriterImpl()
{
    // Try to update the point count on our way out, but we don't really
    // care if we weren't able to write it.
    try
    {
        UpdatePointCount(0);
        
    } catch (std::runtime_error const&)
    {
        
    }

    delete m_zipper;
}

void ZipWriterImpl::SetFilters(std::vector<liblas::FilterPtr> const& filters)
{
    m_filters = filters;
}

void ZipWriterImpl::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    m_transforms = transforms;
}

liblas::Header& ZipWriterImpl::GetHeader() const
{
    return *m_header;
}
void ZipWriterImpl::SetHeader(liblas::Header const& header)
{
    m_header = HeaderPtr(new liblas::Header(header));
}


}} // namespace liblas::detail

#endif // HAVE_LASZIP
