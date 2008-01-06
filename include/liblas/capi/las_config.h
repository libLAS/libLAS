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

#endif