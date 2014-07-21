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
#include <liblas/detail/reader/cachedreader.hpp>
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


CachedReaderImpl::CachedReaderImpl(std::istream& ifs , std::size_t size)
    : ReaderImpl(ifs)
    , m_cache_size(size)
    , m_cache_start_position(0)
    , m_cache_read_position(0)
    , m_cache_initialized(false)
{
}

void CachedReaderImpl::ReadHeader()
{
    ReaderImpl::ReadHeader();
    HeaderPtr hptr(new liblas::Header(ReaderImpl::GetHeader()));
    
    // If we were given no cache size, try to cache the whole thing
    if (m_cache_size == 0) {
        m_cache_size = hptr->GetPointRecordsCount();
    }

    if (m_cache_size > hptr->GetPointRecordsCount()) {
        m_cache_size = hptr->GetPointRecordsCount();
    }
    // // FIXME: Note, vector::resize never shrinks the container and frees memory! Are we aware of this fact here? --mloskot
    // m_cache.resize(m_cache_size);
    // 
    // // Mark all positions as uncached and build up the mask
    // // to the size of the number of points in the file
    // uint8_t const uncached_mask = 0;
    // cache_mask_type(hptr->GetPointRecordsCount(), uncached_mask).swap(m_mask);
    
    m_header = hptr;
}

void CachedReaderImpl::CacheData(uint32_t position) 
{
    cache_mask_type::size_type old_cache_start_position = m_cache_start_position;
    m_cache_start_position = position;

    cache_mask_type::size_type header_size = static_cast<cache_mask_type::size_type>(m_header->GetPointRecordsCount());
    cache_mask_type::size_type left_to_cache = (std::min)(m_cache_size, header_size - m_cache_start_position);

    cache_mask_type::size_type to_mark = (std::min)(m_cache_size, header_size - old_cache_start_position);

    for (uint32_t i = 0; i < to_mark; ++i)
    {
        m_mask[old_cache_start_position + i] = 0;
    }

    // if these aren't equal, we've hopped around with ReadPointAt
    // and we need to seek to the proper position.
    if (m_current != position) {
        CachedReaderImpl::Seek(position);
        m_current = position;
    }
    m_cache_read_position =  position;

    for (uint32_t i = 0; i < left_to_cache; ++i) 
    {
        try {
            m_mask[m_current] = 1;
            ReadNextUncachedPoint();
            m_cache[i] = new liblas::Point(ReaderImpl::GetPoint());
        } catch (std::out_of_range&) {
            // cached to the end
            break;
        }
    }
}

void CachedReaderImpl::ReadNextUncachedPoint()
{
    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(m_header->GetDataOffset(), std::ios::beg);
    }

    if (m_current >= m_size ){
        throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");
    } 

    // m_point_reader->read();
    detail::read_n(m_point->GetData().front(), m_ifs, m_record_size);
    ++m_current;
    // *m_point = m_point_reader->GetPoint();


    if (!m_transforms.empty())
    {
        ReaderImpl::TransformPoint(*m_point);
    }

}

void CachedReaderImpl::ReadCachedPoint(uint32_t position) {
    
    int32_t cache_position = position - m_cache_start_position ;

    // std::cout << "MASK: ";
    // std::vector<bool>::iterator it;
    // for (it = m_mask.begin(); it != m_mask.end(); ++it) {
    //     std::cout << *it << ",";
    // }
    // std::cout << std::endl;

    // If our point cache and mask have not yet been initialized, we 
    // should do so before tyring to read any points.  We don't want to do 
    // this in ::Reset or ::ReadHeader, as these functions may be called 
    // multiple times and screw up our assumptions.
    if (!m_cache_initialized) 
    {
        m_cache = cache_type(m_cache_size);
    
        // Mark all positions as uncached and build up the mask
        // to the size of the number of points in the file
        uint8_t const uncached_mask = 0;
        cache_mask_type(m_header->GetPointRecordsCount(), uncached_mask).swap(m_mask);
 
        m_cache_initialized = true;
    }
    if (m_mask[position] == 1) {
        m_cache_read_position = position;
        *m_point =  *m_cache[cache_position];
        return;
    } else {

        CacheData(position);
        
        // At this point, we can't have a negative cache position.
        // If we do, it's a big error or we'll segfault.
        cache_position = position - m_cache_start_position ;
        if (cache_position < 0) {
            std::ostringstream msg;
            msg  << "ReadCachedPoint:: cache position: " 
                 << cache_position 
                 << " is negative. position or m_cache_start_position is invalid "
                 << "position: " << position << " m_cache_start_position: "
                 << m_cache_start_position;
            throw std::runtime_error(msg.str());
        }
            
        if (m_mask[position] == 1) {
            if (static_cast<uint32_t>(cache_position) > m_cache.size()) {
                std::ostringstream msg;
                msg << "ReadCachedPoint:: cache position: " 
                    << position 
                    << " greater than cache size: " << m_cache.size() ;
                throw std::runtime_error(msg.str());
            }
            *m_point = *m_cache[cache_position];
            return;
        } else {
            std::ostringstream msg;
            msg << "ReadCachedPoint:: unable to obtain cached point"
                << " at position: " << position 
                << " cache_position was " << cache_position ;
            std::string out(msg.str());
            
            throw std::runtime_error(out);
        }
    }
}

