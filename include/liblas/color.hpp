/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS color class 
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2008, Howard Butler
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must rede following disclaimer.
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

#ifndef LIBLAS_LASCOLOR_HPP_INCLUDED
#define LIBLAS_LASCOLOR_HPP_INCLUDED

#include <liblas/export.hpp>
#include <stdint.h>

// boost
#include <boost/array.hpp>
// std
#include <cstdlib> // std::size_t

namespace liblas {

/// RGB color container
class LAS_DLL Color
{
public:

    typedef uint16_t value_type;

    /// Default constructor.
    /// Initializes with black color using RGB {0, 0, 0}.
    Color();

    /// User-defined constructor.
    /// Initializes object with given RGB values.
    /// The color component values must be normalized to range from 0 to 65535.
    /// \exception std::invalid_argument if color component value is out of range of unsigned 16-bit integer.
    Color(uint32_t red, uint32_t green, uint32_t blue);

    /// User-defined constructor.
    /// Initializes color components based on values of 3-element array.
    /// The color component values must be normalized to range from 0 to 65535.
    /// \exception std::invalid_argument if color component value is out of range of unsigned 16-bit integer.
    Color(boost::array<value_type, 3> const& color);

    /// Copy constructor.
    Color(Color const& other);

    /// Assignment operator.
    Color& operator=(Color const& rhs);

    /// Fetch value of the red image channel 
    value_type GetRed() const;

    /// Set value of the red image channel 
    void SetRed(value_type const& value);

    /// Fetch value of the blue image channel 
    value_type GetBlue() const;

    /// Set value of the blue image channel 
    void SetBlue(value_type const& value);

    /// Fetch value of the green image channel 
    value_type GetGreen() const;

    /// Set value of the red image channel 
    void SetGreen(value_type const& value);

    /// Index operator providing access to RGB values.
    /// Valid index values are 0, 1 or 2.
    /// \exception std::out_of_range if requested index is out of range (> 2).
    value_type& operator[](std::size_t const& index);

    /// Const version of index operator providing access to RGB values.
    /// Valid index values are 0, 1 or 2.
    /// \exception std::out_of_range if requested index is out of range (> 2).
    value_type const& operator[](std::size_t const& index) const;

private:

    typedef boost::array<value_type, 3> base_type;
    base_type m_color;

    void throw_index_out_of_range() const;
    void throw_invalid_color_component() const;
};

inline Color::value_type Color::GetRed() const
{
    return m_color[0];
}

inline void Color::SetRed(Color::value_type const& value)
{
    m_color[0] = value;
}

inline Color::value_type Color::GetGreen() const
{
    return m_color[1];
}

inline void Color::SetGreen(Color::value_type const& value)
{
    m_color[1] = value;
}

inline Color::value_type Color::GetBlue() const
{
    return m_color[2];
}

inline void Color::SetBlue(Color::value_type const& value)
{
    m_color[2] = value;
}

inline Color::value_type& Color::operator[](std::size_t const& index)
{
    return m_color[index];
}

inline Color::value_type const& Color::operator[](std::size_t const& index) const
{
    return m_color[index];
}

inline bool operator==(Color const& lhs, Color const& rhs)
{
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

inline bool operator!=(Color const& lhs, Color const& rhs)
{
    return !(lhs == rhs);
}

} // namespace liblas

#endif // LIBLAS_LASCOLOR_HPP_INCLUDED
