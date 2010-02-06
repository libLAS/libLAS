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

#ifndef LIBLAS_DETAIL_WRITER10_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER10_HPP_INCLUDED

#include <liblas/detail/fwd.hpp>
#include <liblas/detail/writer/point.hpp>

#ifndef HAVE_GDAL
    typedef struct OGRCoordinateTransformationHS *OGRCoordinateTransformationH;
    typedef struct OGRSpatialReferenceHS *OGRSpatialReferenceH;
#endif


// std
#include <iosfwd>

namespace liblas { namespace detail { 

// class WriterCan
// {
// public:
// 
//     WriterCan(std::ostream& ofs, liblas::uint32_t& count);
//     ~WriterCan();
//     
//     std::ostream& GetStream() const { return m_ofs; }
//     liblas::uint32_t& GetPointCount() const { return m_pointCount; }
//     void SetPointCount(liblas::uint32_t& count) { m_pointCount = count; }
//     
//     
// private:
// 
//     // Blocked copying operations, declared but not defined.
//     WriterCan(WriterCan const& other);
//     WriterCan& operator=(WriterCan const& rhs);
// 
//     
//     liblas::uint32_t& m_pointCount;
//     std::ostream& m_ofs;
//     
//     
// };



class WriterImpl 
{
public:

    
    WriterImpl(std::ostream& ofs);
    ~WriterImpl();
    LASVersion GetVersion() const;
    void WriteHeader(LASHeader& header);
    void UpdateHeader(LASHeader const& header);
    void WritePointRecord(LASPoint const& record, const LASHeader& header);

    std::ostream& GetStream() const;

    void SetSRS(const LASSpatialReference& srs, const LASHeader& header);
    void SetInputSRS(const LASSpatialReference& srs);
    void SetOutputSRS(const LASSpatialReference& srs, const LASHeader& header);

protected:
    PointRecord m_record;
    std::ostream& m_ofs;

    void FillPointRecord(PointRecord& record, const LASPoint& point, const LASHeader& header);

    void Project(LASPoint& point);      
    LASSpatialReference m_out_srs;
    LASSpatialReference m_in_srs;
    
    OGRCoordinateTransformationH m_transform;
    OGRSpatialReferenceH m_in_ref;
    OGRSpatialReferenceH m_out_ref;
    
    detail::writer::Point* m_point_writer;
    
private:

    void CreateTransform();
    liblas::uint32_t m_pointCount;
};

class WriterFactory
{
public:

    static WriterImpl* Create(std::ostream& ofs, LASHeader const& header);
    static void Destroy(WriterImpl* p);
};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_WRITER10_HPP_INCLUDED
