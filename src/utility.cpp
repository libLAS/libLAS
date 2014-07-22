/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS utility classes
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

#include <liblas/utility.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <vector>

using namespace boost;

namespace liblas { 

Summary::Summary() : 
    FilterI(liblas::FilterI::eInclusion),
    synthetic(0),
    withheld(0),
    keypoint(0),
    count(0),
    first(true),
    bHaveHeader(false),
    bHaveColor(true),
    bHaveTime(true)

    
{
    classes.assign(0);
    points_by_return.assign(0);
    returns_of_given_pulse.assign(0);    
}

Summary::Summary(Summary const& other)
    : FilterI(liblas::FilterI::eInclusion)
    , classes(other.classes)
    , synthetic(other.synthetic)
    , withheld(other.withheld)
    , keypoint(other.keypoint)
    , count(other.count)
    , points_by_return(other.points_by_return)
    , returns_of_given_pulse(other.returns_of_given_pulse)
    , first(other.first)
    , minimum(boost::shared_ptr<liblas::Point>(new liblas::Point(*other.minimum)))
    , maximum(boost::shared_ptr<liblas::Point>(new liblas::Point(*other.maximum)))
    , m_header(other.m_header)
    , bHaveHeader(other.bHaveHeader)
    , bHaveColor(other.bHaveColor)
    , bHaveTime(other.bHaveTime)
{
}

Summary& Summary::operator=(Summary const& rhs)
{
    if (&rhs != this)
    {
        classes = rhs.classes;
        synthetic = rhs.synthetic;
        withheld = rhs.withheld;
        keypoint = rhs.keypoint;
        count = rhs.count;
        first = rhs.first;
        points_by_return = rhs.points_by_return;
        returns_of_given_pulse = rhs.returns_of_given_pulse;
        minimum = boost::shared_ptr<liblas::Point>(new liblas::Point(*rhs.minimum));
        maximum =  boost::shared_ptr<liblas::Point>(new liblas::Point(*rhs.maximum));
        m_header = rhs.m_header;
        bHaveHeader = rhs.bHaveHeader;
        bHaveColor = rhs.bHaveColor;
        bHaveTime = rhs.bHaveTime;
    }
    return *this;
}

void Summary::AddPoint(liblas::Point const& p)
{
        count++;

        if (first) {
            
            minimum = boost::shared_ptr<liblas::Point>(new liblas::Point(p));
            maximum = boost::shared_ptr<liblas::Point>(new liblas::Point(p));
            
            // We only summarize the base dimensions 
            // but we want to be able to read/set them all.  The 
            // point copy here would set the header ptr of min/max
            // to be whatever might have come off of the file, 
            // and this may/may not have space for time/color


            if (bHaveHeader)
            {
                maximum.get()->SetHeader(&m_header);
                minimum.get()->SetHeader(&m_header);
            }

            liblas::Header const* h = p.GetHeader();

            m_header.SetScale(h->GetScaleX(), h->GetScaleY(), h->GetScaleZ());
            m_header.SetOffset(h->GetOffsetX(), h->GetOffsetY(), h->GetOffsetZ());


            if (m_header.GetDataFormatId() != h->GetDataFormatId())
                m_header.SetDataFormatId(h->GetDataFormatId());

            liblas::Schema const& schema = h->GetSchema();
            boost::optional<Dimension const&> red;
            boost::optional<Dimension const&> green;
            boost::optional<Dimension const&> blue;
            boost::optional<Dimension const&> time;

            red = schema.GetDimension("Red");
            green = schema.GetDimension("Green");
            blue = schema.GetDimension("Blue");
            if (red && green && blue)
                bHaveColor = true;
            else 
                bHaveColor = false;

            time = schema.GetDimension("Time");
            if (time) bHaveTime = true; else bHaveTime = false;

            first = false;
        }
        
        if (p.GetRawX() < minimum.get()->GetRawX() )
            minimum.get()->SetRawX(p.GetRawX());
        if (p.GetRawX() > maximum.get()->GetRawX() )
            maximum.get()->SetRawX(p.GetRawX());

        if (p.GetRawY() < minimum.get()->GetRawY() )
            minimum.get()->SetRawY(p.GetRawY());
        if (p.GetRawY() > maximum.get()->GetRawY() )
            maximum.get()->SetRawY(p.GetRawY());

        if (p.GetRawZ() < minimum.get()->GetRawZ() )
            minimum.get()->SetRawZ(p.GetRawZ());
        if (p.GetRawZ() > maximum.get()->GetRawZ() )
            maximum.get()->SetRawZ(p.GetRawZ());

        if (p.GetIntensity() < minimum.get()->GetIntensity() )
            minimum.get()->SetIntensity(p.GetIntensity());
        if (p.GetIntensity() > maximum.get()->GetIntensity() )
            maximum.get()->SetIntensity(p.GetIntensity());
        
        if (bHaveTime)
        {
            if (p.GetTime() < minimum.get()->GetTime() )
                minimum.get()->SetTime(p.GetTime());
            if (p.GetTime() > maximum.get()->GetTime() )
                maximum.get()->SetTime(p.GetTime());
        }


        if (p.GetReturnNumber() < minimum.get()->GetReturnNumber() )
            minimum.get()->SetReturnNumber(p.GetReturnNumber());
        if (p.GetReturnNumber() > maximum.get()->GetReturnNumber() )
            maximum.get()->SetReturnNumber(p.GetReturnNumber());

        if (p.GetNumberOfReturns() < minimum.get()->GetNumberOfReturns() )
            minimum.get()->SetNumberOfReturns(p.GetNumberOfReturns());
        if (p.GetNumberOfReturns() > maximum.get()->GetNumberOfReturns() )
            maximum.get()->SetNumberOfReturns(p.GetNumberOfReturns());

        if (p.GetScanDirection() < minimum.get()->GetScanDirection() )
            minimum.get()->SetScanDirection(p.GetScanDirection());
        if (p.GetScanDirection() > maximum.get()->GetScanDirection() )
            maximum.get()->SetScanDirection(p.GetScanDirection());


        if (p.GetFlightLineEdge() < minimum.get()->GetFlightLineEdge() )
            minimum.get()->SetFlightLineEdge(p.GetFlightLineEdge());
        if (p.GetFlightLineEdge() > maximum.get()->GetFlightLineEdge() )
            maximum.get()->SetFlightLineEdge(p.GetFlightLineEdge());

        if (p.GetScanAngleRank() < minimum.get()->GetScanAngleRank() )
            minimum.get()->SetScanAngleRank(p.GetScanAngleRank());
        if (p.GetScanAngleRank() > maximum.get()->GetScanAngleRank() )
            maximum.get()->SetScanAngleRank(p.GetScanAngleRank());

        if (p.GetUserData() < minimum.get()->GetUserData() )
            minimum.get()->SetUserData(p.GetUserData());
        if (p.GetUserData() > maximum.get()->GetUserData() )
            maximum.get()->SetUserData(p.GetUserData());

        if (p.GetPointSourceID() < minimum.get()->GetPointSourceID() )
            minimum.get()->SetPointSourceID(p.GetPointSourceID());
        if (p.GetPointSourceID() > maximum.get()->GetPointSourceID() )
            maximum.get()->SetPointSourceID(p.GetPointSourceID());

     
        liblas::Classification const& cls = p.GetClassification();
        
        uint8_t minc = (std::min)(cls.GetClass(), minimum.get()->GetClassification().GetClass());
        uint8_t maxc = (std::max)(cls.GetClass(), maximum.get()->GetClassification().GetClass());
        
        classes[cls.GetClass()]++;
        
        if (cls.IsWithheld()) withheld++;
        if (cls.IsKeyPoint()) keypoint++;
        if (cls.IsSynthetic()) synthetic++;
        
        if (minc < minimum.get()->GetClassification().GetClass())
            minimum.get()->SetClassification(liblas::Classification(minc));
        if (maxc > maximum.get()->GetClassification().GetClass())
            maximum.get()->SetClassification(liblas::Classification(maxc));
        
        if (bHaveColor)
        {
            liblas::Color const& color = p.GetColor();
            liblas::Color::value_type min_red = minimum.get()->GetColor().GetRed();
            liblas::Color::value_type min_green = minimum.get()->GetColor().GetGreen();
            liblas::Color::value_type min_blue = minimum.get()->GetColor().GetBlue();

            liblas::Color::value_type max_red = maximum.get()->GetColor().GetRed();
            liblas::Color::value_type max_green = maximum.get()->GetColor().GetGreen();
            liblas::Color::value_type max_blue = maximum.get()->GetColor().GetBlue();
                        
            bool bSetMinColor = false;
            if (color.GetRed() < min_red)
            {
                bSetMinColor = true;
                min_red = color.GetRed();
            }
            if (color.GetGreen() < min_green)
            {
                bSetMinColor = true;
                min_green = color.GetGreen();
            }
            if (color.GetBlue() < min_blue)
            {
                bSetMinColor = true;
                min_blue = color.GetBlue();
            }

        
            bool bSetMaxColor = false;
            if (color.GetRed() > max_red)
            {
                bSetMaxColor = true;
                max_red = color.GetRed();
            }

                
            if (color.GetGreen() > max_green)
            {
                bSetMaxColor = true;
                max_green = color.GetGreen();
            }
            if (color.GetBlue() > max_blue)
            {
                bSetMaxColor = true;
                max_blue = color.GetBlue();
            }

            if (bSetMinColor)
                minimum.get()->SetColor(liblas::Color( min_red, 
                                            min_green,
                                            min_blue));
            if (bSetMaxColor)
                maximum.get()->SetColor(liblas::Color( max_red, 
                                            max_green, 
                                            max_blue));
        }
        

        points_by_return[p.GetReturnNumber()]++;
        returns_of_given_pulse[p.GetNumberOfReturns()]++;    
}

void Summary::SetHeader(liblas::Header const& h) 
{
    m_header = h;

    minimum = boost::shared_ptr<liblas::Point>(new liblas::Point(&m_header));
    maximum = boost::shared_ptr<liblas::Point>(new liblas::Point(&m_header));

    bHaveHeader = true;
}

bool Summary::filter(liblas::Point const& p)
{
    AddPoint(p);
    return true;
}

ptree Summary::GetPTree() const
{
    ptree pt;
    
    ptree pmin = minimum.get()->GetPTree();
    ptree pmax = maximum.get()->GetPTree();
    
     
    pt.add_child("minimum", pmin);
    pt.add_child("maximum", pmax);
    
    ptree klasses;
    
    for (classes_type::size_type i=0; i < classes.size(); i++)
    {
        if (classes[i] != 0)
        {
            liblas::Classification c(i, false, false, false);
            std::string const& name = c.GetClassName();

            klasses.put("name", name);
            klasses.put("count", classes[i]);
            klasses.put("id", i);
            pt.add_child("classification.classification",klasses);            
        }
    }
    pt.put("encoding.withheld", withheld);
    pt.put("encoding.keypoint", keypoint);
    pt.put("encoding.synthetic", synthetic);
    
    ptree returns;
    bool have_returns = false;
    for (boost::array<uint32_t,8>::size_type i=0; i < points_by_return.size(); i++) {
        if (i == 0) continue;

        if (points_by_return[i] != 0)
        {
            have_returns = true;
            returns.put("id", i);
            returns.put("count", points_by_return[i]);
            pt.add_child("points_by_return.return", returns);
            
        }
    }
    
    if (! have_returns) {
        // Assume all points are first return
        returns.put("id", 1);
        returns.put("count", count);
        pt.add_child("points_by_return.return", returns);        
    }
    
    ptree pulses;
    for (boost::array<uint32_t,8>::size_type i=0; i < returns_of_given_pulse.size(); i++) {
        if (returns_of_given_pulse[i] != 0) {
            pulses.put("id",i);
            pulses.put("count", returns_of_given_pulse[i]);
            pt.add_child("returns_of_given_pulse.pulse", pulses);
        }
    }
    
    pt.put("count", count);

    liblas::property_tree::ptree top;
    if (bHaveHeader)
        top.add_child("summary.header",m_header.GetPTree());
    top.add_child("summary.points",pt);
    return top;
}

std::ostream& operator<<(std::ostream& os, liblas::Summary const& s)
{
    liblas::property_tree::ptree tree = s.GetPTree();
    
    os << "---------------------------------------------------------" << std::endl;
    os << "  Point Inspection Summary" << std::endl;
    os << "---------------------------------------------------------" << std::endl;

    if (tree.get<uint32_t>("summary.points.count") == 0 )
    {
        os << "  File has no points ...";
        return os;
    }
    os << "  Header Point Count: " << tree.get<uint32_t>("summary.header.count") << std::endl;
    os << "  Actual Point Count: " << tree.get<uint32_t>("summary.points.count") << std::endl;
    
    os << std::endl;
    os << "  Minimum and Maximum Attributes (min,max)" << std::endl;
    os << "---------------------------------------------------------" << std::endl;
    
    uint32_t x_precision = 6;
    uint32_t y_precision = 6;
    uint32_t z_precision = 6;

    try {
        double x_scale = tree.get<double>("summary.header.scale.x");
        double y_scale = tree.get<double>("summary.header.scale.y");
        double z_scale = tree.get<double>("summary.header.scale.z");
        x_precision = GetStreamPrecision(x_scale);
        y_precision = GetStreamPrecision(y_scale);
        z_precision = GetStreamPrecision(z_scale);

    }
    catch (liblas::property_tree::ptree_bad_path const& e) {
        ::boost::ignore_unused_variable_warning(e);
    }
    
    os << "  Min X, Y, Z: \t\t";
    os.setf(std::ios_base::fixed, std::ios_base::floatfield);
    os.precision(x_precision);
    os << tree.get<double>("summary.points.minimum.x") << ", ";
    os.precision(y_precision);
    os << tree.get<double>("summary.points.minimum.y") << ", ";
    os.precision(z_precision);
    os << tree.get<double>("summary.points.minimum.z");

    os << std::endl;
    os << "  Max X, Y, Z: \t\t";
    os.setf(std::ios_base::fixed, std::ios_base::floatfield);
    os.precision(x_precision);
    os << tree.get<double>("summary.points.maximum.x") << ", ";
    os.precision(y_precision);
    os << tree.get<double>("summary.points.maximum.y") << ", ";
    os.precision(z_precision);
    os << tree.get<double>("summary.points.maximum.z");
    
    os << std::endl;
    os << "  Bounding Box:\t\t";

    os.precision(x_precision);
    os << tree.get<double>("summary.points.minimum.x") << ", ";
    os.precision(y_precision);
    os << tree.get<double>("summary.points.minimum.y") << ", ";
    os.precision(x_precision);
    os << tree.get<double>("summary.points.maximum.x") << ", ";
    os.precision(y_precision);
    os << tree.get<double>("summary.points.maximum.y");
    
    os << std::endl;
    os.precision(6);
    os << "  Time:\t\t\t" << tree.get<double>("summary.points.minimum.time") << ", "
       << tree.get<double>("summary.points.maximum.time");


    os << std::endl;
    os.setf(std::ios::floatfield);
    os << "  Return Number:\t" << tree.get<uint32_t>("summary.points.minimum.returnnumber") << ", "
       << tree.get<uint32_t>("summary.points.maximum.returnnumber");

    os << std::endl;
    os << "  Return Count:\t\t" << tree.get<uint32_t>("summary.points.minimum.numberofreturns") << ", "
       << tree.get<uint32_t>("summary.points.maximum.numberofreturns");

    os << std::endl;
    os << "  Flightline Edge:\t" << tree.get<uint32_t>("summary.points.minimum.flightlineedge") << ", "
       << tree.get<uint32_t>("summary.points.maximum.flightlineedge");

    os << std::endl;
    os << "  Intensity:\t\t" << tree.get<uint32_t>("summary.points.minimum.intensity") << ", "
       << tree.get<uint32_t>("summary.points.maximum.intensity");

    os << std::endl;
    os << "  Scan Direction Flag:\t" << tree.get<int32_t>("summary.points.minimum.scandirection") << ", "
       << tree.get<int32_t>("summary.points.maximum.scandirection");

    os << std::endl;
    os << "  Scan Angle Rank:\t" << tree.get<int32_t>("summary.points.minimum.scanangle") << ", "
       << tree.get<int32_t>("summary.points.maximum.scanangle");

    os << std::endl;
    os << "  Classification:\t" << tree.get<int32_t>("summary.points.minimum.classification.id") << ", "
       << tree.get<int32_t>("summary.points.maximum.classification.id");

    os << std::endl;
    os << "  Point Source Id:\t" << tree.get<uint32_t>("summary.points.minimum.pointsourceid") << ", "
       << tree.get<uint32_t>("summary.points.maximum.pointsourceid");

    os << std::endl;
    os << "  User Data:\t\t" << tree.get<uint32_t>("summary.points.minimum.userdata") << ", "
       << tree.get<uint32_t>("summary.points.maximum.userdata");

    os << std::endl;
    os << "  Minimum Color (RGB):\t" 
       << tree.get<uint32_t>("summary.points.minimum.color.red") << " "
       << tree.get<uint32_t>("summary.points.minimum.color.green") << " "
       << tree.get<uint32_t>("summary.points.minimum.color.blue") << " ";

    os << std::endl;
    os << "  Maximum Color (RGB):\t" 
       << tree.get<uint32_t>("summary.points.maximum.color.red") << " "
       << tree.get<uint32_t>("summary.points.maximum.color.green") << " "
       << tree.get<uint32_t>("summary.points.maximum.color.blue") << " ";

    os << std::endl;
    os << std::endl;
    os << "  Number of Points by Return" << std::endl;
    os << "---------------------------------------------------------" << std::endl;
    
    BOOST_FOREACH(ptree::value_type &v,
            tree.get_child("summary.points.points_by_return"))
    {
        uint32_t i = v.second.get<uint32_t>("id");
        uint32_t count = v.second.get<uint32_t>("count");
        os << "\t(" << i << ") " << count;
    }
    
    os << std::endl;
    os << std::endl;
    os << "  Number of Returns by Pulse" << std::endl;
    os << "---------------------------------------------------------" << std::endl;
    
    BOOST_FOREACH(ptree::value_type &v,
            tree.get_child("summary.points.returns_of_given_pulse"))
    {
        uint32_t i = v.second.get<uint32_t>("id");
        uint32_t count = v.second.get<uint32_t>("count");
        os << "\t(" << i << ") " << count;
    }     

    os << std::endl;
    os << std::endl;
    os << "  Point Classifications" << std::endl;
    os << "---------------------------------------------------------" << std::endl;
    

    BOOST_FOREACH(ptree::value_type &v,
            tree.get_child("summary.points.classification"))
    {
        uint32_t i = v.second.get<uint32_t>("id");
        uint32_t count = v.second.get<uint32_t>("count");
        std::string name = v.second.get<std::string>("name");
        os << "\t" << count << " " << name << " (" << i << ") " << std::endl;;
    }

    os << "  -------------------------------------------------------" << std::endl;
    os << "  \t" << tree.get<uint32_t>("summary.points.encoding.withheld") << " withheld" << std::endl;
    os << "  \t" << tree.get<uint32_t>("summary.points.encoding.keypoint") << " keypoint" << std::endl;
    os << "  \t" << tree.get<uint32_t>("summary.points.encoding.synthetic") << " synthetic" << std::endl;
    os << "  -------------------------------------------------------" << std::endl;

    return os;
    
}

CoordinateSummary::CoordinateSummary() :
    FilterI(eInclusion)
    , count(0)
    , first(true)
    , bHaveHeader(false)
    , bHaveColor(true)
    , bHaveTime(true)
    
{
    points_by_return.assign(0);
    returns_of_given_pulse.assign(0);    
}

CoordinateSummary::CoordinateSummary(CoordinateSummary const& other)
    : FilterI(eInclusion)
    , count(other.count)
    , points_by_return(other.points_by_return)
    , returns_of_given_pulse(other.returns_of_given_pulse)
    , first(other.first)
    , minimum(boost::shared_ptr<liblas::Point>(new liblas::Point(*other.minimum)))
    , maximum(boost::shared_ptr<liblas::Point>(new liblas::Point(*other.maximum)))
    , m_header(other.m_header)
    , bHaveHeader(other.bHaveHeader)
    , bHaveColor(other.bHaveColor)
    , bHaveTime(other.bHaveTime)
    
{
}

CoordinateSummary& CoordinateSummary::operator=(CoordinateSummary const& rhs)
{
    if (&rhs != this)
    {
        count = rhs.count;
        first = rhs.first;
        points_by_return = rhs.points_by_return;
        returns_of_given_pulse = rhs.returns_of_given_pulse;
        minimum = boost::shared_ptr<liblas::Point>(new liblas::Point(*rhs.minimum));
        maximum = boost::shared_ptr<liblas::Point>(new liblas::Point(*rhs.maximum));
        m_header = rhs.m_header;
        bHaveHeader = rhs.bHaveHeader;
        bHaveColor = rhs.bHaveColor;
        bHaveTime = rhs.bHaveTime;        
    }
    return *this;
}

void CoordinateSummary::AddPoint(liblas::Point const& p)
{
        count++;

        if (first) {
            minimum = boost::shared_ptr<liblas::Point>(new liblas::Point(p));
            maximum = boost::shared_ptr<liblas::Point>(new liblas::Point(p));
            
            if (bHaveHeader)
            {
                maximum.get()->SetHeader(&m_header);
                minimum.get()->SetHeader(&m_header);
            }
            
            // We only summarize the base dimensions 
            // but we want to be able to read/set them all.  The 
            // point copy here would set the header ptr of min/max
            // to be whatever might have come off of the file, 
            // and this may/may not have space for time/color
            
            // If we do have scale/offset values, we do want to keep those, 
            // however. 
            
            liblas::Header const* h = p.GetHeader();

            if (detail::compare_distance(h->GetScaleX(), m_header.GetScaleX()) ||
                detail::compare_distance(h->GetScaleY(), m_header.GetScaleY()) ||
                detail::compare_distance(h->GetScaleZ(), m_header.GetScaleZ()))
                            
            {
                m_header.SetScale(h->GetScaleX(), h->GetScaleY(), h->GetScaleZ());
                m_header.SetOffset(h->GetOffsetX(), h->GetOffsetY(), h->GetOffsetZ());
            }
            
            first = false;
        }
        
        if (p.GetRawX() < minimum.get()->GetRawX() )
            minimum.get()->SetRawX(p.GetRawX());
        if (p.GetRawX() > maximum.get()->GetRawX() )
            maximum.get()->SetRawX(p.GetRawX());

        if (p.GetRawY() < minimum.get()->GetRawY() )
            minimum.get()->SetRawY(p.GetRawY());
        if (p.GetRawY() > maximum.get()->GetRawY() )
            maximum.get()->SetRawY(p.GetRawY());

        if (p.GetRawZ() < minimum.get()->GetRawZ() )
            minimum.get()->SetRawZ(p.GetRawZ());
        if (p.GetRawZ() > maximum.get()->GetRawZ() )
            maximum.get()->SetRawZ(p.GetRawZ());

        points_by_return[p.GetReturnNumber()]++;
        returns_of_given_pulse[p.GetNumberOfReturns()]++;    
}

void CoordinateSummary::SetHeader(liblas::Header const& h) 
{
    m_header = h;
    minimum = boost::shared_ptr<liblas::Point>(new liblas::Point(&m_header));
    maximum = boost::shared_ptr<liblas::Point>(new liblas::Point(&m_header));
    bHaveHeader = true;
}

ptree CoordinateSummary::GetPTree() const
{
    ptree pt;
    
    ptree pmin = minimum.get()->GetPTree();
    ptree pmax = maximum.get()->GetPTree();
    
     
    pt.add_child("minimum", pmin);
    pt.add_child("maximum", pmax);

    ptree returns;
    bool have_returns = false;
    for (boost::array<uint32_t,8>::size_type i=0; i < points_by_return.size(); i++) {
        if (i == 0) continue;

        if (points_by_return[i] != 0)
        {
            have_returns = true;
            returns.put("id", i);
            returns.put("count", points_by_return[i]);
            pt.add_child("points_by_return.return", returns);
            
        }
    }
    
    if (! have_returns) {
        // Assume all points are first return
        returns.put("id", 1);
        returns.put("count", count);
        pt.add_child("points_by_return.return", returns);        
    }
    
    ptree pulses;
    for (boost::array<uint32_t,8>::size_type i=0; i < returns_of_given_pulse.size(); i++) {
        if (returns_of_given_pulse[i] != 0) {
            pulses.put("id",i);
            pulses.put("count", returns_of_given_pulse[i]);
            pt.add_child("returns_of_given_pulse.pulse", pulses);
        }
    }
    
    pt.put("count", count);

    liblas::property_tree::ptree top;
    if (bHaveHeader)
        top.add_child("summary.header",m_header.GetPTree());
    top.add_child("summary.points",pt);
    return top;
}

bool CoordinateSummary::filter(liblas::Point const& p)
{
    AddPoint(p);
    return true;
}


uint32_t GetStreamPrecision(double scale)
{
    double frac = 0;
    double integer = 0;
    
    frac = std::modf(scale, &integer);
    double precision = std::fabs(std::floor(std::log10(frac)));
    
    uint32_t output = static_cast<uint32_t>(precision);
    return output;
}
} // namespace liblas
