#ifndef LIBLAS_GUID_HPP
#define LIBLAS_GUID_HPP

// This file has been stolen from <boost/guid.hpp> file
// available at http://www.boost-consulting.com/vault/
// and modified for libLAS purposes.
//
// Copyright 2008 Mateusz Loskot, mateusz@loskot.net.
// Copyright 2006 Andy Tompkins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Revision History
//  06 Feb 2006 - Initial Revision
//  09 Nov 2006 - fixed variant and version bits for v4 guids
//  13 Nov 2006 - added serialization
//  17 Nov 2006 - added name-based guid creation
//  20 Nov 2006 - add fixes for gcc (from Tim Blechmann)
//  07 Mar 2007 - converted to header only
//  20 Jan 2008 - removed dependency of Boost and modified for libLAS (by Mateusz Loskot)

#include <liblas/cstdint.hpp>
#include <liblas/detail/sha1.hpp>
#include <iosfwd>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cassert>

namespace liblas { namespace detail {

inline liblas::uint8_t random_byte()
{
    using liblas::uint8_t;

    // Requires pseudo-random numbers generator to be initialized
    // in create_random_based() function - a poor man solution.
    uint8_t const rmin = std::numeric_limits<uint8_t>::min();
    uint8_t const rmax = std::numeric_limits<uint8_t>::max();
    uint32_t const rnd = std::rand() % rmax + rmin;

    assert(rnd <= 255);
    return static_cast<uint8_t>(rnd);
}

} // namespace detail

class guid
{
public:

    guid() /* throw() */
    {
        std::fill(data_, data_ + static_size, 0);
    }

    explicit guid(char const* const str)
    {
        if (0 == str)
            throw_invalid_argument();
        construct(std::string(str));
    }

    explicit guid(wchar_t const* const str)
    {
        if (0 == str)
            throw_invalid_argument();
        construct(std::wstring(str));
    }

    template <typename ch, typename char_traits, typename alloc>
    explicit guid(std::basic_string<ch, char_traits, alloc> const& str)
    {
        construct(str);
    }

    guid(liblas::uint32_t const& d1, liblas::uint16_t const& d2, liblas::uint16_t const& d3, liblas::uint8_t const (&d4)[8])
    {
        construct(d1, d2, d3, d4);
    }

    guid(guid const& rhs) /* throw() */
    {
        std::copy(rhs.data_, rhs.data_ + static_size, data_);
    }

    ~guid() /* throw() */
    {}

    guid& operator=(guid const& rhs) /* throw() */
    {
        if (&rhs != this)
        {
            std::copy(rhs.data_, rhs.data_ + static_size, data_);
        }
        return *this;
    }

    bool operator==(guid const& rhs) const /* throw() */
    {
        return std::equal(data_, data_ + static_size, rhs.data_);
    }

    bool operator!=(guid const& rhs) const /* throw() */
    {
        return (!(*this == rhs));
    }

    bool operator<(guid const& rhs) const /* throw() */
    {
        return std::lexicographical_compare(data_, data_ + static_size, rhs.data_, rhs.data_ + static_size);
    }
    
    bool operator>(guid const& rhs) const /* throw() */
    {
        return std::lexicographical_compare(rhs.data_, rhs.data_ + static_size, data_, data_ + static_size);
    }

    bool operator<=(guid const& rhs) const /* throw() */
    {
        return (*this == rhs) || (*this < rhs);
    }

    bool operator>=(guid const& rhs) const /* throw() */
    {
        return (*this == rhs) || (*this > rhs);
    }

    bool is_null() const /* throw() */
    {
        return ((*this) == null());
    }

    std::string to_string() const
    {
        return to_basic_string<std::string::value_type, std::string::traits_type, std::string::allocator_type>();
    }

    std::wstring to_wstring() const
    {
       return to_basic_string<std::wstring::value_type, std::wstring::traits_type, std::wstring::allocator_type>();
    }
    
