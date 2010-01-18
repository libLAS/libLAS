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

#include <liblas/detail/format.hpp>
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

Format::Format( liblas::uint8_t major, 
                liblas::uint8_t minor, 
                liblas::LASHeader::PointSize& size) :
    m_size(size),
    m_versionminor(minor), 
    m_versionmajor(major),
    m_compressed(false)
{
    
}

PointFormat::PointFormat( liblas::uint8_t major, 
                liblas::uint8_t minor, 
                liblas::LASHeader::PointSize& size) :
    Base(major, minor, size),
    m_hasColor(false),
    m_hasTime(false)
{
    // PointFormat is assumed to be used for the standard point formats
    // that are specified in the specification.  They have standard byte 
    // sizes of the following:
    //      ePointSize0 = 20 -> point data format 0
    //      ePointSize1 = 28 -> point data format 1
    //      ePointSize2 = 26 -> point data format 2
    //      ePointSize3 = 34 -> point data format 3
    
    // If the size we were given is not one of these values, we are not 
    // going to be able to determine automatically whether or not the record
    // has color or time values.  
    
    if (size == LASHeader::ePointSize0) {
        
    }
}

}} // namespace liblas::detail
