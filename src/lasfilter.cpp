/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS filter implementation for C++ libLAS 
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

#include <liblas/lasfilter.hpp>
#include <liblas/lasclassification.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <vector>

using namespace boost;

namespace liblas { 

ClassificationFilter::ClassificationFilter( std::vector<boost::uint8_t> classes )
    : FilterI(eInclusion)
    , m_classes(classes) 
{
}

bool ClassificationFilter::filter(const Point& p)
{
    Classification c = p.GetClassification();
    boost::uint8_t cls = c.GetClass();
    
    // If the user gave us an empty set of classes to filter
    // we're going to return true regardless
    bool output = true;
    for (class_list_type::const_iterator it = m_classes.begin(); it != m_classes.end(); ++it) {
        
        if (cls == *it) {
            if (GetType() == eInclusion) {
                output = true;
            } else {
                output = false;
            }
            break;
        } else {
            output = false;
        }
    }
    return output;
}

BoundsFilter::BoundsFilter( double minx, double miny, double maxx, double maxy ) : FilterI(eInclusion)
{
    mins[0] = minx;
    mins[1] = miny;
    mins[2] = 0;
    maxs[0] = maxx;
    maxs[1] = maxy;
    maxs[2] = 0;
    m_2d = true;
}

BoundsFilter::BoundsFilter( double minx, double miny, double minz, double maxx, double maxy, double maxz ) : FilterI(eInclusion)
{
    mins[0] = minx;
    mins[1] = miny;
    mins[2] = minz;
    maxs[0] = maxx;
    maxs[1] = maxy;
    maxs[2] = maxz;
    m_2d = false;
}

BoundsFilter::BoundsFilter( Bounds<double> const& bounds) : FilterI(eInclusion)
{
    mins[0] = bounds.min(0);
    mins[1] = bounds.min(1);
    maxs[0] = bounds.max(0);
    maxs[1] = bounds.max(1);

    if (bounds.dimension() > 2)
        mins[2] = bounds.min(2);
        maxs[2] = bounds.max(2);
        m_2d = false;
}
bool BoundsFilter::filter(const Point& p)
{
    bool output = false;
    
    double x = p.GetX();
    double y = p.GetY();
    double z = p.GetZ();
    
    if (! m_2d) {
        if (x > mins[0] && y > mins[1] && z > mins[2]) {
            if (x < maxs[0] && y < maxs[1] && z < maxs[2]) {
                output = true;
            }
        }
    } else {
        if (x > mins[0] && y > mins[1]) {
            if (x < maxs[0] && y < maxs[1]) {
                output = true;
            }
        }
    }
    return output;
}



ThinFilter::ThinFilter( uint32_t thin ) :
 liblas::FilterI(eInclusion)
{
    thin_amount = thin;
    thin_count = 0;
}



bool ThinFilter::filter(const liblas::Point& p)
{
    // FIXME: why p is not used? --mloskot
    // Because this filter is really just a counter.  
    // It throws out all points that aren't 
    // thin_count % thin_amount == 0. --hobu
    boost::ignore_unused_variable_warning(p);

    // If thin_amount == thin_count, we throw this one out.
    bool output = false;
    if (thin_amount == thin_count)
    {
        output = true;
        thin_count = 0;
    }
    
    thin_count = thin_count + 1;
    
    return output;
}


ReturnFilter::ReturnFilter( return_list_type returns, bool last_only )
    : FilterI(eInclusion)
    , m_returns(returns), last_only(last_only)
{
}

bool ReturnFilter::filter(const Point& p)
{
    if (last_only) {
        bool output = false;
        if (p.GetReturnNumber() == p.GetNumberOfReturns()) {
            output = true;
        }

        // If the type is switched to eExclusion, we'll throw out all last returns.
        if (GetType() == eExclusion && output == true) {
            output = false;
        } else {
            output = true;
        }
        return output;
    }
    
    uint16_t r = p.GetReturnNumber();
    
    // If the user gave us an empty set of classes to filter
    // we're going to return true regardless
    bool output = true;
    for (return_list_type::const_iterator it = m_returns.begin(); it != m_returns.end(); ++it) {
        
        if (r == *it) {
            if (GetType() == eInclusion) {
                output = true;
            } else {
                output = false;
            }
            break;
        } else {
            output = false;
        }
    }
    return output;
}

ValidationFilter::ValidationFilter() :
 liblas::FilterI(eInclusion)
{

}



bool ValidationFilter::filter(const liblas::Point& p)
{

    bool output = false;
    if (p.IsValid()){
        if (GetType() == eInclusion) {
            output = true;
        } else {
            output = false;
        }    
    }

    return output;
}


ColorFilter::ColorFilter(liblas::Color const& low, liblas::Color const& high) :
 liblas::FilterI(eInclusion), m_low(low), m_high(high)
{

}

ColorFilter::ColorFilter(liblas::Color::value_type low_red, 
                liblas::Color::value_type high_red,
                liblas::Color::value_type low_blue,
                liblas::Color::value_type high_blue,
                liblas::Color::value_type low_green,
                liblas::Color::value_type high_green) :
 liblas::FilterI(eInclusion), m_low(low_red, low_green, low_blue), m_high(high_red, high_blue, high_green)
{

}

bool ColorFilter::DoExclude()
{
    if (GetType() == eInclusion) {
        return true;
    } else {
        return false;
    }  
}


bool ColorFilter::filter(const liblas::Point& p)
{

    liblas::Color const& c = p.GetColor();
    
    if (c.GetRed() < m_low.GetRed())
        return !DoExclude();
    
    if (c.GetRed() > m_high.GetRed())
        return !DoExclude();

    if (c.GetBlue() < m_low.GetBlue())
        return !DoExclude();
    
    if (c.GetBlue() > m_high.GetBlue())
        return !DoExclude();

    if (c.GetGreen() < m_low.GetGreen())
        return !DoExclude();
    
    if (c.GetGreen() > m_high.GetGreen())
        return !DoExclude();

    return DoExclude();
}

} // namespace liblas