    template <typename ch, typename char_traits, typename alloc>
    std::basic_string<ch, char_traits, alloc> to_basic_string() const
    {
        std::basic_string<ch, char_traits, alloc> s;
        std::basic_stringstream<ch, char_traits, alloc> ss;
        if (!(ss << *this) || !(ss >> s))
        {
            throw std::runtime_error("failed to convert guid to string");
        }
        return s;
    }

    size_t byte_count() const /* throw() */
    {
        return static_size;
    }

    template <typename ByteOutputIterator>
    void output_bytes(ByteOutputIterator out) const
    {
        std::copy(data_, data_ + static_size, out);
    }

    void output_data(liblas::uint32_t& d1, liblas::uint16_t& d2, liblas::uint16_t& d3, liblas::uint8_t (&d4)[8]) const
    {
        std::stringstream ss;

        ss << data_[0] << data_[1] << data_[2] << data_[3];
        ss >> std::hex >> d1;

        ss << data_[4] << data_[5];
        ss >> std::hex >> d2;

        ss << data_[6] << data_[7];
        ss >> std::hex >> d3;

        for (std::size_t i = 0; i < sizeof(d4); ++i)
        {
            d4[i] = data_[i + 8];
        }
    }

    static guid const& null() /* throw() */
    {
        static const guid n;
        return n;
    }

    static guid create()
    {
        return create_random_based();
    }
    
    static guid create(guid const& namespace_guid, char const* name, int name_length)
    {
        return create_name_based(namespace_guid, name, name_length);
    }

    static inline bool get_showbraces(std::ios_base & iosbase)
    {
        return (iosbase.iword(get_showbraces_index()) != 0);
    }
    static inline void set_showbraces(std::ios_base & iosbase, bool showbraces)
    {
        iosbase.iword(get_showbraces_index()) = showbraces;
    }

    static inline std::ios_base& showbraces(std::ios_base& iosbase)
    {
        set_showbraces(iosbase, true);
        return iosbase;
    }
    static inline std::ios_base& noshowbraces(std::ios_base& iosbase)
    {
        set_showbraces(iosbase, false);
        return iosbase;
    }
    
    template <typename ch, typename char_traits> 
    friend std::basic_ostream<ch, char_traits>& operator<<(std::basic_ostream<ch, char_traits> &os, guid const& g);

    template <typename ch, typename char_traits> 
    friend std::basic_istream<ch, char_traits>& operator>>(std::basic_istream<ch, char_traits> &is, guid &g);

private:

    void throw_invalid_argument() const
    {
        throw std::invalid_argument("invalid guid string");
    }

    template <typename ch, typename char_traits, typename alloc>
    void construct(std::basic_string<ch, char_traits, alloc> const& str)
    {
        std::basic_stringstream <ch, char_traits, alloc > ss;
        if (!(ss << str) || !(ss >> *this))
        {
            throw_invalid_argument();
        }
    }

    void construct(liblas::uint32_t const& d1, liblas::uint16_t const& d2, liblas::uint16_t const& d3, liblas::uint8_t const (&d4)[8])
    {
        std::ostringstream ss;
        ss.flags(std::ios::hex);        
        ss.fill('0');

        ss.width(8);
        ss << d1;
        ss << '-';

        ss.width(4);
        ss << d2;
        ss << '-';

        ss.width(4);
        ss << d3;
        ss << '-';

        for (std::size_t i = 0; i < sizeof(d4); ++i)
        {
            ss.width(2);
            ss << static_cast<liblas::uint32_t>(d4[i]);
            if (1 == i)
                ss << '-';
        }

        construct(ss.str());
    }

    //random number based
    static guid create_random_based()
    {
        guid result;
        static bool init_rand = true;
        if (init_rand)
        {
            std::srand(static_cast<unsigned int>(std::time(0)));
            init_rand = false;
        }
        
        for (size_t i = 0; i < static_size; i++)
        {
            result.data_[i] = detail::random_byte();
        }
    
        // set variant
        // should be 0b10xxxxxx
        result.data_[8] &= 0xBF;
        result.data_[8] |= 0x80;
        
       // set version
        // should be 0b0100xxxx
        result.data_[6] &= 0x4F; //0b01001111
        result.data_[6] |= 0x40; //0b01000000
    
        return result;
    }
    
