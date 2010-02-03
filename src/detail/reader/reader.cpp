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

#include <liblas/detail/reader/reader.hpp>
#include <liblas/detail/reader/header.hpp>
#include <liblas/detail/reader/vlr.hpp>

#include <liblas/detail/utility.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/liblas.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>

// std
#include <fstream>
#include <istream>
#include <iostream>
#include <stdexcept>
#include <cstddef> // std::size_t
#include <cstdlib> // std::free
#include <cassert>

namespace liblas { namespace detail { 

ReaderImpl::ReaderImpl(std::istream& ifs) :
    m_ifs(ifs), m_size(0), m_current(0),
    m_transform(0), m_in_ref(0), m_out_ref(0), m_point_reader(0)
{
}

ReaderImpl::~ReaderImpl()
{
#ifdef HAVE_GDAL
    if (m_transform)
    {
        OCTDestroyCoordinateTransformation(m_transform);
    }
    if (m_in_ref)
    {
        OSRDestroySpatialReference(m_in_ref);
    }
    if (m_out_ref)
    {
        OSRDestroySpatialReference(m_out_ref);
    }

#endif

    delete m_point_reader;
}

std::istream& ReaderImpl::GetStream() const
{
    return m_ifs;
}

bool ReaderImpl::ReadVLR(LASHeader& header)
{
    detail::reader::VLR reader(m_ifs, header);
    reader.read();
    header = reader.GetHeader();

    LASSpatialReference srs(header.GetVLRs());    
    header.SetSRS(srs);

    // keep a copy on the reader in case we're going to reproject data 
    // on the way out.
    m_in_srs = srs;

    return true;
}


void ReaderImpl::Reset(LASHeader const& header)
{
    m_ifs.clear();
    m_ifs.seekg(0);

    // Reset sizes and set internal cursor to the beginning of file.
    m_current = 0;
    m_size = header.GetPointRecordsCount();
    
    // If we reset the reader, we're ready to start reading points, so 
    // we'll create a point reader at this point.
    if (m_point_reader == 0) {
        if (m_transform != 0) {
            m_point_reader = new detail::reader::Point(m_ifs, header, m_transform);
        } else {
            m_point_reader = new detail::reader::Point(m_ifs, header);
        }
    } 
}

void ReaderImpl::SetOutputSRS(const LASSpatialReference& srs, const LASHeader& header)
{
    m_out_srs = srs;
    CreateTransform();
    
    // reset the point reader to include our new transform
    if (m_point_reader != 0) {
        delete m_point_reader;
        m_point_reader = 0;
        m_point_reader = new detail::reader::Point(m_ifs, header, m_transform);
    }
}

void ReaderImpl::SetSRS(const LASSpatialReference& srs, const LASHeader& header)
{
    SetOutputSRS(srs, header);
}

void ReaderImpl::SetInputSRS(const LASSpatialReference& srs)
{
    m_in_srs = srs;
}


void ReaderImpl::CreateTransform(){
#ifdef HAVE_GDAL
    
    if (m_transform)
    {
        OCTDestroyCoordinateTransformation(m_transform);
    }
    if (m_in_ref)
    {
        OSRDestroySpatialReference(m_in_ref);
    }
    if (m_out_ref)
    {
        OSRDestroySpatialReference(m_out_ref);
    }
    
    m_in_ref = OSRNewSpatialReference(0);
    m_out_ref = OSRNewSpatialReference(0);

    int result = OSRSetFromUserInput(m_in_ref, m_in_srs.GetWKT().c_str());
    if (result != OGRERR_NONE) 
    {
        std::ostringstream msg; 
        msg << "Could not import input spatial reference for Reader::" << CPLGetLastErrorMsg() << result;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }
    
    result = OSRSetFromUserInput(m_out_ref, m_out_srs.GetWKT().c_str());
    if (result != OGRERR_NONE) 
    {
        std::ostringstream msg; 
        msg << "Could not import output spatial reference for Reader::" << CPLGetLastErrorMsg() << result;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }

    m_transform = OCTNewCoordinateTransformation( m_in_ref, m_out_ref);
    
#endif
}


bool ReaderImpl::ReadHeader(LASHeader& header)
{
    using detail::read_n;
    using detail::reader::Header;
    
    Header h(m_ifs);
    h.read();
    header = h.GetHeader();
    
    Reset(header);

    return true;
}

bool ReaderImpl::ReadNextPoint(LASPoint& point, const LASHeader& header)
{
    if (0 == m_current)
    {
        m_ifs.clear();
        m_ifs.seekg(header.GetDataOffset(), std::ios::beg);
    }

    if (m_current < m_size)
    {
        m_point_reader->read();
        point = m_point_reader->GetPoint();
        ++m_current;
        return true;
    }

    return false;
}

bool ReaderImpl::ReadPointAt(std::size_t n, LASPoint& point, const LASHeader& header)
{
    if (m_size <= n)
        return false;

    std::streamsize pos = (static_cast<std::streamsize>(n) * header.GetDataRecordLength()) + header.GetDataOffset();    

    m_ifs.clear();
    m_ifs.seekg(pos, std::ios::beg);

    m_point_reader->read();
    point = m_point_reader->GetPoint();


    return true;
}


ReaderImpl* ReaderFactory::Create(std::istream& ifs)
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

    return new ReaderImpl(ifs);
    
    // if (1 == verMajor && 0 == verMinor)
    // {
    // 
    //     return new ReaderImpl(ifs);
    // }
    // else if (1 == verMajor && 1 == verMinor)
    // {
    //     return new v11::ReaderImpl(ifs);
    // }
    // else if (1 == verMajor && 2 == verMinor)
    // {
    //     return new v12::ReaderImpl(ifs);
    // }
    // else if (2 == verMajor && 0 == verMinor )
    // {
    //     // TODO: LAS 2.0 read/write support
    //     throw std::runtime_error("LAS 2.0+ file detected but unsupported");
    // }

    // throw std::runtime_error("LAS file of unknown version");
}


void ReaderFactory::Destroy(ReaderImpl* p) 
{
    delete p;
    p = 0;
}
}} // namespace liblas::detail

