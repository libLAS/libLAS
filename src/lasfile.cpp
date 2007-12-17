/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Implementation of the LASFile class. 
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#include <liblas.hpp>

namespace liblas {

void LASFile::SkipTo(uint64_t nRecord) {
    if (nRecord >= m_lPointCount) {
        /* throw exception */
        throw std::out_of_range(
              "provided value for next read point > point count");
    }
    m_lNextPoint = nRecord;
}

}; /* end namespace liblas */
