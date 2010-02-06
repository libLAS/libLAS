/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Point Writer implementation for C++ libLAS 
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Howard Butler
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
 
#ifndef LIBLAS_DETAIL_WRITER_POINT_HPP_INCLUDED
#define LIBLAS_DETAIL_WRITER_POINT_HPP_INCLUDED

#include <liblas/lasspatialreference.hpp>
#include <liblas/detail/fwd.hpp>

#include <liblas/laspoint.hpp>

#include <liblas/detail/fwd.hpp>
#include <liblas/detail/utility.hpp>

#include <liblas/lasheader.hpp>
#include <liblas/detail/writer/writer.hpp>

#ifndef HAVE_GDAL
    typedef struct OGRCoordinateTransformationHS *OGRCoordinateTransformationH;
    typedef struct OGRSpatialReferenceHS *OGRSpatialReferenceH;
#endif

// std
#include <iosfwd>

namespace liblas { namespace detail { namespace writer {

class Point : public WriterCan
{
public:
    
    typedef WriterCan Base;
    
    Point(std::ostream& ofs, liblas::uint32_t& count, LASHeader const& header);
    Point(  std::ostream& ofs, 
            liblas::uint32_t& count, 
            LASHeader const& header,
            OGRCoordinateTransformationH transform);
    virtual ~Point();

    const LASPoint& GetPoint() const { return m_point; }
    void write( const LASPoint& );
    
protected:

    typedef std::ostream::off_type off_type;
    typedef std::ostream::pos_type pos_type;
        
    
private:

    // Blocked copying operations, declared but not defined.
    Point(Point const& other);
    Point& operator=(Point const& rhs);
    
    

    std::ostream& m_ofs;
    const LASHeader& m_header;
    LASPoint m_point;
    OGRCoordinateTransformationH m_transform;
    
    PointRecord m_record;
    
    void project();
    void setup();
    void fill();
};


}}} // namespace liblas::detail::writer

#endif // LIBLAS_DETAIL_WRITER_POINT_HPP_INCLUDED
