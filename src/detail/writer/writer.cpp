/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS 1.0 writer implementation for C++ libLAS 
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

#include <liblas/detail/writer/writer.hpp>
#include <liblas/detail/writer/header.hpp>
#include <liblas/detail/writer/point.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/liblas.hpp>
// std
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstdlib> // std::size_t
#include <cassert>

namespace liblas { namespace detail { 

WriterImpl::WriterImpl(std::ostream& ofs) :
    m_ofs(ofs), m_transform(0),  m_in_ref(0), m_out_ref(0), m_point_writer(0), m_pointCount(0)
{
}


void WriterImpl::WriteHeader(LASHeader& header)
{
    detail::writer::Header hwriter(m_ofs,m_pointCount, header );
    hwriter.write();
    header = hwriter.GetHeader();
    
}

void WriterImpl::UpdateHeader(LASHeader const& header)
{
    if (m_pointCount != header.GetPointRecordsCount())
    {
        // Skip to first byte of number of point records data member
        std::streamsize const dataPos = 107; 
        m_ofs.seekp(dataPos, std::ios::beg);

        detail::write_n(m_ofs, m_pointCount , sizeof(m_pointCount));
    }
}

void WriterImpl::WritePointRecord(LASPoint const& point, const LASHeader& header)
{
    if (m_point_writer == 0) {
        if (m_transform != 0) {
            m_point_writer = new detail::writer::Point(m_ofs, m_pointCount, header, m_transform);
        } else {
            m_point_writer = new detail::writer::Point(m_ofs, m_pointCount, header);
        }
    } 
    m_point_writer->write(point);

}

WriterImpl::~WriterImpl()
{
    if (m_point_writer != 0)
        delete m_point_writer;
        
#ifdef HAVE_GDAL
    if (m_transform) {
        OCTDestroyCoordinateTransformation(m_transform);
    }
    if (m_in_ref) {
        OSRDestroySpatialReference(m_in_ref);
    }
    if (m_out_ref) {
        OSRDestroySpatialReference(m_out_ref);
    }
#endif
}

std::ostream& WriterImpl::GetStream() const
{
    return m_ofs;
}


void WriterImpl::SetOutputSRS(const LASSpatialReference& srs, const LASHeader& header )
{
    m_out_srs = srs;
    CreateTransform();

    // reset the point writer to include our new transform
    if (m_point_writer != 0) {
        delete m_point_writer;
        m_point_writer = 0;
        m_point_writer = new detail::writer::Point(m_ofs, m_pointCount, header, m_transform);
    }
}

void WriterImpl::SetSRS(const LASSpatialReference& srs , const LASHeader& header)
{
    SetOutputSRS(srs, header);
}

void WriterImpl::SetInputSRS(const LASSpatialReference& srs )
{
    m_in_srs = srs;
}

void WriterImpl::CreateTransform()
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
    
    m_in_ref = OSRNewSpatialReference(0);
    m_out_ref = OSRNewSpatialReference(0);

    int result = OSRSetFromUserInput(m_in_ref, m_in_srs.GetWKT().c_str());
    if (result != OGRERR_NONE) 
    {
        std::ostringstream msg; 
        msg << "Could not import input spatial reference for Writer::" << CPLGetLastErrorMsg() << result;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }
    
    result = OSRSetFromUserInput(m_out_ref, m_out_srs.GetWKT().c_str());
    if (result != OGRERR_NONE) 
    {
        std::ostringstream msg; 
        msg << "Could not import output spatial reference for Writer::" << CPLGetLastErrorMsg() << result;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }

    m_transform = OCTNewCoordinateTransformation( m_in_ref, m_out_ref);
#endif
}



WriterImpl* WriterFactory::Create(std::ostream& ofs, LASHeader const& header)
{
    if (!ofs)
    {
        throw std::runtime_error("output stream state is invalid");
    }
    
    return new detail::WriterImpl(ofs);

}

void WriterFactory::Destroy(detail::WriterImpl* p) 
{
    delete p;
    p = 0;
}

}} // namespace liblas::detail
