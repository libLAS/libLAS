
#ifndef LIBLAS_H_INCLUDED
#define LIBLAS_H_INCLUDED



#include "las_version.h"
#include "las_config.h"


typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

LAS_C_START

#include <stdint.h>

LASReaderH LAS_DLL LASReader_Create(const char * filename);
void LASReader_Destroy(LASReaderH hReader);

LASHeaderH LAS_DLL LASReader_GetHeader(LASReaderH hReader);
void LASHeader_Destroy(LASHeaderH hHeader);

char* LASHeader_GetFileSignature(LASHeaderH hHeader);
uint16_t LASHeader_GetFileSourceId(LASHeaderH hHeader);


uint32_t LAS_DLL LASHeader_GetProjectId1(LASHeaderH hHeader);
uint16_t LASHeader_GetProjectId2(LASHeaderH hHeader);
uint16_t LASHeader_GetProjectId3(LASHeaderH hHeader);
char* LASHeader_GetProjectId4(LASHeaderH hHeader);

uint8_t LASHeader_GetVersionMajor(LASHeaderH hHeader);
uint8_t LASHeader_GetVersionMinor(LASHeaderH hHeader);
char* LASHeader_GetSystemId(LASHeaderH hHeader);
char* LASHeader_GetSoftwareId(LASHeaderH hHeader);

int16_t LASHeader_GetReserved(LASHeaderH hHeader);

int16_t LASHeader_GetCreationDOY(LASHeaderH hHeader);
int16_t LASHeader_GetCreationYear(LASHeaderH hHeader);

uint16_t LASHeader_GetHeaderSize(LASHeaderH hHeader);
uint32_t LASHeader_GetDataOffset(LASHeaderH hHeader);
uint32_t LASHeader_GetRecordsCount(LASHeaderH hHeader);
uint8_t LASHeader_GetDataFormatId(LASHeaderH hHeader);
uint16_t LASHeader_GetDataRecordLength(LASHeaderH hHeader);
uint8_t LASHeader_GetDataFormatId(LASHeaderH hHeader);
uint32_t LASHeader_GetPointRecordsCount(LASHeaderH hHeader);

LAS_C_END
#endif