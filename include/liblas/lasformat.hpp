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

// std
#include <iosfwd>

namespace liblas {  



class PointFormat
{
public:

    PointFormat( liblas::uint8_t major, 
                    liblas::uint8_t minor, 
                    liblas::uint16_t size);

    PointFormat( liblas::uint8_t major, 
                    liblas::uint8_t minor, 
                    liblas::uint16_t size,
                    bool bColor,
                    bool bTime);
    PointFormat& operator=(PointFormat const& rhs);
    PointFormat(PointFormat const& other);
    PointFormat();
    
    ~PointFormat() {};

    /// Fetch byte size
    uint16_t GetByteSize() const;

    /// Set value of the red image channel 
    void SetByteSize(uint16_t const& value);

    
    uint8_t GetVersionMajor() const; 
    void SetVersionMajor(uint8_t const& value);
    
    uint8_t GetVersionMinor() const;
    void SetVersionMinor(uint8_t const& value);

    bool HasColor() const;
    void Color(bool const& bColor); // updatesize(); }
    bool HasTime() const; 
    void Time(bool const& bTime); // {m_hasTime = bTime; updatesize(); }
  
protected:
    
    liblas::uint16_t m_size;
    liblas::uint8_t m_versionminor;
    liblas::uint8_t m_versionmajor;

    bool m_hasColor;
    bool m_hasTime;    

private:
    void updatesize();
};

inline uint16_t PointFormat::GetByteSize() const
{
    return m_size;
}

inline void PointFormat::SetByteSize(uint16_t const& value)
{
    m_size = value;
}

inline uint8_t PointFormat::GetVersionMajor() const
{
    return m_versionmajor;
}

inline void PointFormat::SetVersionMajor(uint8_t const& value)
{
    m_versionmajor = value;
}

inline uint8_t PointFormat::GetVersionMinor() const
{
    return m_versionminor;
}

inline void PointFormat::SetVersionMinor(uint8_t const& value)
{
    m_versionminor = value;
}

inline bool PointFormat::HasColor() const
{
    return m_hasColor;
}

inline void PointFormat::Color(bool const& value)
{
    m_hasColor = value;
    updatesize();
}

inline bool PointFormat::HasTime() const
{
    return m_hasTime;
}

inline void PointFormat::Time(bool const& value)
{
    m_hasTime = value;
    updatesize();
}

} // namespace liblas

#endif // LIBLAS_FORMAT_HPP_INCLUDED
