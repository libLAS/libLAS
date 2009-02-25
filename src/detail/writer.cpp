/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS writer implementation for C++ libLAS 
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

#include <liblas/detail/writer.hpp>
#include <liblas/detail/writer10.hpp>
#include <liblas/detail/writer11.hpp>
#include <liblas/detail/writer12.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lassrs.hpp>

#ifdef HAVE_GDAL
#include <ogr_srs_api.h>
#endif


// std
#include <fstream>
#include <cassert>
#include <cstdlib> // std::size_t
#include <stdexcept>

namespace liblas { namespace detail {

Writer::Writer(std::ostream& ofs) : m_ofs(ofs), m_transform(0)
{
}

Writer::~Writer()
{
#ifdef HAVE_GDAL
    if (m_transform) {
        OCTDestroyCoordinateTransformation(m_transform);
    }    
#endif
}

std::ostream& Writer::GetStream() const
{
    return m_ofs;
}


void Writer::FillPointRecord(PointRecord& record, const LASPoint& point, const LASHeader& header) 
{
    record.x = static_cast<int32_t>((point.GetX() - header.GetOffsetX()) / header.GetScaleX());
    record.y = static_cast<int32_t>((point.GetY() - header.GetOffsetY()) / header.GetScaleY());
    record.z = static_cast<int32_t>((point.GetZ() - header.GetOffsetZ()) / header.GetScaleZ());

    if (m_transform) Project(record);

    record.intensity = point.GetIntensity();
    record.flags = point.GetScanFlags();
    record.classification = point.GetClassification();
    record.scan_angle_rank = point.GetScanAngleRank();
    record.user_data = point.GetUserData();
    record.point_source_id = point.GetPointSourceID();
}

void Writer::WriteVLR(LASHeader const& header) 
{
    m_ofs.seekp(header.GetHeaderSize(), std::ios::beg);

    for (uint32_t i = 0; i < header.GetRecordsCount(); ++i)
    {
        LASVLR vlr = header.GetVLR(i);

        detail::write_n(m_ofs, vlr.GetReserved(), sizeof(uint16_t));
        detail::write_n(m_ofs, vlr.GetUserId(true).c_str(), 16);
        detail::write_n(m_ofs, vlr.GetRecordId(), sizeof(uint16_t));
        detail::write_n(m_ofs, vlr.GetRecordLength(), sizeof(uint16_t));
        detail::write_n(m_ofs, vlr.GetDescription(true).c_str(), 32);
        std::vector<uint8_t> const& data = vlr.GetData();
        std::streamsize const size = static_cast<std::streamsize>(data.size());
        detail::write_n(m_ofs, data.front(), size);
    }
}

void Writer::SetSRS(const LASSRS& srs)
{
    m_out_srs = srs;
#ifdef HAVE_GDAL
    OGRSpatialReferenceH in_ref = OSRNewSpatialReference(NULL);
    OGRSpatialReferenceH out_ref = OSRNewSpatialReference(NULL);

    const char* in_wkt = m_in_srs.GetWKT().c_str();
    if (OSRImportFromWkt(in_ref, (char**) &in_wkt) != OGRERR_NONE) 
    {
        throw std::runtime_error("Could not import input spatial reference for Reader::");
    }
    
    const char* out_wkt = m_out_srs.GetWKT().c_str();
    if (OSRImportFromWkt(out_ref, (char**) &out_wkt) != OGRERR_NONE) 
    {
        throw std::runtime_error("Could not import output spatial reference for Reader::");
    }

    m_transform = OCTNewCoordinateTransformation( in_ref, out_ref);
    
#endif
}

void Writer::Project(PointRecord& point)
{
#ifdef HAVE_GDAL
    
    int ret = 0;
    double x = point.x;
    double y = point.y;
    double z = point.z;
    
    ret = OCTTransform(m_transform, 1, &x, &y, &z);
    
    if (ret != OGRERR_NONE) {
        throw std::runtime_error("could not project point!");
    }
    
    // FIXME: PointRecords need to be descaled
    point.x = x;
    point.y = y;
    point.z = z;
#else
    UNREFERENCED_PARAMETER(point);
#endif
}
Writer* WriterFactory::Create(std::ostream& ofs, LASHeader const& header)
{
    if (!ofs)
    {
        throw std::runtime_error("output stream state is invalid");
    }

    // Select writer implementation based on requested LAS version.
    uint8_t major = header.GetVersionMajor();
    uint8_t minor = header.GetVersionMinor();
    
    if (1 == major && 0 == minor)
    {
        return new v10::WriterImpl(ofs);
    }
    if (1 == major && 1 == minor)
    {
        return new v11::WriterImpl(ofs);
    }
    if (1 == major && 2 == minor)
    {
        return new v12::WriterImpl(ofs);
    }
    else if (2 == major && 0 == minor)
    {
        // TODO: LAS 2.0 read/write support
        throw std::runtime_error("LAS 2.0 file detected but unsupported");
    }

    throw std::runtime_error("LAS file of unknown version");
}

void WriterFactory::Destroy(Writer* p) 
{
    delete p;
    p = 0;
}

}} // namespace liblas::detail
