/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS PointFormat implementation for C++ libLAS 
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

#include <liblas/lasformat.hpp>
#include <liblas/detail/utility.hpp>

namespace liblas { 


void PointFormat::updatesize() {
    m_size = sizeof(detail::PointRecord);
    
    if (HasColor()) {
        m_size += 3 * sizeof(liblas::uint16_t);
    }
    
    if (HasTime()) {
        m_size += sizeof(double);
    }
}

PointFormat::PointFormat( liblas::uint8_t major, 
                liblas::uint8_t minor, 
                liblas::uint32_t size) :
    m_size(size),
    m_versionminor(minor), 
    m_versionmajor(major),
    m_hasColor(false),
    m_hasTime(false)
{
    updatesize();
}


PointFormat::PointFormat( liblas::uint8_t major, 
                liblas::uint8_t minor, 
                liblas::uint32_t size,
                bool bColor,
                bool bTime) :
    m_size(size),
    m_versionminor(minor), 
    m_versionmajor(major),
    m_hasColor(bColor),
    m_hasTime(bTime)
{
    updatesize();
}

// copy constructor
PointFormat::PointFormat(PointFormat const& other) :
    m_size(other.m_size),
    m_versionminor(other.m_versionminor),
    m_versionmajor(other.m_versionmajor),
    m_hasColor(other.m_hasColor),
    m_hasTime(other.m_hasTime)
{
    updatesize();
}

// assignment constructor
PointFormat& PointFormat::operator=(PointFormat const& rhs)
{
    if (&rhs != this)
    {
        m_size = rhs.m_size;
        m_versionminor = rhs.m_versionminor;
        m_versionmajor = rhs.m_versionmajor;
        m_hasColor = rhs.m_hasColor;
        m_hasTime = rhs.m_hasTime;

    }
    return *this;
}


} // namespace liblas
