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

// Common test procedure for default constructed point data.
void test_default_point(liblas::LASPoint const& p);

// Common test procedure for default constructed header data.
void test_default_header(liblas::LASHeader const& h);

// Test of header data in trunk/test/data/TO_core_last_clip.las file
void test_file10_header(liblas::LASHeader const& h);

} // namespace tut

