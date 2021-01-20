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
#include <liblas/point.hpp>
#include <liblas/transform.hpp>
#include <liblas/detail/private_utility.hpp>
#include <liblas/export.hpp>

#include <boost/concept_check.hpp>
// std
#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <vector>


namespace liblas {


template <typename T>
class LAS_DLL Range
{
public:
    T minimum;
    T maximum;

    typedef T value_type;

    Range(T mmin=(std::numeric_limits<T>::max)(), T mmax=(std::numeric_limits<T>::min)())
        : minimum(mmin), maximum(mmax) {}
    

    Range(Range const& other)
        : minimum(other.minimum)
        , maximum(other.maximum)
    {
    }

    Range& operator=(Range<T> const& rhs)
    {
        if (&rhs != this)
        {
            minimum = rhs.minimum;
            maximum = rhs.maximum;
        }
        return *this;
    }
    
    bool operator==(Range<T> const& rhs) const
    {
        return equal(rhs);
    }
    
    bool operator!=(Range const& rhs) const
    {
        return !(equal(rhs));
    }
    
    bool equal(Range const& other) const
    {
        
        if    (!(detail::compare_distance(minimum, other.minimum))  
            || !(detail::compare_distance(maximum, other.maximum))) 
        {
            return false;
        }
    
        return true;
    }
    
    bool overlaps(Range const& r) const 
    {
        return minimum < r.maximum && maximum > r.minimum;
    }

    bool contains(Range const& r) const
    {
        return minimum <= r.minimum && r.maximum <= maximum;
    }
    
    bool contains(T v) const
    {
        return minimum <= v && v <= maximum;
    }
    
    bool empty(void) const 
    {
        return detail::compare_distance(minimum, (std::numeric_limits<T>::max)()) && detail::compare_distance(maximum, (std::numeric_limits<T>::min)());
    }
    
    void shift(T v) 
    {
        minimum += v;
        maximum += v;
    }
    
    void scale(T v) 
    {
        minimum *= v;
        maximum *= v;
    }
    
    void clip(Range const& r)
    {
        if (r.minimum > minimum)
            minimum = r.minimum;
        if (r.maximum < maximum)
            maximum = r.maximum;
    }
    
    void grow(T v) 
    {
        if (v < minimum) 
            minimum = v;
        if (v > maximum)
            maximum = v;
    }

    void grow(Range const& r) 
    {
        grow(r.minimum);
        grow(r.maximum);
    }

    T length() const
    {
        return maximum - minimum;
    }
};
    
template <typename T>
class Bounds
{
public:

	typedef T value_type;
    typedef typename std::vector< Range<T> >::size_type size_type;
    
    typedef typename std::vector< Range<T> > RangeVec;
private:

