/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS bounds class 
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

#ifndef LIBLAS_LASBOUNDS_HPP_INCLUDED
#define LIBLAS_LASBOUNDS_HPP_INCLUDED

#include <liblas/detail/fwd.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/utility.hpp>

// boost
#include <boost/cstdint.hpp>

#include <boost/concept_check.hpp>
// std
#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <vector>


namespace liblas {

template <typename T>
class Bounds
{
public:

    typedef typename std::vector<T> Vector;
    typedef typename std::vector<T>::size_type size_type;
        
private:
    Vector mins;
    Vector maxs;

public:

Bounds<T>()
{
    mins.resize(0);
    maxs.resize(0);
}

Bounds(Bounds const& other)
    : mins(other.mins)
    , maxs(other.maxs)
{
}

Bounds( T minx, 
        T miny, 
        T minz, 
        T maxx, 
        T maxy, 
        T maxz)
{
    mins.resize(3);
    maxs.resize(3);
    
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

Bounds( T minx, 
        T miny, 
        T maxx, 
        T maxy)
{
    mins.resize(2);
    maxs.resize(2);
    mins[0] = minx;
    mins[1] = miny;
    maxs[0] = maxx;
    maxs[1] = maxy;
    
#ifdef DEBUG
    verify();
#endif

}

Bounds( const Point& min, const Point& max)
{
    mins.resize(3);
    maxs.resize(3);
    
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


Bounds( Vector const& low, Vector const& high)
{
    if (low.size() != high.size() ) {
        std::ostringstream msg; 
        msg << "Bounds dimensions are not equal.  Low bounds dimensions are " << low.size()
            << " and the high bounds are " << high.size();
        throw std::runtime_error(msg.str());                
    }
    mins.resize(low.size());
    
    mins = low;
    maxs = high;
    
#ifdef DEBUG
    verify();
#endif

}

T min(std::size_t const& index) const
{
    if (mins.size() <= index) {
        return 0.0;
    }
    return mins[index];
}

void min(std::size_t const& index, T v)
{
    if (mins.size() <= index) {
        mins.resize(index + 1);
        maxs.resize(index + 1);
    }
    mins[index] = v;
}

T max(std::size_t const& index) const
{
    if (maxs.size() <= index) {
        return 0.0;
    }
        return maxs[index];
}

void max(std::size_t const& index, T v)
{
    if (maxs.size() <= index) {
        maxs.resize(index + 1);
        mins.resize(index + 1);
    }
    maxs[index] = v;
}

inline bool operator==(Bounds<T> const& rhs) const
{
    return equal(rhs);
}

inline bool operator!=(Bounds<T> const& rhs) const
{
    return (!equal(rhs));
}


Bounds<T>& operator=(Bounds<T> const& rhs) 
{
    if (&rhs != this)
    {
        mins = rhs.mins;
        maxs = rhs.maxs;
    }
    return *this;
}

size_type dimension() const
{
    return mins.size();
}

void dimension(Bounds::size_type d)
{
    if (maxs.size() < d) {
        maxs.resize(d);
    }    
    if (mins.size() < d){
        mins.resize(d);
    }
}


bool equal(Bounds<T> const& other) const
{
    for (size_type i = 0; i < dimension(); i++) {
        
        if    (!(detail::compare_distance(min(i), other.min(i)))  
            || !(detail::compare_distance(max(i), other.max(i)))) 
        {
            return false;
        }
    }
    return true;
}

bool intersects(Bounds const& other) const
{
    
    if (other.dimension() != dimension())
    {
        std::ostringstream msg; 
        msg << "Bounds dimensions are not equal.  Comparison dimension is " << other.dimension()
            << " and this dimension is " << dimension();
        throw std::runtime_error(msg.str());        
    }

    for (size_type i = 0; i < dimension(); i++){
        if (min(i) > other.max(i) || max(i) < other.min(i)) return false;
    }
    
    return true;
}


void verify()
{
    for (size_type d = 0; d < dimension(); ++d)
    {
        if (min(d) > max(d) )
        {
            // Check that we're not infinity either way
            if ( (detail::compare_distance(min(d), std::numeric_limits<T>::max()) ||
                  detail::compare_distance(max(d), -std::numeric_limits<T>::max()) ))
            {
                std::ostringstream msg; 
                msg << "liblas::Bounds::verify: Minimum point at dimension " << d
                    << "is greater than maximum point.  Neither point is infinity.";
                throw std::runtime_error(msg.str());
            }
        }
    }
}


};
} // namespace liblas

#endif // ndef LIBLAS_LASBOUNDS_HPP_INCLUDED
