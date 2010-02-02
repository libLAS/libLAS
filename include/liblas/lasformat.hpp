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

#ifndef LIBLAS_FORMAT_HPP_INCLUDED
#define LIBLAS_FORMAT_HPP_INCLUDED

#include <liblas/detail/fwd.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/lasheader.hpp>
// std
#include <iosfwd>

namespace liblas {  



class LASPointFormat
{
public:

    LASPointFormat( liblas::uint8_t major, 
                    liblas::uint8_t minor, 
                    liblas::uint32_t size);

    LASPointFormat( liblas::uint8_t major, 
                    liblas::uint8_t minor, 
                    liblas::uint32_t size,
                    bool bColor,
                    bool bTime);
    LASPointFormat& operator=(LASPointFormat const& rhs);
    LASPointFormat(LASPointFormat const& other);
    
    liblas::uint16_t GetByteSize() const {return m_size;}
    void SetByteSize(liblas::uint16_t size) {m_size = size;}
    
    liblas::uint8_t GetVersionMajor() { return m_versionmajor; }
    void SetVersionMajor(liblas::uint8_t v) {m_versionmajor = v; }
    
    liblas::uint8_t GetVersionMinor() { return m_versionminor; }
    void SetVersionMinor(liblas::uint8_t v) {m_versionminor = v; }

    bool HasColor() const {return m_hasColor; }
    void Color(bool bColor) {m_hasColor = bColor; }
    bool HasTime() const { return m_hasTime; }
    void Time(bool bTime) {m_hasTime = bTime; }
  
protected:
    
    liblas::uint16_t m_size;
    liblas::uint8_t m_versionminor;
    liblas::uint8_t m_versionmajor;

    bool m_hasColor;
    bool m_hasTime;    
};

} // namespace liblas

#endif // LIBLAS_FORMAT_HPP_INCLUDED
