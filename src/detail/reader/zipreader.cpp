/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  laszip reader implementation for C++ libLAS 
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
#include <liblas/detail/reader/zipreader.hpp>
#include <liblas/detail/private_utility.hpp>
#include <liblas/detail/zippoint.hpp>
// laszip
#include <laszip/lasunzipper.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <fstream>
#include <istream>
#include <iostream>
#include <stdexcept>
#include <cstddef> // std::size_t
#include <cstdlib> // std::free
#include <cassert>

using namespace boost;

namespace liblas { namespace detail { 


ZipReaderImpl::ZipReaderImpl(std::istream& ifs)
    : m_ifs(ifs)
    , m_size(0)
    , m_current(0)
    , m_header_reader(new reader::Header(m_ifs))
    , m_header(HeaderPtr(new liblas::Header(DefaultHeader::get())))
    , m_point(PointPtr(new liblas::Point(m_header.get())))
    , m_filters(0)
    , m_transforms(0)
    , bNeedHeaderCheck(false)
    , m_zipReadStartPosition(0)
{
    return;
}

ZipReaderImpl::~ZipReaderImpl()
{
    if (m_unzipper)
    {
        m_unzipper->close();
    }

    m_zipPoint.reset();
    m_unzipper.reset();

    return;
}

void ZipReaderImpl::Reset()
{
    m_ifs.clear();
    m_ifs.seekg(0);

    // Reset sizes and set internal cursor to the beginning of file.
    m_current = 0;
    m_size = m_header->GetPointRecordsCount();


    if (!m_zipPoint)
    {
    
        PointFormatName format = m_header->GetDataFormatId();
        boost::scoped_ptr<ZipPoint> z(new ZipPoint(format, m_header->GetVLRs()));
        m_zipPoint.swap(z);
    
    }

    if (!m_unzipper)
    {
        boost::scoped_ptr<LASunzipper> z(new LASunzipper());
        m_unzipper.swap(z);

        bool ok(false);
        m_ifs.seekg(m_header->GetDataOffset(), std::ios::beg);
        ok = m_unzipper->open(m_ifs, m_zipPoint->GetZipper());

        m_zipReadStartPosition = m_ifs.tellg();

        if (!ok)
        {
            std::ostringstream oss;
            const char* err = m_unzipper->get_error();
            if (err==NULL) err="(unknown error)";
            oss << "Failed to open LASzip stream: " << std::string(err);
            throw liblas_error(oss.str());
        }
    }
    return;
}

void ZipReaderImpl::TransformPoint(liblas::Point& p)
{    

    // Apply the transforms to each point
    std::vector<liblas::TransformPtr>::const_iterator ti;

    for (ti = m_transforms.begin(); ti != m_transforms.end(); ++ti)
    {
        liblas::TransformPtr transform = *ti;
        transform->transform(p);
    }            
}


bool ZipReaderImpl::FilterPoint(liblas::Point const& p)
{    
    // If there's no filters on this reader, we keep 
    // the point no matter what.
    if (m_filters.empty() ) {
        return true;
    }

    std::vector<liblas::FilterPtr>::const_iterator fi;
    for (fi = m_filters.begin(); fi != m_filters.end(); ++fi)
    {
        liblas::FilterPtr filter = *fi;
        if (!filter->filter(p))
        {
            return false;
        }
    }
    return true;
}


    
void ZipReaderImpl::ReadHeader()
{
    // If we're eof, we need to reset the state
    if (m_ifs.eof())
        m_ifs.clear();
    
    m_header_reader->ReadHeader();
    m_header = m_header_reader->GetHeader();

    if (!m_header->Compressed())
        throw liblas_error("Internal error: compressed reader encountered uncompressed header"); 

    m_point->SetHeader(m_header.get());
    Reset();
}

void ZipReaderImpl::SetHeader(liblas::Header const& header) 
{
    m_header = HeaderPtr(new liblas::Header(header));
}
    
void ZipReaderImpl::ReadIdiom()
{
    bool ok = false;

    ok = m_unzipper->read(m_zipPoint->m_lz_point);

    if (!ok)
    {
        std::ostringstream oss;
        const char* errMsg = m_unzipper->get_error();
        oss << "Error reading compressed point data: " ;
        if (errMsg)
            oss << "'" << m_unzipper->get_error() <<"'";
        else
            oss << "no message from LASzip library";
        throw liblas_error(oss.str());
    }

    {
        std::vector<uint8_t>& data = m_point->GetData();

        unsigned int size = m_zipPoint->m_lz_point_size;
        assert(size == data.size());
        
        unsigned char* p = m_zipPoint->m_lz_point_data.get();
        data.assign(p, p+size);

        ++m_current;
    }

    return;
}

void ZipReaderImpl::ReadNextPoint()
{
    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(m_zipReadStartPosition, std::ios::beg);
    }

    if (m_current >= m_size ){
        throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");
    } 

    if (bNeedHeaderCheck) 
    {
        if (!(m_point->GetHeader() == m_header.get()))
            m_point->SetHeader(m_header.get());
    }
    
    ReadIdiom();

    // Filter the points and continue reading until we either find 
    // one to keep or throw an exception.

    bool bLastPoint = false;
    if (!FilterPoint(*m_point))
    {
        ReadIdiom();

        while (!FilterPoint(*m_point))
        {
            ReadIdiom();
            if (m_current == m_size) 
            {
                bLastPoint = true;
                break;
            }
        }
    }

    if (!m_transforms.empty())
    {
        TransformPoint(*m_point);
    }

    if (bLastPoint)
        throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");


}


liblas::Point const& ZipReaderImpl::ReadPointAt(std::size_t n)
{
    Seek(n);

    ReadNextPoint();

    return this->GetPoint();
}


void ZipReaderImpl::Seek(std::size_t n)
{
    if (m_size == n) {
        throw std::out_of_range("file has no more points to read, end of file reached");
    } else if (m_size < n) {
        std::ostringstream msg;
        msg << "Seek:: Inputted value: " << n << " is greater than the number of points: " << m_size;
        throw std::runtime_error(msg.str());
    } 

    m_ifs.clear();

    m_unzipper->seek(n);

    m_current = n;
}

void ZipReaderImpl::SetFilters(std::vector<liblas::FilterPtr> const& filters)
{
    m_filters = filters;
}
std::vector<liblas::FilterPtr>  ZipReaderImpl::GetFilters() const
{
    return m_filters;
}

void ZipReaderImpl::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    m_transforms = transforms;
    
    // Transforms are allowed to change the point, including moving the 
    // point's HeaderPtr.  We need to check if we need to set that 
    // back on any subsequent reads.
    for (std::vector<liblas::TransformPtr>::const_iterator i = transforms.begin(); i != transforms.end(); i++)
    {
        if (i->get()->ModifiesHeader())
            bNeedHeaderCheck = true;
    }

}

std::vector<liblas::TransformPtr>  ZipReaderImpl::GetTransforms() const
{
    return m_transforms;
}

}} // namespace liblas::detail

#endif // HAVE_LASZIP
