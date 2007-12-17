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


class LASFile {
public:

    enum OpenMode {
        eRead = 0,
        eReadWrite,
        eReadWriteUpdate
    };


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
   void SetPoint(const LASPoint &roPoint, uint64_t nPointID);
   void AddPoint(const LASPoint &roPoint);

private:
    FILE *m_fp;
    uint64_t m_nlNextPoint;
    uint64_t m_nlPointCount;
    char *m_pszFilename;
    /* invariably, we will need more members here, for example,
     * offsets, scaling factors, etc...
     */
};

}; /* end namespace liblas */

#endif /* ndef LIBLAS_HPP_INCLUDED */
