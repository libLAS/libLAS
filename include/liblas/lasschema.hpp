/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS Schema implementation for C++ libLAS 
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

#ifndef LIBLAS_SCHEMA_HPP_INCLUDED
#define LIBLAS_SCHEMA_HPP_INCLUDED

#include <liblas/lasversion.hpp>

// boost
#include <boost/cstdint.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

// std
#include <iosfwd>
#include <limits>
#include <string>
#include <vector>

namespace liblas {  

// class Schema
// {
// public:
// 
//     Schema(boost::uint8_t major, boost::uint8_t minor, boost::uint16_t size);
//     Schema(boost::uint8_t major, boost::uint8_t minor, boost::uint16_t size, bool bColor, bool bTime);
// 
//     Schema& operator=(Schema const& rhs);
//     Schema(Schema const& other);
//     
//     ~Schema() {};
// 
//     /// Fetch byte size
//     boost::uint16_t GetByteSize() const;
// 
//     /// Set the total byte size of the point record
//     void SetByteSize(boost::uint16_t const& value);
// 
//     /// Get the base size (only accounting for Time, Color, etc )
//     /// This is equivalent to the point format's base byte size
//     boost::uint16_t GetBaseByteSize() const;
//     
//     boost::uint8_t GetVersionMajor() const; 
//     void SetVersionMajor(boost::uint8_t const& value);
//     
//     boost::uint8_t GetVersionMinor() const;
//     void SetVersionMinor(boost::uint8_t const& value);
// 
//     bool HasColor() const;
//     void Color(bool const& bColor); // updatesize(); }
//     bool HasTime() const; 
//     void Time(bool const& bTime); // {m_hasTime = bTime; updatesize(); }
//   
// protected:
//     
//     boost::uint16_t m_size;
//     boost::uint8_t m_versionminor;
//     boost::uint8_t m_versionmajor;
// 
//     bool m_hasColor;
//     bool m_hasTime;
//     
//     boost::uint16_t m_base_size;
// 
// private:
//     void updatesize();
//     void updatesize(boost::uint16_t new_size);
//     boost::uint16_t calculate_base_size();
// };

class DimensionI;
typedef boost::shared_ptr<DimensionI> DimensionPtr;

class Schema
{
public:
    
    // Schema();
    Schema(PointFormatName data_format_id);
    Schema& operator=(Schema const& rhs);
    Schema(Schema const& other);

    
    ~Schema() {};

    /// Fetch byte size
    boost::uint32_t GetByteSize() const;

    boost::uint32_t GetSize() const;

    /// Get the base size (only accounting for Time, Color, etc )
    boost::uint32_t GetBaseByteSize() const;


    PointFormatName GetDataFormatId() const { return m_data_format_id; }
    void SetDataFormatId(PointFormatName const& value);//{ m_data_format_id = value; }
    
    bool HasColor() const;
    // void Color(bool const& bColor); 
    bool HasTime() const; 
    // void Time(bool const& bTime);
    
    void AddDimension(boost::shared_ptr<DimensionI> dim);
    boost::shared_ptr<DimensionI> GetDimension(std::string const& name) const;
    void RemoveDimension(DimensionPtr dim);
    
    std::vector<std::string> GetDimensionNames() const;
  
protected:
    
    boost::uint16_t m_size;
    PointFormatName m_data_format_id;

private:


    
    std::vector<DimensionPtr> m_dimensions;    
    
    
    void add_record0_dimensions();
    void add_time();
    void add_color();
    void update_required_dimensions(PointFormatName data_format_id);
};


class DimensionI
{
public:
    DimensionI(std::string const& name, boost::uint32_t size_in_bits) : 
        m_name(name), 
        m_bitsize(size_in_bits) 
    {};
    
    virtual ~DimensionI() {};
        
    std::string const& GetName() { return m_name; }
    
    /// bits, logical size of point record
    std::size_t GetSize() const 
    {
        return m_bitsize;
    }
    
    /// bytes, physical/serialisation size of record
    std::size_t GetByteSize() const 
    {
        return m_bitsize / 8;
    }    
    
    /// Is this dimension required by PointFormatName
    bool IsRequired() const { return m_required; }
    void IsRequired(bool v) { m_required = v; }

    bool IsActive() const { return m_active; }
    void IsActive(bool v) { m_active = v; }

    std::string GetDescription() const { return m_description; }
    void SetDescription(std::string const& v) { m_description = v; }


    
private:
        
    std::string m_name;
    boost::uint32_t m_bitsize;
    bool m_required;
    bool m_active;
    std::string m_description;
};

template <typename T>
class NumericDimension : public DimensionI
{
public:

    NumericDimension(std::string const& name, 
                     T type, 
                     boost::uint32_t size_in_bits ) : 
        DimensionI(name, size_in_bits),
        m_min(std::numeric_limits<T>::min()),
        m_max(std::numeric_limits<T>::max()),
        m_type(type)
    {
        
    };

    NumericDimension& operator=(NumericDimension const& rhs);
    NumericDimension(DimensionI const& other);
    
    ~NumericDimension() {};

    
    T const& GetMin() { return m_min; }
    void SetMax(T const& max) { m_max = max; }
    
    T const& GetMax() { return m_max; }
    void SetMin(T const& min) { m_min = min; }


 

private:

    T m_min;
    T m_max;

    
    T m_type;
};


class ByteDimension : public DimensionI
{
public:

    ByteDimension(std::string const& name, 
                     boost::uint32_t size_in_bits ) : 
        DimensionI(name, size_in_bits)
    {
        
    };

    ByteDimension& operator=(ByteDimension const& rhs);
    ByteDimension(DimensionI const& other);
    
};    

} // namespace liblas

#endif // LIBLAS_SCHEMA_HPP_INCLUDED