    RangeVec ranges;
    
public:

Bounds<T>()
{
    ranges.resize(0);
}

Bounds(Bounds const& other)
    : 
    ranges(other.ranges)
{
}

Bounds(RangeVec const& rngs)
    : 
    ranges(rngs)
{
}

Bounds( T minx, 
        T miny, 
        T minz, 
        T maxx, 
        T maxy, 
        T maxz)
{
    ranges.resize(3);
    
    ranges[0].minimum = minx;
    ranges[1].minimum = miny;
    ranges[2].minimum = minz;

    ranges[0].maximum = maxx;
    ranges[1].maximum = maxy;
    ranges[2].maximum = maxz;
    
#ifdef DEBUG
    verify();
#endif

}

Bounds( T minx, 
        T miny, 
        T maxx, 
        T maxy)
{

    ranges.resize(2);

    ranges[0].minimum = minx;
    ranges[1].minimum = miny;

    ranges[0].maximum = maxx;
    ranges[1].maximum = maxy;
    
#ifdef DEBUG
    verify();
#endif

}

Bounds( const Point& min, const Point& max)
{
    ranges.resize(3);
    
    ranges[0].minimum = min.GetX();
    ranges[1].minimum = min.GetY();
    ranges[2].minimum = min.GetZ();

    ranges[0].maximum = max.GetX();
    ranges[1].maximum = max.GetY();
    ranges[2].maximum = max.GetZ();
    
#ifdef DEBUG
    verify();
#endif

}


// Bounds( Vector const& low, Vector const& high)
// {
//     if (low.size() != high.size() ) {
//         std::ostringstream msg; 
//         msg << "Bounds dimensions are not equal.  Low bounds dimensions are " << low.size()
//             << " and the high bounds are " << high.size();
//         throw std::runtime_error(msg.str());                
//     }
//     mins.resize(low.size());
//     
//     mins = low;
//     maxs = high;
//     
// #ifdef DEBUG
//     verify();
// #endif
// 
// }

T (min)(std::size_t const& index) const
{
    if (ranges.size() <= index) {
        // std::ostringstream msg; 
        // msg << "Bounds dimensions, " << ranges.size() <<", is less "
        //     << "than the given index, " << index;
        // throw std::runtime_error(msg.str());                
        return 0;
    }
    return ranges[index].minimum;
}

void (min)(std::size_t const& index, T v)
{
    if (ranges.size() <= index) {
        ranges.resize(index + 1);
    }
    ranges[index].minimum = v;
}

T (max)(std::size_t const& index) const
{
    if (ranges.size() <= index) {
        // std::ostringstream msg; 
        // msg << "Bounds dimensions, " << ranges.size() <<", is less "
        //     << "than the given index, " << index;
        // throw std::runtime_error(msg.str());    
        return 0;
    }
    return ranges[index].maximum;
}

void (max)(std::size_t const& index, T v)
{
    if (ranges.size() <= index) {
        ranges.resize(index + 1);
    }
    ranges[index].maximum = v;
}

// liblas::Point (min)() {
//     liblas::Point p(&DefaultHeader::get());
//     try
//     {
//         p.SetCoordinates(ranges[0].minimum, ranges[1].minimum, ranges[2].minimum);
//     }
//     catch (std::runtime_error const& )
//     {
//         p.SetCoordinates(ranges[0].minimum, ranges[1].minimum, 0);
//
//     }
//
//     return p;
// }
//
// liblas::Point (max)() {
//     liblas::Point p(&DefaultHeader::get());
//     try
//     {
//         p.SetCoordinates(ranges[0].maximum, ranges[1].maximum, ranges[2].maximum);
//     }
//     catch (std::runtime_error const& )
//     {
//         p.SetCoordinates(ranges[0].maximum, ranges[1].maximum, 0);
//
//     }
//     return p;
// }

T minx() const { if (ranges.size() == 0) return 0; return ranges[0].minimum; }
T miny() const { if (ranges.size() < 2) return 0; return ranges[1].minimum; }
T minz() const { if (ranges.size() < 3) return 0; return ranges[2].minimum; }
T maxx() const { if (ranges.size() == 0) return 0; return ranges[0].maximum; }
T maxy() const { if (ranges.size() < 2) return 0; return ranges[1].maximum; }
T maxz() const { if (ranges.size() < 3) return 0; return ranges[2].maximum; }

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
        ranges = rhs.ranges;
    }
    return *this;
}

/// The vector of Range<T> for the Bounds
RangeVec const& dims () const { return ranges; }

/// The number of dimensions of the Bounds
size_type dimension() const
{
    return ranges.size();
}

/// Resize the dimensionality of the Bounds to d
void dimension(size_type d)
{
    if (ranges.size() < d) {
        ranges.resize(d);
    }    
}

/// Is this Bounds equal to other?
bool equal(Bounds<T> const& other) const
{
    for (size_type i = 0; i < dimension(); i++) {
        if ( ranges[i] != other.ranges[i] )
            return false;
    }
    return true;
}

/// Does this Bounds intersect other?
bool intersects(Bounds const& other) const
{

    for (size_type i = 0; i < dimension(); i++) {
        if ( ranges[i].overlaps(other.ranges[i]) )
            return true;
    }
    
    return false;

}

/// Synonym for intersects for now
bool overlaps(Bounds const& other) const
{
    return intersects(other);
}

/// Does this Bounds contain other?
bool contains(Bounds const& other) const
{
    for (size_type i = 0; i < dimension(); i++) {
        if ( !ranges[i].contains(other.ranges[i]) )
            return false; // As soon as it is not contains, we're false
    }
    return true;
}

