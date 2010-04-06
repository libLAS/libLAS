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

void PointFormat::updatesize(liblas::uint16_t new_size) {


        long base_difference = new_size - m_size;
        
        // printf("updatesize() %d\n", base_difference);
        if (base_difference == 0) {
            return;
        }
        else if (base_difference > 0) {
            // Expand m_size to include new_base_size
            // printf("we're now here! %d %d %d %d\n", m_size, base_difference, new_size, m_base_size);            

            m_size = new_size;
        }
        else {
            // printf("we're here! %d %d %d\n", m_size, base_difference, m_base_size);            
            m_size = std::max(new_size, m_base_size);
        }
        
}

liblas::uint16_t PointFormat::calculate_base_size() {
    liblas::uint16_t new_base_size = sizeof(detail::PointRecord);
    
    if (HasColor()) {
        new_base_size += 3 * sizeof(liblas::uint16_t);
    }
    
    if (HasTime()) {
        new_base_size += sizeof(double);
    }
    return new_base_size;
    
}
void PointFormat::updatesize() {
    liblas::uint16_t new_base_size = calculate_base_size();
    
    // Set to the base if we haven't set it at all yet
    if (m_size == 0) {
        m_size = new_base_size;
        m_base_size = new_base_size;
    }
    
    // Expand or contract the size based on our old difference
    else {
        
        long base_difference = m_base_size - new_base_size;
        
        if (base_difference == 0) {
            return;
        }
        else if (base_difference > 0) {
            // Expand m_size to include new_base_size
            m_size = m_size + base_difference;
            m_base_size = new_base_size;
        }
        else {
            m_size = m_size - base_difference;
            m_base_size = new_base_size;
        }
        
    }
}

PointFormat::PointFormat( liblas::uint8_t major, 
                liblas::uint8_t minor, 
                liblas::uint16_t size) :
    m_size(size),
    m_versionminor(minor), 
    m_versionmajor(major),
    m_hasColor(false),
    m_hasTime(false),
    m_base_size(0)
{
    updatesize();
}


PointFormat::PointFormat( liblas::uint8_t major, 
                liblas::uint8_t minor, 
                liblas::uint16_t size,
                bool bColor,
                bool bTime) :
    m_size(size),
    m_versionminor(minor), 
    m_versionmajor(major),
    m_hasColor(bColor),
    m_hasTime(bTime),
    m_base_size(0)
{
    updatesize();
}

PointFormat::PointFormat( ) :
    m_size(0),
    m_versionminor(2), 
    m_versionmajor(1),
    m_hasColor(false),
    m_hasTime(false),
    m_base_size(0)
{
    updatesize();
}


// copy constructor
PointFormat::PointFormat(PointFormat const& other) :
    m_size(other.m_size),
    m_versionminor(other.m_versionminor),
    m_versionmajor(other.m_versionmajor),
    m_hasColor(other.m_hasColor),
    m_hasTime(other.m_hasTime),
    m_base_size(other.m_base_size)
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
        m_base_size = rhs.m_base_size;

    }
    return *this;
}


} // namespace liblas
