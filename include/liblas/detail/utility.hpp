/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  A grab bucket 'o fun for C++ libLAS 
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

//
#ifndef LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
#define LIBLAS_DETAIL_UTILITY_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <iosfwd>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cassert>

namespace liblas { namespace detail {

struct VLRHeader
{
  uint16_t reserved;
  char userId[16]; 
  uint16_t recordId;
  uint16_t recordLengthAfterHeader;
  char description[32];
};

struct GeoKeysHeader
{
    uint16_t keyDirectoryVersion;
    uint16_t keyRevision;
    uint16_t minorRevision;
    uint16_t numberOfKeys;
};

struct GeoKeyEntry
{
  uint16_t keyId;
  uint16_t tiffTagLocation;
  uint16_t count;
  uint16_t valueOffset;
};

class VariableLengthRecord
{
    VariableLengthRecord()
        : reserved(0), record_id(0), record_length_after_header(0)
    {
        std::memset(user_id, 0, eUserIdSize);
        std::memset(description, 0, eDescriptionSize);
    }
 
    enum
    {
        eUserIdSize = 16,
        eDescriptionSize = 32
    };

    uint16_t reserved;
    int8_t user_id[eUserIdSize]; 
    uint16_t record_id;
    uint16_t record_length_after_header;
    int8_t description[eDescriptionSize];
    uint8_t data;
};

struct PointRecord
{
    PointRecord() :
        x(0), y(0), z(0),
        intensity(0),
        flags(0),
        classification(0),
        scan_angle_rank(0),
        user_data(0),
        point_source_id(0)
    {}

    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint16_t intensity;
    uint8_t flags; // TODO: Replace with portable std::bitset<8>
    uint8_t classification;
    int8_t scan_angle_rank;
    uint8_t user_data;
    uint16_t point_source_id;
};

template <typename T>
struct Point
{
    Point() : x(T()), y(T()), z(T()) {}
    Point(T const& x, T const& y, T const& z) : x(x), y(y), z(z) {}
    T x;
    T y;
    T z;

    bool equal(Point<T> const& other) const
    {
        return ((x == other.x) && (y == other.y) && (z == other.z));
    }
};

template <typename T>
bool operator==(Point<T> const& lhs, Point<T> const& rhs)
{
    return lhs.equal(rhs);
}

template <typename T>
bool operator!=(Point<T> const& lhs, Point<T> const& rhs)
{
    return (!lhs.equal(rhs));
}

template <typename T>
struct Extents
{
    typename detail::Point < T > min;
    typename detail::Point < T > max;

    bool equal(Extents<T> const& other) const
    {
        return (min == other.min && max == other.max);
    }
};

template <typename T>
bool operator==(Extents<T> const& lhs, Extents<T> const& rhs)
{
    return lhs.equal(rhs);
}

template <typename T>
bool operator!=(Extents<T> const& lhs, Extents<T> const& rhs)
{
    return (!lhs.equal(rhs));
}

template <typename T>
inline T generate_random_byte()
{
    // Requires pseudo-random numbers generator to be initialized
    // in create_random_based() function - a poor man solution.
    T const rmin = std::numeric_limits<T>::min();
    T const rmax = std::numeric_limits<T>::max();
    unsigned int const rnd = std::rand() % rmax + rmin;

    assert(rnd <= 255);
    return static_cast<T>(rnd);
}

template<typename T>
inline char* as_buffer(T& data)
{
    return static_cast<char*>(static_cast<void*>(&data));
}

template<typename T>
inline char* as_buffer(T* data)
{
    return static_cast<char*>(static_cast<void*>(data));
}

template<typename T>
inline char const* as_bytes(T const& data)
{
    return static_cast<char const*>(static_cast<void const*>(&data));
}

template<typename T>
inline char const* as_bytes(T const* data)
{
    return static_cast<char const*>(static_cast<void const*>(data));
}

template <typename T>
void read_n(T& dest, std::istream& src, std::streamsize const& num)
{
    // TODO: Review and redesign errors handling logic if necessary

    if (!src)
        throw std::runtime_error("detail::liblas::read_n input stream is not readable");

    // Read bytes into buffer
    src.read(detail::as_buffer(dest), num);
    
    // Test stream state bits
    if (src.eof())
        throw std::out_of_range("end of file encountered");
    else if (src.fail())
        throw std::runtime_error("non-fatal I/O error occured");
    else if (src.bad())
        throw std::runtime_error("fatal I/O error occured");

    // Poor man test of data consistency
    std::streamsize const rn = src.gcount();
    if (num != rn)
    {
        std::ostringstream os;
        os << "read only " << rn << " bytes of " << num;
        throw std::runtime_error(os.str());
    }
}

template <typename T>
void write_n(std::ostream& dest, T const& src, std::streamsize const& num)
{
    if (!dest)
        throw std::runtime_error("detail::liblas::write_n: output stream is not writable");


    dest.write(detail::as_bytes(src), num);

    // Test stream state bits
    if (dest.eof())
        throw std::out_of_range("end of file encountered");
    else if (dest.fail())
        throw std::runtime_error("non-fatal I/O error occured");
    else if (dest.bad())
        throw std::runtime_error("fatal I/O error occured");
}

template <typename T> 
bool compare_doubles(const T& actual, const T& expected) 
{ 
const T epsilon = std::numeric_limits<T>::epsilon();  
const T diff = actual - expected; 
 
if ( !((diff <= epsilon) && (diff >= -epsilon )) ) 
{ 
    return false; 
} 

return true;
}

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
