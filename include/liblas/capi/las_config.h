#ifndef LAS_CONFIG_H_INCLUDED
#define LAS_CONFIG_H_INCLUDED

#define LIBLAS_C_API 1

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

/* FIXME -  Ticket #23 */
#ifdef _MSC_VER
#define PACKAGE_VERSION "0.9.3"
#endif

#endif /* LAS_CONFIG_H_INCLUDED */

