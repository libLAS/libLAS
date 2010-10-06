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
#include <liblas/external/property_tree/ptree.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/lasversion.hpp>
// boost
#include <boost/cstdint.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
// std
#include <iosfwd>
#include <limits>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/unordered_map.hpp>

namespace liblas {  

class Dimension;

typedef boost::shared_ptr<Dimension> DimensionPtr;
typedef boost::unordered_map<std::string, DimensionPtr> DimensionMap;
typedef std::vector<DimensionPtr> DimensionArray;
typedef boost::array<std::size_t, 4> SizesArray;
typedef boost::unordered_map<std::string, SizesArray> SizesMap;

/// Schema definition
class Schema
{
public:
    
    // Schema();
    Schema(PointFormatName data_format_id);
    Schema(std::vector<VariableRecord> const& vlrs);
    Schema& operator=(Schema const& rhs);
    Schema(Schema const& other);
    
    ~Schema() {};

    /// Fetch byte size
    std::size_t GetByteSize() const;

    std::size_t GetBitSize() const;
    void CalculateSizes();

    /// Get the base size (only accounting for Time, Color, etc )
    std::size_t GetBaseByteSize() const;

    PointFormatName GetDataFormatId() const { return m_data_format_id; }
    void SetDataFormatId(PointFormatName const& value);
    
    void AddDimension(DimensionPtr dim);
    DimensionPtr GetDimension(std::string const& name) const;
    // DimensionPtr GetDimension(std::size_t index) const;
    void RemoveDimension(DimensionPtr dim);
    
    std::vector<std::string> GetDimensionNames() const;
    DimensionMap const& GetDimensions() const { return m_dimensions; }
    liblas::property_tree::ptree GetPTree() const;
    SizesArray const& GetSizes(std::string const& name) const;
    
    boost::uint16_t GetSchemaVersion() const { return m_schemaversion; }
    void SetSchemaVersion(boost::uint16_t v) { m_schemaversion = v; }
    
    bool IsCustom() const;
    VariableRecord GetVLR() const;

protected:
    
    PointFormatName m_data_format_id;
    boost::uint32_t m_nextpos;
    std::size_t m_bit_size;
    std::size_t m_base_bit_size;
    boost::uint16_t m_schemaversion;
    SizesMap m_sizes;
    
private:

    DimensionMap m_dimensions;
    
    void add_record0_dimensions();
    void add_time();
    void add_color();
    void update_required_dimensions(PointFormatName data_format_id);
    bool IsSchemaVLR(VariableRecord const& vlr);
    liblas::property_tree::ptree LoadPTree(VariableRecord const& v);
    DimensionMap LoadDimensions(liblas::property_tree::ptree tree);

};

/// Dimension definition
class Dimension
{
public:
    Dimension(std::string const& name, boost::uint32_t size_in_bits) : 
        m_name(name), 
        m_bitsize(size_in_bits),
        m_required(false),
        m_active(false),
        m_description(std::string("")),
        m_min(0),
        m_max(0),
        m_numeric(false),
        m_signed(false),
        m_integer(false),
        m_position(0)
    {
         if (size_in_bits == 0) {
            std::ostringstream oss;
            oss << "The bit size of the dimension is 0, the dimension is invalid.";
            throw std::runtime_error(oss.str());
        }
    };
    
    virtual ~Dimension() {};
        
    std::string const& GetName() { return m_name; }
    
    /// bits, logical size of point record
    std::size_t GetBitSize() const 
    {
        return m_bitsize;
    }
    
    /// bytes, physical/serialisation size of record
    std::size_t GetByteSize() const 
    {

        std::size_t bit_position = m_bitsize % 8;
        if (bit_position > 0) {
            // For dimensions that are not byte aligned,
            // we need to determine how many bytes they 
            // will take.  We have to read at least one byte if the 
            // size in bits is less than 8.  If it is more than 8, 
            // we need to read the number of bytes it takes + 1 extra.
            if (m_bitsize > 8) {
                return m_bitsize/8 + 1;
            } else {
                return 1;
            }
        }
        return m_bitsize / 8;
    }

    //             /// This stuff needs to be put in dim-GetBytesize
    //         if (m_bitsize > 8) {
    //             return = m_bitsize / 8 + 1;
    //         } else {
    //             byte_size = 1; 
    // }
    //             
    //     // if (m_bitsize % 8 != 0) {
    //     //     std::ostringstream oss;
    //     //     oss << m_name << "'s bit size, " << m_bitsize 
    //     //         << ", is not a multiple of 8 and " 
    //     //         << "cannot be expressed as a single byte value";
    //     //     throw std::range_error(oss.str());
    //     // }
    //     // return m_bitsize / 8;
    // }    
    
    /// Is this dimension required by PointFormatName
    bool IsRequired() const { return m_required; }
    void IsRequired(bool v) { m_required = v; }

    /// Is this dimension being used.  A dimension with 
    /// IsActive false may exist as a placeholder in PointFormatName-specified
    /// dimensions, but have their IsActive flag set to false.  In this 
    /// case, those values may be disregarded.
    bool IsActive() const { return m_active; }
    void IsActive(bool v) { m_active = v; }

    std::string GetDescription() const { return m_description; }
    void SetDescription(std::string const& v) { m_description = v; }

    /// Is this dimension a numeric dimension.  Dimensions with IsNumeric == false
    /// are considered generic bit/byte fields/
    bool IsNumeric() const { return m_numeric ; }
    void IsNumeric(bool v) { m_numeric = v; }

    /// Does this dimension have a sign?  Only applicable to dimensions with 
    /// IsNumeric == true.
    bool IsSigned() const { return m_signed; }
    void IsSigned(bool v) { m_signed = v; }

    /// Does this dimension interpret to an integer?  Only applicable to dimensions 
    /// with IsNumeric == true.
    bool IsInteger() const { return m_integer; }
    void IsInteger(bool v) { m_integer = v; }

    /// The minimum value of this dimension as a double
    double GetMinimum() { return m_min; }
    void SetMinimum(double min) { m_min = min; }
    
    /// The maximum value of this dimension as a double
    double GetMaximum() { return m_max; }
    void SetMaximum(double max) { m_max = max; }
    
    boost::uint32_t GetPosition() const { return m_position; }
    void SetPosition(boost::uint32_t v) { m_position = v; }
    
    double GetScale() const { return m_scale; }
    void SetScale(double v) { m_scale = v; }
    
    double GetOffset() const { return m_offset; }
    void SetOffset(double v) { m_offset = v; }
    
    bool IsFinitePrecision() const { return m_precise; }
    void IsFinitePrecision(bool v) { m_precise = v; }
    
    bool operator < (Dimension const& dim) const 
    {
        return m_position < dim.m_position;
    }
private:
        
    std::string m_name;
    boost::uint32_t m_bitsize;
    bool m_required;
    bool m_active;
    std::string m_description;
    double m_min;
    double m_max;
    bool m_numeric;
    bool m_signed;
    bool m_integer;
    boost::uint32_t m_position;
    double m_scale;
    bool m_precise;
    double m_offset;
  
};

bool inline sort_dimensions(DimensionPtr i, DimensionPtr j) 
{ 
    return (*i) < (*j); 
}

std::ostream& operator<<(std::ostream& os, liblas::Schema const&);


} // namespace liblas

#endif // LIBLAS_SCHEMA_HPP_INCLUDED
