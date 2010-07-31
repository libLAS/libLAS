/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS bounds for C++ libLAS 
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

#include <liblas/lasbounds.hpp>
#include <liblas/laspoint.hpp>
// boost
#include <boost/array.hpp>
#include <boost/concept_check.hpp>
// std
#include <cmath>
#include <limits>
#include <string>
#include <sstream>

namespace liblas { 

Bounds::Bounds()
{
    mins.assign(0);
    maxs.assign(0);
}

Bounds::Bounds( double minx, 
                double miny, 
                double maxx, 
                double maxy, 
                double minz, 
                double maxz)
{
    mins[0] = minx;
    mins[1] = miny;
    mins[2] = minz;
    maxs[0] = maxx;
    maxs[1] = maxy;
    maxs[2] = maxz;
    
#ifdef DEBUG
    verify();
#endif

}
Bounds::Bounds( const Point& min, const Point& max)
{
    mins[0] = min.GetX();
    mins[1] = min.GetY();
    mins[2] = min.GetZ();
    maxs[0] = max.GetX();
    maxs[1] = max.GetY();
    maxs[2] = max.GetZ();
    
#ifdef DEBUG
    verify();
#endif

}

Bounds::Bounds( double minx, 
                double miny, 
                double maxx, 
                double maxy)
{
    mins[0] = minx;
    mins[1] = miny;
    mins[2] = 0;
    maxs[0] = maxx;
    maxs[1] = maxy;
    maxs[2] = 0;
    
#ifdef DEBUG
    verify();
#endif

}

Bounds::Bounds(Bounds const& other)
    : mins(other.mins)
    , maxs(other.maxs)
{
}

Bounds& Bounds::operator=(Bounds const& rhs) 
{
    if (&rhs != this)
    {
        mins = rhs.mins;
        maxs = rhs.maxs;
    }
    return *this;
}

void Bounds::verify()
{

    for (uint32_t d = 0; d < 3; ++d)
    {
        if (min(d) > max(d) )
        {
            // FIXME: direct comparison of float-point may not be robust enough --mloskot
            // check for infinitive region
            if (!(min(d) == std::numeric_limits<double>::max() ||
                 max(d) == -std::numeric_limits<double>::max() ))
            {
                std::ostringstream msg; 
                msg << "liblas::Bounds::verify: Minimum point at dimension " << d
                    << "is greater than maximum point.  Neither point is infinity.";
                throw std::runtime_error(msg.str());
            }
        }
    }
}

bool Bounds::equal(Bounds const& other) const
{
    // FIXME: direct comparison of float-point values may give wrong result --mloskot
    for (Array::size_type i = 0; i < 3; i++) {
        if (!(min(i) == other.min(i)) && !(max(i) == other.max(i))) 
        {
            return false;
        }
    }
    return true;
}

bool Bounds::intersects2d(Bounds const& other) const
{

    return (other.min(0) < max(0) && other.min(0) > min(0) && other.min(1) < max(1) && other.min(1) > max(1))
        || (other.max(0) < max(0) && other.max(0) > min(0) && other.min(1) < max(1) && other.min(1) > max(1))
        || (other.min(0) < max(0) && other.min(0) > min(0) && other.max(1) < max(1) && other.max(1) > max(1))
        || (other.max(0) < max(0) && other.max(0) > min(0) && other.max(1) < max(1) && other.max(1) > max(1));
}

bool Bounds::intersects3d(Bounds const& other) const
{
    boost::ignore_unused_variable_warning(other);
    // not implemented
    throw    std::runtime_error("not implemented");

}
} // namespace liblas
