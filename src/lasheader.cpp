/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Implementation of the variable-length header class 
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

LASRecordHeader::LASRecordHeader(uint16_t nRecordID, uint16_t nRecordLen, 
    char psUserID[16], char pszDescription[32], uint8_t *Data)
    : m_nRecordID(nRecordID), m_nRecordLen(nRecordLen), m_psUserID(psUserID),
      m_pszDescription(pszDescription), m_Data(Data)
{
}

}; /* end namespace liblas */
