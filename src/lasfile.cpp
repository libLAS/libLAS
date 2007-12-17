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

LASFile::LASFile(const char *pszFilename) {

}

LASFile::LASFile(const char *pszFilename, OpenMode eCreateFlags) {

}

LASFile::~LASFile() {

}

const LASHeaderEntry &LASFile::GetVariableHeaderRecord(uint16_t nEntryID) const
{

}

const LASPoint &LASFile::GetPointByID (uint16_t nPointID) const
{

}

void LASFile::Rewind() {

}

void LASFile::SkipTo(uint64_t nRecord) {

}

void LASFile::SetPoint(const LASPoint &roPoint, uint64_t nPointID) {

}

void LASFile::AddPoint(const LASPoint &roPoint) {

}

}; /* end namespace liblas */
