// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/liblas.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lasheader.hpp>


namespace tut
{

// Predicate testing LASPoint against given XY coordinates
// and tolerance.
struct is_xy
{
    is_xy(double x, double y, double tolerance)
        : x(x), y(y), t(tolerance)
    {}

    bool operator()(liblas::Point const& p)
    {
        double const dx = x - p.GetX();
        double const dy = y - p.GetY();

        return ((dx <= t && dx >= -t) && (dy <= t && dy >= -t));
    }

    double x;
    double y;
    double t;
};

// Functor to calculate bounding box of a set of points
struct bbox_calculator
{
    // bbox object will store operation result
    bbox_calculator(liblas::detail::Extents<double>& bbox)
        : empty(true), bbox(bbox)
    {}

    void operator()(liblas::Point const& p)
    {
        // Box initialization during first iteration only
        if (empty)
        {
            bbox.min.x = bbox.max.x = p.GetX();
            bbox.min.y = bbox.max.y = p.GetY();
            bbox.min.z = bbox.max.z = p.GetZ();
            empty = false;
        }

        // Expand bounding box to include given point
        bbox.min.x = std::min(bbox.min.x, p.GetX());
        bbox.min.y = std::min(bbox.min.y, p.GetY());
        bbox.min.z = std::min(bbox.min.z, p.GetZ());
        bbox.max.x = std::max(bbox.max.x, p.GetX());
        bbox.max.y = std::max(bbox.max.y, p.GetY());
        bbox.max.z = std::max(bbox.max.z, p.GetZ());
    }

    bool empty;
    liblas::detail::Extents<double>& bbox;
};

// Common test procedure for default constructed point data.
void test_default_point(liblas::Point const& p);

// Common test procedure for default constructed header data.
void test_default_header(liblas::Header const& h);

// Test of header data in trunk/test/data/TO_core_last_clip.las file
void test_file10_header(liblas::Header const& h);

// Test of 1st point record in trunk/test/data/TO_core_last_clip.las file
void test_file10_point1(liblas::Point const& p);

// Test of 2nd point record in trunk/test/data/TO_core_last_clip.las file
void test_file10_point2(liblas::Point const& p);

// Test of 4th point record in trunk/test/data/TO_core_last_clip.las file
void test_file10_point4(liblas::Point const& p);

} // namespace tut

