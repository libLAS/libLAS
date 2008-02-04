#ifndef LAS_CONFIG_H_INCLUDED
#define LAS_CONFIG_H_INCLUDED

/* ==================================================================== */
/*      Other standard services.                                        */
/* ==================================================================== */
#ifdef __cplusplus
#  define LAS_C_START           extern "C" {
#  define LAS_C_END             }
#else
#  define LAS_C_START
#  define LAS_C_END
#endif

#ifndef LAS_DLL
#if defined(_MSC_VER) && !defined(LAS_DISABLE_DLL)
#  define LAS_DLL     __declspec(dllexport)
#else
#  if defined(USE_GCC_VISIBILITY_FLAG)
#    define LAS_DLL     __attribute__ ((visibility("default")))
#  else
#    define LAS_DLL
#  endif
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif

#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif



// This file has been stolen from <boost/cstdint.hpp> and
// modified for libLAS purposes.
//
// Copyright Mateusz Loskot 2007, mateusz@loskot.net
// Copyright Phil Vachon 2007, philippe@cowpig.ca
// Copyright John Maddock 2001
// Copyright Jens Mauer 2001
// Copyright Beman Dawes 1999
// Distributed under the Boost  Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <limits.h> // needed for limits macros



//  These are fairly safe guesses for some 16-bit, and most 32-bit and 64-bit
//  platforms.  For other systems, they will have to be hand tailored.
//
//  Because the fast types are assumed to be the same as the undecorated types,
//  it may be possible to hand tailor a more efficient implementation.  Such
//  an optimization may be illusionary; on the Intel x86-family 386 on, for
//  example, byte arithmetic and load/stores are as fast as "int" sized ones.

//  8-bit types  ------------------------------------------------------------//

#if UCHAR_MAX == 0xff
    typedef signed char     int8_t;
    typedef unsigned char   uint8_t;
#else
#   error defaults not correct; you must hand modify liblas/cstdint.hpp
#endif

//  16-bit types  -----------------------------------------------------------//

#if USHRT_MAX == 0xffff
    typedef short           int16_t;
    typedef unsigned short  uint16_t;
#else
#   error defaults not correct; you must hand modify liblas/cstdint.hpp
#endif

//  32-bit types  -----------------------------------------------------------//

#if ULONG_MAX == 0xffffffff
    typedef long            int32_t;
    typedef unsigned long   uint32_t;
#elif UINT_MAX == 0xffffffff
    typedef int             int32_t;
    typedef unsigned int    uint32_t;
#else
#   error defaults not correct; you must hand modify liblas/cstdint.hpp
# endif

//  64-bit types + intmax_t and uintmax_t  ----------------------------------//

#if ULONG_MAX != 0xffffffff
# if ULONG_MAX == 18446744073709551615 // 2**64 - 1
    typedef long                int64_t;
    typedef unsigned long       uint64_t;
# else
#   error defaults not correct; you must hand modify boost/cstdint.hpp
# endif
#elif defined(__GNUC__) || defined(HAVE_LONG_LONG)
    __extension__ typedef long long            int64_t;
    __extension__ typedef unsigned long long   uint64_t;
#elif defined(_MSC_VER)
    // we have Borland/Intel/Microsoft __int64:
    typedef __int64             int64_t;
    typedef unsigned __int64    uint64_t;
#else // assume no 64-bit integers
# define LIBLAS_NO_INT64_T
#endif


#endif // LAS_CONFIG_H_INCLUDED

