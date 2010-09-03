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
// boost
#include <boost/cstdint.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

// std
#include <iosfwd>
#include <limits>
#include <string>
#include <vector>

namespace liblas {  

class Dimension;
typedef boost::shared_ptr<Dimension> DimensionPtr;

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
    boost::uint32_t GetByteSize() const;

    boost::uint32_t GetSize() const;

    /// Get the base size (only accounting for Time, Color, etc )
    boost::uint32_t GetBaseByteSize() const;


    PointFormatName GetDataFormatId() const { return m_data_format_id; }
    void SetDataFormatId(PointFormatName const& value);
    
    bool HasColor() const;
    bool HasTime() const; 
    
    void AddDimension(boost::shared_ptr<Dimension> dim);
    boost::shared_ptr<Dimension> GetDimension(std::string const& name) const;
    void RemoveDimension(DimensionPtr dim);
    
    std::vector<std::string> GetDimensionNames() const;
  
    liblas::property_tree::ptree GetPTree() const;
    
    bool IsCustom() const;
    VariableRecord GetVLR() const;

protected:
    
    boost::uint16_t m_size;
    PointFormatName m_data_format_id;
    boost::uint32_t m_nextpos;
private:

    std::vector<DimensionPtr> m_dimensions;    
    
    void add_record0_dimensions();
    void add_time();
    void add_color();
    void update_required_dimensions(PointFormatName data_format_id);
    bool IsSchemaVLR(VariableRecord const& vlr);
    liblas::property_tree::ptree LoadPTree(VariableRecord const& v);
    std::vector<DimensionPtr> LoadDimensions(liblas::property_tree::ptree tree);

};


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
    {};
    
    virtual ~Dimension() {};
        
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
};


} // namespace liblas

#endif // LIBLAS_SCHEMA_HPP_INCLUDED
