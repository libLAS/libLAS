// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
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

struct VariableLengthRecordHeader
{
    VariableLengthRecordHeader()
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
