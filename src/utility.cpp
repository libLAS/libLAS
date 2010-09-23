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
    first(true)
{
    classes.assign(0);
    points_by_return.assign(0);
    returns_of_given_pulse.assign(0);    
}

void Summary::AddPoint(liblas::Point const& p)
{
        count++;

        if (first) {
            min = p;
            max = p;
            first = false;
        }
        
        min.SetX(std::min(p.GetX(), min.GetX()));
        max.SetX(std::max(p.GetX(), max.GetX()));

        min.SetY(std::min(p.GetY(), min.GetY()));
        max.SetY(std::max(p.GetY(), max.GetY()));        

        min.SetZ(std::min(p.GetZ(), min.GetZ()));
        max.SetZ(std::max(p.GetZ(), max.GetZ()));

        min.SetIntensity(std::min(p.GetIntensity(), min.GetIntensity()));
        max.SetIntensity(std::max(p.GetIntensity(), max.GetIntensity()));

        min.SetTime(std::min(p.GetTime(), min.GetTime()));
        max.SetTime(std::max(p.GetTime(), max.GetTime()));

        min.SetReturnNumber(std::min(p.GetReturnNumber(), min.GetReturnNumber()));
        max.SetReturnNumber(std::max(p.GetReturnNumber(), max.GetReturnNumber()));

        min.SetNumberOfReturns(std::min(p.GetNumberOfReturns(), min.GetNumberOfReturns()));
        max.SetNumberOfReturns(std::max(p.GetNumberOfReturns(), max.GetNumberOfReturns()));

        min.SetScanDirection(std::min(p.GetScanDirection(), min.GetScanDirection()));
        max.SetScanDirection(std::max(p.GetScanDirection(), max.GetScanDirection()));

        min.SetFlightLineEdge(std::min(p.GetFlightLineEdge(), min.GetFlightLineEdge()));
        max.SetFlightLineEdge(std::max(p.GetFlightLineEdge(), max.GetFlightLineEdge()));

        min.SetScanAngleRank(std::min(p.GetScanAngleRank(), min.GetScanAngleRank()));
        max.SetScanAngleRank(std::max(p.GetScanAngleRank(), max.GetScanAngleRank()));

        min.SetUserData(std::min(p.GetUserData(), min.GetUserData()));
        max.SetUserData(std::max(p.GetUserData(), max.GetUserData()));

        min.SetPointSourceID(std::min(p.GetPointSourceID(), min.GetPointSourceID()));
        max.SetPointSourceID(std::max(p.GetPointSourceID(), max.GetPointSourceID()));
        
        liblas::Classification const& cls = p.GetClassification();
        
        boost::uint8_t minc = std::min(cls.GetClass(), min.GetClassification().GetClass());
        boost::uint8_t maxc = std::max(cls.GetClass(), max.GetClassification().GetClass());
        
        classes[cls.GetClass()]++;
        
        if (cls.IsWithheld()) withheld++;
        if (cls.IsKeyPoint()) keypoint++;
        if (cls.IsSynthetic()) synthetic++;
        
        min.SetClassification(liblas::Classification(minc));
        max.SetClassification(liblas::Classification(maxc));
        
        liblas::Color const& color = p.GetColor();
        
        liblas::Color::value_type red;
        liblas::Color::value_type green;
        liblas::Color::value_type blue;
        
        red = std::min(color.GetRed(), min.GetColor().GetRed());
        green = std::min(color.GetGreen(), min.GetColor().GetGreen());
        blue = std::min(color.GetBlue(), min.GetColor().GetBlue());
        
        min.SetColor(liblas::Color(red, green, blue));
        
        red = std::max(color.GetRed(), max.GetColor().GetRed());
        green = std::max(color.GetGreen(), max.GetColor().GetGreen());
        blue = std::max(color.GetBlue(), max.GetColor().GetBlue());        

        max.SetColor(liblas::Color(red, green, blue));

        points_by_return[p.GetReturnNumber()]++;
        returns_of_given_pulse[p.GetNumberOfReturns()]++;    
}

ptree Summary::GetPTree() const
{
    ptree pt;
    
    ptree pmin = min.GetPTree();
    ptree pmax = max.GetPTree();
    
     
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
    pt.put("classification.withheld", withheld);
    pt.put("classification.keypoint", keypoint);
    pt.put("classification.synthetic", synthetic);
    
    ptree returns;
    for (boost::array<boost::uint32_t,8>::size_type i=0; i < points_by_return.size(); i++) {
        if (i == 0) continue;

        if (points_by_return[i] != 0)
        {
            returns.put("id", i);
            returns.put("count", points_by_return[i]);
            pt.add_child("points_by_return.return", returns);
            
        }
    }
    
    ptree pulses;
    for (boost::array<boost::uint32_t,8>::size_type i=0; i < returns_of_given_pulse.size(); i++) {
        if (returns_of_given_pulse[i] != 0) {
            pulses.put("id",i);
            pulses.put("count", returns_of_given_pulse[i]);
            pt.add_child("returns_of_given_pulse.pulse", pulses);
        }
    }
    
    pt.put("count", count);
    return pt;
}
} // namespace liblas
