/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Variable-length header entry class for libLAS
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/
#ifndef LASHEADER_HPP_INCLUDED
#define LASHEADER_HPP_INCLUDED

#include <exception>
#include <lastypes.hpp>

namespace liblas {

class LASRecordHeader
{
public:
    LASHeaderEntry(uint16_t nRecordID, uint16_t nRecordLen, char psUserID[16],
        char psDescription[32], uint8_t *pszData);
    ~LASHeaderEntry();

    int GetRecordID() const { return nRecordID; }
    int GetRecordLen() const { return nRecordLen; }
    const char *GetUserID() const { return psUserID; }
    const char *GetDescription() const { return pszDescription; }
    const char *GetData() const { return pszData; }
private:
    uint16_t m_nRecordID;
    uint16_t m_nRecordLen;
    char m_psUserID[16];
    char m_pszDescription[32];
    uint8_t *m_Data; /* nRecordLen bytes */
};

}; /* end namespace liblas */

#endif /* ndef LASHEADER_HPP_INCLUDED */
