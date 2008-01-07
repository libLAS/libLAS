
#ifndef LIBLAS_H_INCLUDED
#define LIBLAS_H_INCLUDED

#include "las_version.h"
#include "las_config.h"

typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

LAS_C_START

LASReaderH LAS_DLL LASReader_Create(const char * filename);
LASHeaderH LAS_DLL LASReader_GetHeader(LASReaderH hReader);

//LASWriterH LAS_Writer_Create();

LAS_C_END
#endif