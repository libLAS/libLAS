
#ifndef LIBLAS_H_INCLUDED
#define LIBLAS_H_INCLUDED

#include "las_version.h"
#include "las_config.h"
#include <cstdio>

typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

LAS_C_START

LASReaderH LAS_Reader_Create();
LASHeaderH LAS_Reader_Get_Header(LASReaderH hReader);
int LAS_Reader_Open(FILE* file, int bSkipHeader, int bSkipVariableHeader);
LASWriterH LAS_Writer_Create();

LAS_C_END
#endif