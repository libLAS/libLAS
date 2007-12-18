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
    LASRecordHeader(uint16_t nRecordID, uint16_t nRecordLen, char psUserID[16],
        char psDescription[32], uint8_t *pszData);
    ~LASRecordHeader();

    int GetRecordID() const { return m_nRecordID; }
    int GetRecordLen() const { return m_nRecordLen; }
    const char *GetUserID() const { return m_psUserID; }
    const char *GetDescription() const { return m_pszDescription; }
    uint8_t *GetData() const { return m_Data; }
private:
    uint16_t m_nRecordID;
    uint16_t m_nRecordLen;
    char m_psUserID[16];
    char m_pszDescription[32];
    uint8_t *m_Data; /* nRecordLen bytes */
};

}; /* end namespace liblas */

#endif /* ndef LASHEADER_HPP_INCLUDED */
