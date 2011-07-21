/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  laszip writer implementation for C++ libLAS 
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

#include <liblas/liblas.hpp>
#include <liblas/detail/writer/zipwriter.hpp>
#include <liblas/detail/writer/header.hpp>
#include <liblas/detail/writer/point.hpp>
#include <liblas/detail/private_utility.hpp>
#include <liblas/detail/zippoint.hpp>
// laszip
#include <laszip/laszipper.hpp>
#include <laszip/laszip.hpp>
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
    m_zipper(NULL),
    m_zipPoint(NULL)
{
    return;
}


void ZipWriterImpl::WriteHeader()
{
    m_header_writer = HeaderWriterPtr(new writer::Header(m_ofs, m_pointCount, *m_header) );
    
    m_header_writer->write();
    
    m_header = HeaderPtr(new liblas::Header(m_header_writer->GetHeader()));

    if (!m_zipper)
    {
        boost::scoped_ptr<LASzipper> z(new LASzipper());
        m_zipper.swap(z);

        bool stat(false);

        stat = m_zipper->open(m_ofs, m_zipPoint->GetZipper());
        if (!stat)
        {
            std::ostringstream oss;
            oss << "Error opening LASzipper: " << std::string(m_zipPoint->GetZipper()->get_error());
            throw liblas_error(oss.str());
        }
    }
    
}

void ZipWriterImpl::UpdatePointCount(boost::uint32_t count)
{
    std::streamoff orig_pos = m_ofs.tellp();

    boost::uint32_t out = m_pointCount;
    
    if ( count != 0 ) { out = count; }
    
    if (!m_ofs.good() ) return;
    // Skip to first byte of number of point records data member
    std::streamsize const dataPos = 107; 
    m_ofs.seekp(dataPos, std::ios::beg);
    detail::write_n(m_ofs, out , sizeof(out));

    m_ofs.seekp(orig_pos, std::ios::beg);
}


void ZipWriterImpl::WritePoint(liblas::Point const& point)
{

    bool ok = false;
    const std::vector<boost::uint8_t>* data = &point.GetData();
    assert(data->size() == m_zipPoint->m_lz_point_size);

    for (unsigned int i=0; i<m_zipPoint->m_lz_point_size; i++)
    {
        m_zipPoint->m_lz_point_data[i] = data->at(i);
        //printf("%d %d\n", v[i], i);
    }

    ok = m_zipper->write(m_zipPoint->m_lz_point);

    if (!ok)
    {
        std::ostringstream oss;
        oss << "Error writing compressed point data: " << std::string(m_zipper->get_error());
        throw liblas_error(oss.str());
    }
    ++m_pointCount;
    m_header->SetPointRecordsCount(m_pointCount);
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
        // ignore?
    }


    m_zipper.reset();
    m_zipPoint.reset();

}

void ZipWriterImpl::SetFilters(std::vector<liblas::FilterPtr> const& filters)
{
    m_filters = filters;
}
std::vector<liblas::FilterPtr>  ZipWriterImpl::GetFilters() const
{
    return m_filters;
}

void ZipWriterImpl::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    m_transforms = transforms;
}
std::vector<liblas::TransformPtr>  ZipWriterImpl::GetTransforms() const
{
    return m_transforms;
}

liblas::Header& ZipWriterImpl::GetHeader() const
{
    return *m_header;
}
void ZipWriterImpl::SetHeader(liblas::Header const& header)
{
    m_header = HeaderPtr(new liblas::Header(header));

    if (!m_zipPoint)
    {
            
        PointFormatName format = m_header->GetDataFormatId();

        boost::scoped_ptr<ZipPoint> z(new ZipPoint(format, m_header->GetVLRs()));
        m_zipPoint.swap(z);
    }


}


}} // namespace liblas::detail

#endif // HAVE_LASZIP