/// Does this Bounds this point other?
bool contains(Point const& point) const
{
    // std::cout << ranges[0].length() << std::endl;
    // std::cout << "x contain: " << ranges[0].contains(point.GetX()) 
    //           << " r.x.min: " << ranges[0].min 
    //           << " r.x.max: " << ranges[0].max 
    //           << " p.x: " << point.GetX() << std::endl;
    // std::cout << "y contain: " << ranges[1].contains(point.GetY()) 
    //           << " r.y.min: " << ranges[1].min 
    //           << " r.y.max: " << ranges[1].max 
    //           << " p.y: " << point.GetY() << std::endl;
    // std::cout << "z contain: " << ranges[2].contains(point.GetZ()) 
    //           << " r.z.min: " << ranges[2].min 
    //           << " r.z.max: " << ranges[2].max 
    //           << " p.z: " << point.GetZ() << std::endl;
    if (!ranges[0].contains(point.GetX()))
        return false;
    if (!ranges[1].contains(point.GetY()))
        return false;
        
    // If our z bounds has no length, we'll say it's contained anyway.
    if (!ranges[2].contains(point.GetZ())) 
    {
        if (detail::compare_distance(ranges[2].length(), 0.0))
            return true;
        return false;
    }
    return true;
}
/// Shift each dimension by a vector of detlas
void shift(std::vector<T> deltas)
{
    typedef typename std::vector< T >::size_type size_type;

    size_type i;
    if( dimension() != deltas.size()) 
    {
        std::ostringstream msg; 
        msg << "liblas::Bounds::shift: Delta vector size, " << deltas.size()
            << ", is not equal to the dimensionality of the bounds, "<< dimension() << ".";
        throw std::runtime_error(msg.str());
    }
    for (i = 0; i < deltas.size(); ++i){
        ranges[i].shift(deltas[i]);
    }
}

/// Scale each dimension by a vector of scale factors
void scale(std::vector<T> factors)
{
    typedef typename std::vector< T >::size_type size_type;

    size_type i;
    if( dimension() != factors.size()) 
    {
        std::ostringstream msg; 
        msg << "liblas::Bounds::scale: Scale factor vector size, " << factors.size()
            << ", is not equal to the dimensionality of the bounds, "<< dimension() << ".";
        throw std::runtime_error(msg.str());
    }
    for (i = 0; i < factors.size(); ++i){
        ranges[i].scale(factors[i]);
    }
}

/// Clip this Bounds to the extent of r
void clip(Bounds const& r)
{
    RangeVec ds = r.dims();
    for (size_type i = 0; i < dimension(); ++i){
        ranges[i].clip(ds[i]);
    }    
}

/// Grow to the union of two liblas::Bounds
void grow(Bounds const& r)
{
    RangeVec ds = r.dims();
    for (size_type i = 0; i < dimension(); ++i){
        ranges[i].grow(ds[i]);
    }    
}

/// Expand the liblas::Bounds to include this point
void grow(Point const& p)
{
    ranges[0].grow(p.GetX());
    ranges[1].grow(p.GetY());
    ranges[2].grow(p.GetZ());
}

T volume() const
{
    T output = T();
    for (size_type i = 0; i < dimension(); i++) {
        output = output * ranges[i].length();
    }
    
    return output;
}

bool empty() const
{
    for (size_type i = 0; i < dimension(); i++) {
        if (ranges[i].empty())
            return true;
    }
    return false;
}

void verify()
{
    for (size_type d = 0; d < dimension(); ++d)
    {
        if ((min)(d) > (max)(d) )
        {
            // Check that we're not infinity either way
            if ( (detail::compare_distance((min)(d), (std::numeric_limits<T>::max)()) ||
                  detail::compare_distance((max)(d), -(std::numeric_limits<T>::max)()) ))
            {
                std::ostringstream msg; 
                msg << "liblas::Bounds::verify: Minimum point at dimension " << d
                    << "is greater than maximum point.  Neither point is infinity.";
                throw std::runtime_error(msg.str());
            }
        }
    }
}

// Bounds<T> project(liblas::SpatialReference const& in_ref, liblas::SpatialReference const& out_ref)
// {
//     liblas::ReprojectionTransform trans(in_ref, out_ref);
//
//     liblas::Point minimum = (min)();
//     liblas::Point maximum = (max)();
//     trans.transform(minimum);
//     trans.transform(maximum);
//     return Bounds<T>(minimum, maximum);
// }



};



} // namespace liblas

// Needed for C++ DLL exports
#ifdef _MSC_VER
template class LAS_DLL liblas::Range<double>;
template class LAS_DLL liblas::Bounds<double>;
#endif

#endif // ndef LIBLAS_LASBOUNDS_HPP_INCLUDED
