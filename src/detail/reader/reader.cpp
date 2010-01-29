/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Reader implementation for C++ libLAS 
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
#include <liblas/detail/reader/reader10.hpp>
#include <liblas/detail/reader/reader11.hpp>
#include <liblas/detail/reader/reader12.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/detail/reader/vlr.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lasclassification.hpp>
// 
// // GeoTIFF
// #ifdef HAVE_LIBGEOTIFF
// #include <geotiff.h>
// #include <geo_simpletags.h>
// #include "geo_normalize.h"
// #include "geo_simpletags.h"
// #include "geovalues.h"
// #endif // HAVE_LIBGEOTIFF

#ifdef HAVE_GDAL
#include <ogr_srs_api.h>
#endif

// std
#include <fstream>
#include <cassert>
#include <cstdlib> // std::size_t
#include <stdexcept>

namespace liblas { namespace detail {

Reader::Reader(std::istream& ifs) :
    m_ifs(ifs), m_size(0), m_current(0),
    m_transform(0), m_in_ref(0), m_out_ref(0)
{
}

Reader::~Reader()
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
}

std::istream& Reader::GetStream() const
{
    return m_ifs;
}

void Reader::FillPoint(PointRecord& record, LASPoint& point, const LASHeader& header) 
{

    if (m_transform) {
        point.SetCoordinates(header, record.x, record.y, record.z);
        Project(point);
        
        int32_t x = static_cast<int32_t>((point.GetX() - header.GetOffsetX()) / header.GetScaleX());
        int32_t y = static_cast<int32_t>((point.GetY() - header.GetOffsetY()) / header.GetScaleY());
        int32_t z = static_cast<int32_t>((point.GetZ() - header.GetOffsetZ()) / header.GetScaleZ());
        point.SetX(x);
        point.SetY(y);
        point.SetZ(z);
        
    } else {
        point.SetX(record.x);
        point.SetY(record.y);
        point.SetZ(record.z);
    }

    point.SetIntensity(record.intensity);
    point.SetScanFlags(record.flags);
    point.SetClassification((record.classification));
    point.SetScanAngleRank(record.scan_angle_rank);
    point.SetUserData(record.user_data);
    point.SetPointSourceID(record.point_source_id);
}

bool Reader::ReadVLR(LASHeader& header)
{
    detail::reader::VLR reader(m_ifs, header);
    reader.read();
    header = reader.GetHeader();

    std::cout << "vlrs size: " << header.GetVLRs().size() << std::endl;
    LASSpatialReference srs(header.GetVLRs());    
    header.SetSRS(srs);

    // keep a copy on the reader in case we're going to reproject data 
    // on the way out.
    m_in_srs = srs;

    return true;
}


void Reader::Reset(LASHeader const& header)
{
    m_ifs.clear();
    m_ifs.seekg(0);

    // Reset sizes and set internal cursor to the beginning of file.
    m_current = 0;
    m_size = header.GetPointRecordsCount();
}

void Reader::SetOutputSRS(const LASSpatialReference& srs)
{
    m_out_srs = srs;
    CreateTransform();
}

void Reader::SetSRS(const LASSpatialReference& srs)
{
    SetOutputSRS(srs);
}

void Reader::SetInputSRS(const LASSpatialReference& srs)
{
    m_in_srs = srs;
}


void Reader::CreateTransform(){
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

void Reader::Project(LASPoint& point)
{
#ifdef HAVE_GDAL
    
    int ret = 0;
    double x = point.GetX();
    double y = point.GetY();
    double z = point.GetZ();
    
    ret = OCTTransform(m_transform, 1, &x, &y, &z);    
    if (!ret)
    {
        std::ostringstream msg; 
        msg << "Could not project point for Reader::" << CPLGetLastErrorMsg() << ret;
        std::string message(msg.str());
        throw std::runtime_error(message);
    }

    point.SetX(x);
    point.SetY(y);
    point.SetZ(z);
#else
    detail::ignore_unused_variable_warning(point);
#endif
}

bool Reader::HasPointDataSignature() 
{
    uint8_t const sgn1 = 0xCC;
    uint8_t const sgn2 = 0xDD;
    uint8_t pad1 = 0x0; 
    uint8_t pad2 = 0x0;

    std::streamsize const current_pos = m_ifs.tellg();
    
    // If our little test reads off the end, we'll try to put the 
    // borken dishes back up in the cabinet
    try
    {
        detail::read_n(pad1, m_ifs, sizeof(uint8_t));
        detail::read_n(pad2, m_ifs, sizeof(uint8_t));
    }
    catch (std::out_of_range& e) 
    {
        ignore_unused_variable_warning(e);
        m_ifs.seekg(current_pos, std::ios::beg);
        return false;
    }
    catch (std::runtime_error& e)
    {
        ignore_unused_variable_warning(e);
        m_ifs.seekg(current_pos, std::ios::beg);
        return false;        
    }
    LIBLAS_SWAP_BYTES(pad1);
    LIBLAS_SWAP_BYTES(pad2);
    
    // Put the stream back where we found it
    m_ifs.seekg(current_pos, std::ios::beg);

    // FIXME: we have to worry about swapping issues
    // but some people write the pad bytes backwards 
    // anyway.  Let's check both ways.
    bool found = false;
    if (sgn1 == pad2 && sgn2 == pad1) found = true;
    if (sgn1 == pad1 && sgn2 == pad2) found = true;
    
    return found;
}
Reader* ReaderFactory::Create(std::istream& ifs)
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

    if (1 == verMajor && 0 == verMinor)
    {
        return new v10::ReaderImpl(ifs);
    }
    else if (1 == verMajor && 1 == verMinor)
    {
        return new v11::ReaderImpl(ifs);
    }
    else if (1 == verMajor && 2 == verMinor)
    {
        return new v12::ReaderImpl(ifs);
    }
    else if (2 == verMajor && 0 == verMinor )
    {
        // TODO: LAS 2.0 read/write support
        throw std::runtime_error("LAS 2.0+ file detected but unsupported");
    }

    throw std::runtime_error("LAS file of unknown version");
}


void ReaderFactory::Destroy(Reader* p) 
{
    delete p;
    p = 0;
}


}} // namespace liblas::detail