    // name based
    static guid create_name_based(guid const& namespace_guid, char const* name, int name_length)
    {
        using liblas::uint8_t;
        
        detail::SHA1 sha1;
        sha1.Input(namespace_guid.data_, namespace_guid.static_size);
        sha1.Input(name, name_length);
        unsigned int digest[5];
        
        if (sha1.Result(digest) == false)
        {
            throw std::runtime_error("create error");
        }
        
        guid result;
        for (int i = 0; i < 4; ++i)
        {
            
            result.data_[i*4+0] = static_cast<uint8_t>((digest[i] >> 24) & 0xFF);
            result.data_[i*4+1] = static_cast<uint8_t>((digest[i] >> 16) & 0xFF);
            result.data_[i*4+2] = static_cast<uint8_t>((digest[i] >> 8) & 0xFF);
            result.data_[i*4+3] = static_cast<uint8_t>((digest[i] >> 0) & 0xFF);
        }
        
        // set variant
        // should be 0b10xxxxxx
        result.data_[8] &= 0xBF;
        result.data_[8] |= 0x80;
        
        // set version
        // should be 0b0101xxxx
        result.data_[6] &= 0x5F; //0b01011111
        result.data_[6] |= 0x50; //0b01010000
    
        return result;
    }

    static int get_showbraces_index()
    {
        static int index = std::ios_base::xalloc();
        return index;
    }
    
private:

    static const std::size_t static_size = 16;
    liblas::uint8_t data_[static_size];
};

template <typename ch, typename char_traits>
std::basic_ostream<ch, char_traits>& operator<<(std::basic_ostream<ch, char_traits> &os, guid const& g)
{
    using namespace std;

    // TODO: If optional support of Boost is added,
    // use Boost I/O State Savers for safe RAII.
    std::ios_base::fmtflags flags_saver(os.flags());
    std::streamsize width_saver(os.width());
    ch fill_saver(os.fill());

    const typename basic_ostream<ch, char_traits>::sentry ok(os);
    if (ok)
    {
        bool showbraces = guid::get_showbraces(os);
        if (showbraces)
        {
            os << '{';
        }
        os << hex;
        os.fill('0');
        for (size_t i = 0; i < g.static_size; ++i)
        {
            os.width(2);
            os << static_cast<unsigned int>(g.data_[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9)
            {
                os << '-';
            }
        }
        if (showbraces)
        {
            os << '}';
        }
    }

    os.flags(flags_saver);
    os.width(width_saver);
    os.fill(fill_saver);

    return os;
}

template <typename ch, typename char_traits>
std::basic_istream<ch, char_traits>& operator>>(std::basic_istream<ch, char_traits> &is, guid &g)
{
    using namespace std;

    guid temp_guid;

    const typename basic_istream<ch, char_traits>::sentry ok(is);
    if (ok)
    {
        ch c;
        c = static_cast<ch>(is.peek());
        bool bHaveBraces = false;
        if (c == '{')
        {
            bHaveBraces = true;
            is >> c; // read brace
        }

        for (size_t i = 0; i < temp_guid.static_size && is; ++i)
        {
            std::basic_stringstream<ch, char_traits> ss;

            // read 2 characters into stringstream
            is >> c; ss << c;
            is >> c; ss << c;

            // extract 2 characters from stringstream as a hex number
            unsigned int val = 0;
            ss >> hex >> val;
            if (!ss)
            {
                is.setstate(ios_base::failbit);
            }

            // check that val is within valid range
            if (val > 255)
            {
                is.setstate(ios_base::badbit);
            }

            temp_guid.data_[i] = static_cast<liblas::uint8_t>(val);

            if (is)
            {
                if (i == 3 || i == 5 || i == 7 || i == 9)
                {
                    is >> c;
                    if (c != '-')
                        is.setstate(ios_base::failbit);
                }
            }
        }

        if (bHaveBraces && is)
        {
            is >> c;
            if (c != '}')
                is.setstate(ios_base::failbit);
        }
        
        if (is)
        {
            g = temp_guid;
        }
    }

    return is;
}

} //namespace liblas

#endif // LIBLAS_GUID_HPP
