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

#include <liblas/detail/writer/writer10.hpp>
#include <liblas/detail/writer/header.hpp>
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
    m_ofs(ofs), m_transform(0),  m_in_ref(0), m_out_ref(0), m_pointCount(0)
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
    // TODO: Static assert would be better
    
    double t = 0;
    assert(liblas::ePointSize0 == sizeof(m_record));
    FillPointRecord(m_record, point, header);
    detail::write_n(m_ofs, m_record, sizeof(m_record));

    if (header.GetDataFormatId() == liblas::ePointFormat1) {
        t = point.GetTime();
        detail::write_n(m_ofs, t, sizeof(double));
    }
    ++m_pointCount;
}

WriterImpl::~WriterImpl()
{
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


void WriterImpl::FillPointRecord(PointRecord& record, const LASPoint& point, const LASHeader& header) 
{

    if (m_transform) {
        // let's just copy the point for now.
        LASPoint p = LASPoint(point);
        Project(p);
        record.x = static_cast<int32_t>((p.GetX() - header.GetOffsetX()) / header.GetScaleX());
        record.y = static_cast<int32_t>((p.GetY() - header.GetOffsetY()) / header.GetScaleY());
        record.z = static_cast<int32_t>((p.GetZ() - header.GetOffsetZ()) / header.GetScaleZ());
    } else {
        record.x = static_cast<int32_t>((point.GetX() - header.GetOffsetX()) / header.GetScaleX());
        record.y = static_cast<int32_t>((point.GetY() - header.GetOffsetY()) / header.GetScaleY());
        record.z = static_cast<int32_t>((point.GetZ() - header.GetOffsetZ()) / header.GetScaleZ());
    }

    LASClassification::bitset_type clsflags(point.GetClassification());
    record.classification = static_cast<uint8_t>(clsflags.to_ulong());

    record.intensity = point.GetIntensity();
    record.flags = point.GetScanFlags();
    record.scan_angle_rank = point.GetScanAngleRank();
    record.user_data = point.GetUserData();
    record.point_source_id = point.GetPointSourceID();
}


void WriterImpl::SetOutputSRS(const LASSpatialReference& srs )
{
    m_out_srs = srs;
    CreateTransform();

}

void WriterImpl::SetSRS(const LASSpatialReference& srs )
{
    SetOutputSRS(srs);
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

void WriterImpl::Project(LASPoint& p)
{
#ifdef HAVE_GDAL
    
    int ret = 0;
    double x = p.GetX();
    double y = p.GetY();
    double z = p.GetZ();
    
    ret = OCTTransform(m_transform, 1, &x, &y, &z);
    
    if (!ret) {
        std::ostringstream msg; 
        msg << "Could not project point for Writer::" << CPLGetLastErrorMsg() << ret;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }
    
    p.SetX(x);
    p.SetY(y);
    p.SetZ(z);
#else
    detail::ignore_unused_variable_warning(p);
#endif
}

}} // namespace liblas::detail
