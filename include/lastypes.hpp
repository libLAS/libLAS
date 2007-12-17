/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Common Portability Library types modified for libLAS.
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 1998, 2005 Frank Warmerdam <warmerdam@pobox.com>
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LASTYPES_HPP_INCLUDED
#define LASTYPES_HPP_INCLUDED

namespace liblas {
/* ==================================================================== */
/*      Base portability stuff ... this stuff may need to be            */
/*      modified for new platforms.                                     */
/* ==================================================================== */

/*---------------------------------------------------------------------
 *        types for 16 and 32 bits integers, etc...
 *--------------------------------------------------------------------*/
#if UINT_MAX == 65535
typedef long            int32_t;
typedef unsigned long   uint32_t;
#else
typedef int             int32_t;
typedef unsigned int    uint32_t;
#endif

typedef short           int16_t;
typedef unsigned short  uint16_t;
typedef unsigned char   uint8_t;

/* -------------------------------------------------------------------- */
/*      64bit support                                                   */
/* -------------------------------------------------------------------- */

#if defined(WIN32) && defined(_MSC_VER)

#define VSI_LARGE_API_SUPPORTED
typedef __int64            int64_t;
typedef unsigned __int64   uint64_t;

#elif HAVE_LONG_LONG

typedef long long          int64_t;
typedef unsigned long long uint64_t;

#else

typedef long               int64_t;
typedef unsigned long      uint64_t;

#endif

}; /* end namespace liblas */

#endif /* ndef LASTYPES_HPP_INCLUDED */
