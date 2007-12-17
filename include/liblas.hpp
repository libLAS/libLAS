/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: C++ interface to libLAS, definition of LASFile base class 
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LIBLAS_HPP_INCLUDED
#define LIBLAS_HPP_INCLUDED

#include <exception>

#include <lastypes.hpp>
#include <laspoint.hpp>
#include <lasheader.hpp>

namespace liblas {

enum OpenMode {
    eRead = 0,
    eReadWrite,
    eReadWriteUpdate
}

class LASFile {
     FILE *fp;
     uint64_t nlNextPoint;
     uint64_t nlPointCount;
     /* invariably, we will need more members here, for example,
      * offsets, scaling factors, etc...
      */
public:
     LASFile(const char *pszFilename);
     LASFile(const char *pszFilename, OpenMode eCreateFlags);
     ~LASFile();

    const LASHeaderEntry &GetVariableHeaderRecord (uint16_t nEntryID) const;
    
    /* Get a specific point */
    const LASPoint &GetPointByID (uint64_t nPointID) const;

    /* for iterating over the entire file, one point at a time */
    const LASPoint &GetNextPoint() const;
    /* return to first record */
    void Rewind(); 
    /* Skip to record nRecord */
    void SkipTo(uint64_t nRecord);

    /* for writing point records */
    void SetPoint(const LASPoint &roPoint, long long nPointID);
    void AddPoint(const LASPoint &roPoint);
};

}; /* end namespace liblas */

#endif /* ndef LIBLAS_HPP_INCLUDED */