void CachedReaderImpl::ReadNextPoint()
{
    if (m_cache_read_position == m_size )
    {
        throw std::out_of_range("file has no more points to read, end of file reached");
    }
    
    // PointPtr ptr = ReadCachedPoint(m_cache_read_position, header);
    ReadCachedPoint(m_cache_read_position);
    ++m_cache_read_position;

    // Filter the points and continue reading until we either find 
    // one to keep or throw an exception.

    bool bLastPoint = false;
    if (!FilterPoint(*m_point))
    {
        ReadCachedPoint(m_cache_read_position);
        ++m_cache_read_position;

        while (!FilterPoint(*m_point))
        {
            ReadCachedPoint(m_cache_read_position);
            ++m_cache_read_position;
            if (m_current == m_size) 
            {
                bLastPoint = true;
                break;
            }
        }
    }

    if (bLastPoint)
        throw std::out_of_range("ReadNextPoint: file has no more points to read, end of file reached");
    
}

liblas::Point const& CachedReaderImpl::ReadPointAt(std::size_t n)
{

    if (n >= m_size ){
        throw std::out_of_range("file has no more points to read, end of file reached");
    
    } else if (m_size < n) {
        std::ostringstream output;
        output << "ReadPointAt:: Inputted value: " 
               << n << " is greater than the number of points: " 
               << m_size;
        std::string out(output.str());
        throw std::runtime_error(out);
    }
    
    ReadCachedPoint(n);
    m_cache_read_position = n;
    return *m_point;
}

void CachedReaderImpl::Reset()
{
    if (m_mask.empty())
    {    
        ReaderImpl::Reset();
        return;
    }

    typedef cache_mask_type::size_type size_type;
    size_type old_cache_start_position = m_cache_start_position;
    size_type header_size = static_cast<size_type>(m_header->GetPointRecordsCount());
    size_type to_mark = (std::min)(m_cache_size, header_size - old_cache_start_position); 

    for (uint32_t i = 0; i < to_mark; ++i) {

        size_type const mark_pos = m_cache_start_position + i;
        assert(mark_pos < m_mask.size());

        m_mask[mark_pos] = 0;
    }

    m_cache_start_position = 0;
    m_cache_read_position = 0;
    m_cache_initialized = false;

    ReaderImpl::Reset();
}

void CachedReaderImpl::Seek(std::size_t n)
{

   if (n < 1) {
       CachedReaderImpl::Reset();
   }
   
   m_cache_read_position = n;
   ReaderImpl::Seek(n);
}

void CachedReaderImpl::SetFilters(std::vector<liblas::FilterPtr> const& filters)
{
    Reset();
    ReaderImpl::SetFilters(filters);
}
std::vector<liblas::FilterPtr>  CachedReaderImpl::GetFilters() const
{
    return ReaderImpl::GetFilters();
}

void CachedReaderImpl::SetTransforms(std::vector<liblas::TransformPtr> const& transforms)
{
    // Setting transforms means the cache is invalid
    Reset();
    ReaderImpl::SetTransforms(transforms);

}

std::vector<liblas::TransformPtr>  CachedReaderImpl::GetTransforms() const
{
    return ReaderImpl::GetTransforms();
}
}} // namespace liblas::detail

