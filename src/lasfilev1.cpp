/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Implementation of the LASFileV1 class -- reader for V1.x LAS
 *          data.
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#include <liblas.hpp>
#include <lasfilev1.hpp>

namespace liblas {
namespace details {

LASFileV1::LASFileV1(const char *pszFilename) {

}

LASFileV1::LASFileV1(const char *pszFilename, OpenMode eCreateFlags) {

}

LASFileV1::~LASFileV1() {

}

const LASRecordHeader &LASFileV1::GetVariableHeaderRecord(uint16_t nEntryID) 
    const 
{

}

const LASPoint &LASFileV1::GetPointByID(uint64_t nPointID) const {

}

const LASPoint &LASFileV1::GetNextPoint() const {

}

void  LASFileV1::SetPoint(const LASPoint &roPoint, uint64_t nPointID) {

}

void LASFileV1::AddPoint(const LASPoint &roPoint) {

}

}; /* end namespace liblas::details */
}; /* end namespace liblas */
