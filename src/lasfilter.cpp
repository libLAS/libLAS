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

namespace liblas { 



ClassificationFilter::ClassificationFilter( std::vector<liblas::uint8_t> classes ) : 
m_classes(classes), 
m_keep(true)
{
}



bool ClassificationFilter::filter(const Point& p)
{
    Classification c = p.GetClassification();
    liblas::uint8_t cls = c.GetClass();
    
    // If the user gave us an empty set of classes to filter
    // we're going to return true regardless
    bool output = true;
    std::vector<liblas::uint8_t>::const_iterator i;
    for (i = m_classes.begin(); i != m_classes.end(); ++i) {
        
        if (cls == *i) {
            if (m_keep == true) {
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


BoundsFilter::BoundsFilter( double minx, double miny, double maxx, double maxy ) 
{
    mins[0] = minx;
    mins[1] = miny;
    mins[2] = 0;
    maxs[0] = maxx;
    maxs[1] = maxy;
    maxs[2] = 0;
    m_2d = true;
}

BoundsFilter::BoundsFilter( double minx, double miny, double maxx, double maxy, double minz, double maxz ) 
{
    mins[0] = minx;
    mins[1] = miny;
    mins[2] = minz;
    maxs[0] = maxx;
    maxs[1] = maxy;
    maxs[2] = maxz;
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


} // namespace liblas
