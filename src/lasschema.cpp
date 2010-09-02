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

#include <liblas/lasschema.hpp>
#include <liblas/detail/utility.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <algorithm>
#include <sstream>

using namespace boost;

namespace liblas { 


Schema::Schema(PointFormatName data_format_id):
    m_size(0),
    m_data_format_id(data_format_id)
{
    update_required_dimensions(data_format_id);
}

void Schema::add_record0_dimensions()
{
    std::ostringstream text;
    
    DimensionPtr x = DimensionPtr(new DimensionI("X", 32));
    text << "x coordinate as a long integer.  You must use the scale and "
         << "offset information of the header to determine the double value.";
    x->SetDescription(text.str()); 
    x->IsInteger(true);
    x->IsNumeric(true);
    x->IsSigned(true);
    AddDimension(x);
    text.str("");

    DimensionPtr y = DimensionPtr(new DimensionI("Y", 32));
    text << "y coordinate as a long integer.  You must use the scale and "
         << "offset information of the header to determine the double value.";
    y->SetDescription(text.str()); 
    y->IsInteger(true);
    y->IsNumeric(true);
    y->IsSigned(true);
    AddDimension(y);
    text.str("");
    
    DimensionPtr z = DimensionPtr(new DimensionI("Z", 32));
    text << "z coordinate as a long integer.  You must use the scale and "
         << "offset information of the header to determine the double value.";
    z->SetDescription(text.str()); 
    z->IsInteger(true);
    z->IsNumeric(true);
    z->IsSigned(true);
    AddDimension(z);
    text.str("");

    DimensionPtr intensity = DimensionPtr(new DimensionI("Intensity", 16));
    text << "The intensity value is the integer representation of the pulse" 
            "return magnitude. This value is optional and system specific. "
            "However, it should always be included if available.";
    intensity->SetDescription(text.str());
    intensity->IsInteger(true);
    intensity->IsNumeric(true);
    
    AddDimension(intensity);
    text.str("");

    DimensionPtr return_no = DimensionPtr(new DimensionI("Return Number", 3));
    text << "Return Number: The Return Number is the pulse return number for "
            "a given output pulse. A given output laser pulse can have many "
            "returns, and they must be marked in sequence of return. The first "
            "return will have a Return Number of one, the second a Return "
            "Number of two, and so on up to five returns.";
    return_no->SetDescription(text.str());
    return_no->IsNumeric(true);
    return_no->IsInteger(true);
    AddDimension(return_no);
    text.str("");
    
    DimensionPtr no_returns = DimensionPtr(new DimensionI("Number of Returns", 3));
    text << "Number of Returns (for this emitted pulse): The Number of Returns "
            "is the total number of returns for a given pulse. For example, "
            "a laser data point may be return two (Return Number) within a "
            "total number of five returns.";
    no_returns->SetDescription(text.str());
    no_returns->IsNumeric(true);
    no_returns->IsInteger(true);
    AddDimension(no_returns);
    text.str("");
    
    DimensionPtr scan_dir = DimensionPtr(new DimensionI("Scan Direction", 1));
    text << "The Scan Direction Flag denotes the direction at which the "
            "scanner mirror was traveling at the time of the output pulse. "
            "A bit value of 1 is a positive scan direction, and a bit value "
            "of 0 is a negative scan direction (where positive scan direction "
            "is a scan moving from the left side of the in-track direction to "
            "the right side and negative the opposite). ";
    scan_dir->SetDescription(text.str());
    scan_dir->IsNumeric(true);
    scan_dir->IsInteger(true);
    AddDimension(scan_dir);
    text.str("");
    
    DimensionPtr edge = DimensionPtr(new DimensionI("Flightline Edge", 1));
    text << "The Edge of Flight Line data bit has a value of 1 only when "
            "the point is at the end of a scan. It is the last point on "
            "a given scan line before it changes direction.";
    edge->SetDescription(text.str());
    edge->IsNumeric(true);
    edge->IsInteger(true);
    AddDimension(edge);
    text.str("");

    DimensionPtr classification = DimensionPtr(new DimensionI("Classification", 8));
    text << "Classification in LAS 1.0 was essentially user defined and optional. "
            "LAS 1.1 defines a standard set of ASPRS classifications. In addition, "
            "the field is now mandatory. If a point has never been classified, this "
            "byte must be set to zero. There are no user defined classes since "
            "both point format 0 and point format 1 supply 8 bits per point for "
            "user defined operations. Note that the format for classification is a "
            "bit encoded field with the lower five bits used for class and the "
            "three high bits used for flags.";
    classification->SetDescription(text.str());
    AddDimension(classification);
    text.str("");

    
    DimensionPtr scan_angle = DimensionPtr(new DimensionI("Scan Angle Rank", 8));
    text << "The Scan Angle Rank is a signed one-byte number with a "
            "valid range from -90 to +90. The Scan Angle Rank is the "
            "angle (rounded to the nearest integer in the absolute "
            "value sense) at which the laser point was output from the "
            "laser system including the roll of the aircraft. The scan "
            "angle is within 1 degree of accuracy from +90 to –90 degrees. "
            "The scan angle is an angle based on 0 degrees being nadir, "
            "and –90 degrees to the left side of the aircraft in the "
            "direction of flight.";
    scan_angle->SetDescription(text.str());
    scan_angle->IsSigned(true);
    scan_angle->IsInteger(true);
    scan_angle->IsNumeric(true);
    AddDimension(scan_angle);
    text.str("");
    
    DimensionPtr user_data = DimensionPtr(new DimensionI("User Data", 8));
    text << "This field may be used at the user’s discretion";
    user_data->SetDescription(text.str());
    AddDimension(user_data);
    text.str("");
    
    DimensionPtr point_source_id = DimensionPtr(new DimensionI("Point Source ID", 16));
    text << "This value indicates the file from which this point originated. "
            "Valid values for this field are 1 to 65,535 inclusive with zero "
            "being used for a special case discussed below. The numerical value "
            "corresponds to the File Source ID from which this point originated. "
            "Zero is reserved as a convenience to system implementers. A Point "
            "Source ID of zero implies that this point originated in this file. "
            "This implies that processing software should set the Point Source "
            "ID equal to the File Source ID of the file containing this point "
            "at some time during processing. ";
    point_source_id->SetDescription(text.str());
    point_source_id->IsInteger(true);
    point_source_id->IsNumeric(true);
    AddDimension(point_source_id);    
    text.str("");

    std::vector<DimensionPtr>::iterator i;

    for (i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
    {
        boost::shared_ptr<DimensionI> t = *i;
        t->IsRequired(true);
        t->IsActive(true);
    }
    
}

void Schema::add_color()
{
    std::ostringstream text;
    
    DimensionPtr red = DimensionPtr(new DimensionI("Red", 16));
    text << "The red image channel value associated with this point";
    red->SetDescription(text.str()); 
    red->IsRequired(true);
    red->IsActive(true);
    red->IsInteger(true);
    red->IsNumeric(true);
    AddDimension(red);
    text.str("");

    DimensionPtr green = DimensionPtr(new DimensionI("Green", 16));
    text << "The green image channel value associated with this point";
    green->SetDescription(text.str()); 
    green->IsRequired(true);
    green->IsActive(true);
    green->IsInteger(true);
    green->IsNumeric(true);
    AddDimension(green);
    text.str("");

    DimensionPtr blue = DimensionPtr(new DimensionI("Blue", 16));
    text << "The blue image channel value associated with this point";
    blue->SetDescription(text.str()); 
    blue->IsRequired(true);
    blue->IsActive(true);
    blue->IsInteger(true);
    blue->IsNumeric(true);
    AddDimension(blue);
    text.str("");
    
}

void Schema::add_time()
{
    std::ostringstream text;
    
    DimensionPtr t = DimensionPtr(new DimensionI("Time", 64));
    text << "The GPS Time is the double floating point time tag value at "
            "which the point was acquired. It is GPS Week Time if the "
            "Global Encoding low bit is clear and Adjusted Standard GPS "
            "Time if the Global Encoding low bit is set (see Global Encoding "
            "in the Public Header Block description).";
    t->SetDescription(text.str()); 
    t->IsRequired(true);
    t->IsActive(true);
    t->IsNumeric(true);
    AddDimension(t);
    text.str("");
    
}

void Schema::update_required_dimensions(PointFormatName data_format_id)
{
    std::vector<DimensionPtr> user_dims;
    std::vector<DimensionPtr>::const_iterator i;
    
    if (m_dimensions.size() > 0)
    {
        // Keep any non-required dimensions the user may have added
        // and add them back to the list of dimensions
        for (i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
        {
            DimensionPtr t = *i;
            if ( t->IsRequired() == false)
                user_dims.push_back(t);
        }
    }
    
    m_dimensions.clear();
    
    add_record0_dimensions();

    switch (data_format_id) 
    {
        case ePointFormat3:
            add_time();
            add_color();
            break;
        case ePointFormat2:
            add_color();
            break;
        case ePointFormat1:
            add_time();
            break;
        case ePointFormat0:
            break;

        default:
            std::ostringstream oss;
            oss << "Unhandled PointFormatName id " << static_cast<boost::uint32_t>(data_format_id);
            throw std::runtime_error(oss.str());
    }

    // Copy any user-created dimensions that are not 
    // required by the PointFormatName
    for (i = user_dims.begin(); i != user_dims.end(); ++i)
    {
        m_dimensions.push_back(*i);
    }        

}
/// copy constructor
Schema::Schema(Schema const& other) :
    m_size(other.m_size),
    m_data_format_id(other.m_data_format_id),
    m_dimensions(other.m_dimensions)
{

}
// 
// // assignment constructor
Schema& Schema::operator=(Schema const& rhs)
{
    if (&rhs != this)
    {
        m_size = rhs.m_size;
        m_data_format_id = rhs.m_data_format_id;
        m_dimensions = rhs.m_dimensions;
    }
    
    return *this;
}

boost::uint32_t Schema::GetSize() const
{
    std::vector<DimensionPtr>::const_iterator i;
    boost::uint32_t size=0;
    
    for (i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
    {
        size += (*i)->GetSize();
    }

    if (size % 8 != 0) {
        std::ostringstream oss;
        oss << "The summed size in bits, " << size << ", is not a multiple of 8.";
        oss << " The sum of the sizes of all dimensions must be a multiple ";
        oss << " of 8";
        throw std::runtime_error(oss.str());
    }
    return size;
}

void Schema::SetDataFormatId(PointFormatName const& value)
{
    update_required_dimensions(value);
    m_data_format_id = value;
}

void Schema::RemoveDimension(DimensionPtr dim)
{
    std::vector<DimensionPtr>::iterator i;
    
    bool erase = false;
    for (i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
    {
        if ((*i).get() == dim.get()) {
            erase = true;
            break;
        }
    }    
    
    if (erase)
        m_dimensions.erase(i);
}
boost::uint32_t Schema::GetByteSize() const
{
    
    return GetSize() / 8;
}

boost::uint32_t Schema::GetBaseByteSize() const
{
    std::vector<DimensionPtr>::const_iterator i;
    boost::uint32_t size=0;
    boost::uint32_t byte_size = 0;
    
    for (i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
    {
        boost::shared_ptr<DimensionI> t = *i;
        if ( t->IsRequired() == true)
            size += t->GetSize();
    }



    if (size % 8 != 0) {
        std::ostringstream oss;
        oss << "The summed size in bits, " << size << ", is not a multiple of 8.";
        oss << " The sum of the sizes of the required dimensions must be a multiple ";
        oss << " of 8";
        throw std::runtime_error(oss.str());
    }
    
    byte_size = size / 8 ;
    switch (byte_size)
    {
        case ePointSize0:
        case ePointSize1:
        case ePointSize2:
        case ePointSize3:
            break;

        // FIXME: We don't account for extended LAS 1.3 point formats yet.
        default:
            std::ostringstream oss;
            oss << "The sum of the required dimensions, " << byte_size << ", is";
            oss << " not a recognized value.  ";
            throw std::runtime_error(oss.str());
    }
    return byte_size;
}

void Schema::AddDimension(boost::shared_ptr<DimensionI> d)
{
    m_dimensions.push_back(d);
}

DimensionPtr Schema::GetDimension(std::string const& name) const
{
    std::vector<DimensionPtr>::const_iterator i;
    for (i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
    {
        if ((*i)->GetName().compare(name) == 0) {
            return *i;
        }
    }
    std::ostringstream oss;
    oss << "Dimension with name '" << name << "' not found.";
    throw std::runtime_error(oss.str());
}

std::vector<std::string> Schema::GetDimensionNames() const
{
    std::vector<std::string> output;
    std::vector<DimensionPtr>::const_iterator i;
    for (i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
    {
        output.push_back((*i)->GetName());
    }
    return output;
}


bool Schema::HasColor() const
{
    DimensionPtr c;
    try {
        c = GetDimension("Red");
        c = GetDimension("Green");
        c = GetDimension("Blue");
    } catch (std::runtime_error const& e) {
        boost::ignore_unused_variable_warning(e);
        return false;
    }
    
    return true;
    
}

bool Schema::HasTime() const
{
    DimensionPtr c;
    try {
        c = GetDimension("Time");
    } catch (std::runtime_error const& e) {
        boost::ignore_unused_variable_warning(e);
        return false;
    }
    
    return true;
    
}

liblas::property_tree::ptree Schema::GetPTree() const
{
    using liblas::property_tree::ptree;
    ptree pt;
    
    std::vector<DimensionPtr>::const_iterator i;
    
    boost::uint32_t position = 0;
    for(i = m_dimensions.begin(); i != m_dimensions.end(); ++i)
    {
        ptree dim;
        DimensionPtr t = *i;
        dim.put("name", t->GetName());
        dim.put("description", t->GetDescription());
        dim.put("position", position);
        dim.put("active", static_cast<boost::uint32_t>(t->IsActive()));
        dim.put("size", t->GetSize());
        dim.put("integer", static_cast<boost::uint32_t>(t->IsInteger()));
        dim.put("signed", static_cast<boost::uint32_t>(t->IsSigned()));

       if (t->IsNumeric()) {
           if (t->GetMinimum() != t->GetMaximum() && t->GetMaximum() != 0) 
           {
               dim.put("minimum", t->GetMinimum());
               dim.put("maximum", t->GetMaximum());
           }
        }

        pt.add_child("LASSchema.dimension", dim);
        
        position++;
    } 

    return pt;
}

} // namespace liblas
