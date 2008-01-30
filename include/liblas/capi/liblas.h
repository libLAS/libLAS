
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



typedef enum
{
    LE_None = 0,
    LE_Debug = 1,
    LE_Warning = 2,
    LE_Failure = 3,
    LE_Fatal = 4
} LASError;

void LASError_Reset(void);
void LASError_Pop();
LASError LASError_GetLastErrorNum(void);
const char* LASError_GetLastErrorMsg(void);
const char* LASError_GetLastErrorMethod(void);
int LASError_GetErrorCount(void);

LASReaderH LAS_DLL LASReader_Create(const char * filename);
LASPointH LAS_DLL LASReader_GetNextPoint(LASReaderH hReader);
void LAS_DLL LASReader_Destroy(LASReaderH hReader);

double LAS_DLL LASPoint_GetX(LASPointH);
double LAS_DLL LASPoint_GetY(LASPointH);
double LAS_DLL LASPoint_GetZ(LASPointH);

int LAS_DLL LASHeader_Read(LASHeaderH hHeader);
LASHeaderH LAS_DLL LASReader_GetHeader(LASReaderH hReader);
void LAS_DLL LASHeader_Destroy(LASHeaderH hHeader);

char* LAS_DLL LASHeader_GetFileSignature(LASHeaderH hHeader);
uint16_t LAS_DLL LASHeader_GetFileSourceId(LASHeaderH hHeader);


char* LASHeader_GetProjectId(LASHeaderH hHeader);

uint8_t LAS_DLL LASHeader_GetVersionMajor(LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetVersionMajor(LASHeaderH hHeader, uint8_t value);
uint8_t LAS_DLL LASHeader_GetVersionMinor(LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetVersionMinor(LASHeaderH hHeader, uint8_t value);

char* LAS_DLL LASHeader_GetSystemId(LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetSystemId(LASHeaderH hHeader, const char* value);

char* LAS_DLL LASHeader_GetSoftwareId(LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetSoftwareId(LASHeaderH hHeader, const char* value);

int16_t LAS_DLL LASHeader_GetReserved(LASHeaderH hHeader);

int16_t LAS_DLL LASHeader_GetCreationDOY(LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetCreationDOY(LASHeaderH hHeader, uint16_t value);
int16_t LAS_DLL LASHeader_GetCreationYear(LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetCreationYear(LASHeaderH hHeader, uint16_t value);

uint16_t LAS_DLL LASHeader_GetHeaderSize(LASHeaderH hHeader);
uint32_t LAS_DLL LASHeader_GetDataOffset(LASHeaderH hHeader);
uint32_t LAS_DLL LASHeader_GetRecordsCount(LASHeaderH hHeader);
uint8_t LAS_DLL LASHeader_GetDataFormatId(LASHeaderH hHeader);
uint16_t LAS_DLL LASHeader_GetDataRecordLength(LASHeaderH hHeader);
uint8_t LAS_DLL LASHeader_GetDataFormatId(LASHeaderH hHeader);
uint32_t LAS_DLL LASHeader_GetPointRecordsCount(LASHeaderH hHeader);
uint32_t LAS_DLL LASHeader_GetPointRecordsByReturnCount(LASHeaderH hHeader, int index);

double LAS_DLL LASHeader_GetScaleX(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetScaleY(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetScaleZ(LASHeaderH hHeader);

double LAS_DLL LASHeader_GetOffsetX(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetOffsetY(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetOffsetZ(LASHeaderH hHeader);

double LAS_DLL LASHeader_GetMinX(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMinY(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMinZ(LASHeaderH hHeader);

double LAS_DLL LASHeader_GetMaxX(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMaxY(LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMaxZ(LASHeaderH hHeader);

LAS_C_END
#endif