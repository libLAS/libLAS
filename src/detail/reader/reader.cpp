/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS 1.0 reader implementation for C++ libLAS 
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ******************************************************************************
 * Copyright (c) 2008, Mateusz Loskot
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

#include <liblas/liblas.hpp>
#include <liblas/detail/reader/reader.hpp>
#include <liblas/detail/private_utility.hpp>
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

ReaderImpl::ReaderImpl(std::istream& ifs)
    : m_ifs(ifs)
    , m_size(0)
    , m_current(0)
    // , m_point_reader(PointReaderPtr())
    , m_header_reader(new reader::Header(m_ifs))
    , m_header(HeaderPtr())
    , m_point(PointPtr(new liblas::Point()))
    , m_filters(0)
    , m_transforms(0)
    , bNeedHeaderCheck(false)
{

}

ReaderImpl::~ReaderImpl()
{
}

void ReaderImpl::Reset()
{
    m_ifs.clear();
    m_ifs.seekg(0);

    // Reset sizes and set internal cursor to the beginning of file.
    m_current = 0;
    m_size = m_header->GetPointRecordsCount();

    m_record_size = m_header->GetSchema().GetByteSize();
}

void ReaderImpl::TransformPoint(liblas::Point& p)
{    

    // Apply the transforms to each point
    std::vector<liblas::TransformPtr>::const_iterator ti;

    for (ti = m_transforms.begin(); ti != m_transforms.end(); ++ti)
    {
        liblas::TransformPtr transform = *ti;
        transform->transform(p);
    }            
}


bool ReaderImpl::FilterPoint(liblas::Point const& p)
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


    
void ReaderImpl::ReadHeader()
{
    // If we're eof, we need to reset the state
    if (m_ifs.eof())
        m_ifs.clear();
    
    m_header_reader->ReadHeader();
    m_header = m_header_reader->GetHeader();
    
    if (m_header->Compressed())
        throw std::runtime_error("Internal error: uncompressed reader encountered compressed header"); 
        
    m_point->SetHeader(HeaderOptionalConstRef(*m_header));


    Reset();
}

void ReaderImpl::SetHeader(liblas::Header const& header) 
{
    m_header = HeaderPtr(new liblas::Header(header));
    m_point->SetHeader(HeaderOptionalConstRef(header));
}
    
void ReaderImpl::ReadNextPoint()
{
    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(m_header->GetDataOffset(), std::ios::beg);
    }

    if (m_current >= m_size ){
        throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");
    } 

    if (bNeedHeaderCheck) 
    {
        if (!(m_point->GetHeader().get() == *m_header))
            m_point->SetHeader(HeaderOptionalConstRef(*m_header));
    }
    
    try
    {
        detail::read_n(m_point->GetData().front(), m_ifs, m_record_size);
        ++m_current;
        
    } catch (std::runtime_error&)
    {
        // If the stream is no good anymore, we're done reading points
        throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");

    }

    // Filter the points and continue reading until we either find 
    // one to keep or throw an exception.

    bool bLastPoint = false;
    
    if (!m_filters.empty())
    {
        if (!FilterPoint(*m_point))
        {

            try
            {
                detail::read_n(m_point->GetData().front(), m_ifs, m_record_size);
                ++m_current;
        
            } catch (std::runtime_error&)
            {
                // If the stream is no good anymore, we're done reading points
                throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");
            }

            while (!FilterPoint(*m_point))
            {

                try
                {
                    detail::read_n(m_point->GetData().front(), m_ifs, m_record_size);
                    ++m_current;
        
                } catch (std::runtime_error&)
                {
                    // If the stream is no good anymore, we're done reading points
                    throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");
                }
                if (m_current == m_size) 
                {
                    bLastPoint = true;
                    break;
                }
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

liblas::Point const& ReaderImpl::ReadPointAt(std::size_t n)
{
    if (m_size == n) {
        throw std::out_of_range("file has no more points to read, end of file reached");
    } else if (m_size < n) {
        std::ostringstream msg;
        msg << "ReadPointAt:: Inputted value: " << n << " is greater than the number of points: " << m_size;
        throw std::runtime_error(msg.str());
    } 

    std::streamsize const pos = (static_cast<std::streamsize>(n) * m_header->GetDataRecordLength()) + m_header->GetDataOffset();    

    m_ifs.clear();
    m_ifs.seekg(pos, std::ios::beg);

    if (bNeedHeaderCheck) 
    {
        if (!(m_point->GetHeader().get() == *m_header))
            m_point->SetHeader(HeaderOptionalConstRef(*m_header));
    }
    
    detail::read_n(m_point->GetData().front(), m_ifs, m_record_size);

    if (!m_transforms.empty())
    {
        TransformPoint(*m_point);
    }
    return *m_point;
}

void ReaderImpl::Seek(std::size_t n)
{
    if (m_size == n) {
        throw std::out_of_range("file has no more points to read, end of file reached");
    } else if (m_size < n) {
        std::ostringstream msg;
        msg << "Seek:: Inputted value: " << n << " is greater than the number of points: " << m_size;
        throw std::runtime_error(msg.str());
    } 

    std::streamsize pos = (static_cast<std::streamsize>(n) * m_header->GetDataRecordLength()) + m_header->GetDataOffset();    

    m_ifs.clear();
    m_ifs.seekg(pos, std::ios::beg);
    
    m_current = n;
}

void ReaderImpl::SetFilters(std::vector<liblas::FilterPtr> const& filters)
{
    m_filters = filters;
}

std::vector<liblas::FilterPtr>  ReaderImpl::GetFilters() const
{
    return m_filters;
}

void ReaderImpl::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    m_transforms = transforms;
    
    // Transforms are allowed to change the point, including moving the 
    // point's HeaderPtr.  We need to check if we need to set that 
    // back on any subsequent reads.
    if (m_transforms.size() > 0)
    {
        for (std::vector<liblas::TransformPtr>::const_iterator i = transforms.begin(); i != transforms.end(); i++)
        {
            if (i->get()->ModifiesHeader())
                bNeedHeaderCheck = true;
        }
    }
}

std::vector<liblas::TransformPtr>  ReaderImpl::GetTransforms() const
{
    return m_transforms;
}

}} // namespace liblas::detail

