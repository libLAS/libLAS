/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Definition LASClassification type.
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ******************************************************************************
 * Copyright (c) 2009, Mateusz Loskot
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

#ifndef LIBLAS_LASCLASSIFICATION_HPP_INCLUDED
#define LIBLAS_LASCLASSIFICATION_HPP_INCLUDED

#include <liblas/cstdint.hpp>
// std
#include <cassert>
#include <cstddef>
#include <bitset>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace liblas {

/// Class definition to manipulate properties of point record classification.
///
class LASClassification
{
public:

    ///
    typedef std::bitset<8> bitset_type;

    /// Number of classes in lookup table as defined in ASPRS LAS 1.1+.
    /// For LAS 1.0, this static number may be invalid and
    /// extend up to 255 classes stored in variable-length records.
    static std::size_t const class_table_size = 32;

    enum
    {
        eClassBit     = 0, /// First bit position of 0:4 range.
        eSyntheticBit = 5, /// Synthetic flag.
        eKeyPointBit  = 6, /// Key-point flag.
        eWithheldBit  = 7  /// Withheld flag.
    };

    /// Default initialization constructor.
    /// Initializes all flags of classification not set.
    /// Operation semantic is equivalent to bitset_type::reset().
    LASClassification() {}

    /// Initializes classification with given set of 8 flags.
    explicit LASClassification(bitset_type const& flags)
        : m_flags(flags)
    {}

    /// Initializes classification from flags given as integral type.
    explicit LASClassification(uint8_t const& flags)
        : m_flags(flags)
    {}

    /// Initializes classification with values of given compounds.
    LASClassification(uint8_t cls, bool s, bool k, bool w)
    {
        SetClass(cls);
        SetSynthetic(s);
        SetKeyPoint(k);
        SetWithheld(w);
    }

    LASClassification(LASClassification const& other)
    {
        m_flags = other.m_flags;
    }

    LASClassification& operator=(LASClassification const& rhs)
    {
        if (&rhs != this )
        {    
            m_flags = rhs.m_flags;
        }
        return *this;
    }

    /// Conversion operator.
    /// Returns classification object as flags encoded in form of std::bitset<8>.
    operator bitset_type() const
    {
        return bitset_type(m_flags);
    }

    /// Raturns name of class as defined in LAS 1.1+
    /// Finds class name in lookup table based on class index
    /// as defined in classification object.
    ///
    /// \todo TODO: To be implemented
    std::string GetClassName() const
    {
        return std::string("");
    }

    uint8_t GetClass() const
    {
        bitset_type bits(m_flags);
        bitset_type mask(class_table_size - 1);
        bits &= mask;
        return (static_cast<uint8_t>(bits.to_ulong()));
    }

    void SetClass(uint8_t index)
    {
        check_class_index(index);

        bitset_type binval(index);
        binval <<= 0;

        // Store value in bits 0,1,2,3,4
        bitset_type const mask(0x1F);
        m_flags &= ~mask;
        m_flags |= mask & binval;
    }

    void SetSynthetic(bool flag)
    {
        m_flags[eSyntheticBit] = flag;
    }

    bool IsSynthetic() const
    {
        return m_flags[eSyntheticBit];
    }

    void SetKeyPoint(bool flag)
    {
        m_flags[eKeyPointBit] = flag;
    }

    bool IsKeyPoint() const
    {
        return m_flags[eKeyPointBit];
    }

    void SetWithheld(bool flag)
    {
        m_flags[eWithheldBit] = flag;
    }

    bool IsWithheld() const
    {
        return m_flags[eWithheldBit];
    }

    bool equal(LASClassification const& other) const
    {
        return (other.m_flags == m_flags);
    }

private:

    bitset_type m_flags;

    void check_class_index(std::size_t index)
    {
        if (index > (class_table_size - 1))
        {
            std::ostringstream oss;
            oss << "given index is " << index
                << ", but must fit between 0 and " << (class_table_size - 1);
            throw std::out_of_range(oss.str());
        }
    }
};

inline bool operator==(LASClassification const& lhs, LASClassification const& rhs)
{
    return lhs.equal(rhs);
}

inline bool operator!=(LASClassification const& lhs, LASClassification const& rhs)
{
    return (!(lhs == rhs));
}

/// The output stream operator is based on std::bitset<N>::operator<<.
/// It outputs classification flags in form of string.
/// Effects promised as by Standard for Programming Language C++, 23.3.5.2:
/// Each character is determined by the value of its corresponding bit
/// position in *this. Character position N - 1 corresponds to bit position
/// zero. Subsequent decreasing character positions correspond to increasing
/// bit positions. Bit value zero becomes the character 0, bit value one
/// becomes the character 1.
inline std::ostream& operator<<(std::ostream& os, LASClassification const& cls)
{
    LASClassification::bitset_type flags(cls);
    return (os << flags);
}

} // namespace liblas

#endif // LIBLAS_LASCLASSIFICATION_HPP_INCLUDED
