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
#include <lasfilev1.hpp>
#include <lasexception.hpp>
#include <cstdio>

namespace liblas {

LASFile *Open(char *pszFilename) {
    return Open(pszFilename, eRead);
}

LASFile *Open(char *pszFilename, OpenMode eFlags) {
    FILE *fp = fopen(pszFilename);
    if (fp == NULL) {
        throw file_not_found(pszFilename);
    }

    /* read the first 4 bytes to ensure that it is indeed */
    char sHead[4];
    fread(sHead, 1, 4, fp);
    if (!(sHead[0] == 'L' && sHead[1] == 'A' && sHead[2] == 'S' 
        && sHead[3] == 'F'))
    {
        fclose(fp);
        throw unsupported_format("First four bytes of file were not LASF.");
    }

    /* check the version of the LAS file */
    uint8_t nVerMajor = 0;
    uint8_t nVerMinor = 0;
    fseek(fp, 16, SEEK_SET);
    fread(nVerMajor, 1, 1, fp);
    fread(nVerMinor, 1, 1, fp);

    if (nVerMajor == 1 && nVerMinor == 1) {
        /* construct and return an instance of LASFileV1 */
        LASFileV1 *poLASFile = new LASFileV1(pszFilename, eFlags);
        return poLASFile;
    }
    else if (nVerMajor == 2 && nVerMinor == 0) {
        throw unsupported_version(
            "Version 2.0 of the LAS format is not supported at this time.");
    }
    else {
        throw unsupported_version(
            "An unknown version of the LAS format was found.");
    }
}

void LASFile::SkipTo(uint64_t nRecord) {
    if (nRecord >= m_lPointCount) {
        /* throw exception */
        throw std::out_of_range(
              "provided value for next read point > point count");
    }
    m_lNextPoint = nRecord;
}

}; /* end namespace liblas */
