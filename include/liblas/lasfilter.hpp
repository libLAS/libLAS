/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS filter class 
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

#ifndef LIBLAS_LASFILTER_HPP_INCLUDED
#define LIBLAS_LASFILTER_HPP_INCLUDED

#include <liblas/lasversion.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/fwd.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <vector>

namespace liblas {

/// Defines public interface to LAS filter implementation.
class FilterI
{
public:
    
    /// Determines whether or not the filter keeps or rejects points that meet 
    /// filtering criteria
    enum FilterType
    {
        eExclusion = 0, ///< Filter removes point that meet the criteria of filter(const Point& point)
        eInclusion = 1 ///< Filter keeps point that meet the criteria of filter(const Point& point)
    };
    
    virtual bool filter(const Point& point) = 0;
    
    void SetType(FilterType t) {m_type = t;}
    FilterType GetType() const {return m_type; }

    virtual ~FilterI() {};

    FilterI(FilterType t) : m_type(t) {}
    
private:

    FilterI(FilterI const& other);
    FilterI& operator=(FilterI const& rhs);

    FilterType m_type;
};

class BoundsFilter: public FilterI
{
public:
    
    BoundsFilter(double minx, double miny, double maxx, double maxy);
    BoundsFilter(double minx, double miny, double maxx, double maxy, double minz, double maxz);
    BoundsFilter(Bounds const& bounds);
    bool filter(const Point& point);

private:
    double mins[3]; // TODO: use Bounds directly or array<double, 3> --mloskot
    double maxs[3];
    
    bool m_2d;

    BoundsFilter(BoundsFilter const& other);
    BoundsFilter& operator=(BoundsFilter const& rhs);
};


class ClassificationFilter: public FilterI
{
public:
    
    typedef std::vector<boost::uint8_t> class_list_type;

    ClassificationFilter(class_list_type classes);
    bool filter(const Point& point);
    
private:

    class_list_type m_classes;

    ClassificationFilter(ClassificationFilter const& other);
    ClassificationFilter& operator=(ClassificationFilter const& rhs);
};


class ThinFilter: public liblas::FilterI
{
public:

    ThinFilter(uint32_t thin);
    bool filter(const liblas::Point& point);


private:

    ThinFilter(ThinFilter const& other);
    ThinFilter& operator=(ThinFilter const& rhs);
    
    uint32_t thin_amount;
    uint32_t thin_count;
};



class ReturnFilter: public FilterI
{
public:
    
    typedef std::vector<boost::uint16_t> return_list_type;

    ReturnFilter(return_list_type returns, bool last_only);
    bool filter(const Point& point);
    
private:

    return_list_type m_returns;
    bool last_only;

    ReturnFilter(ReturnFilter const& other);
    ReturnFilter& operator=(ReturnFilter const& rhs);
};



} // namespace liblas

#endif // ndef LIBLAS_LASFILTER_HPP_INCLUDED
