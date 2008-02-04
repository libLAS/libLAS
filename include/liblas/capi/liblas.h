
#ifndef LIBLAS_H_INCLUDED
#define LIBLAS_H_INCLUDED



#include "las_version.h"
#include "las_config.h"

#define LIBLAS_C_API
#include <liblas/cstdint.hpp>


typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

LAS_C_START

#ifndef _WIN32
#include <stdint.h>
#endif


typedef enum
{
    LE_None = 0,
    LE_Debug = 1,
    LE_Warning = 2,
    LE_Failure = 3,
    LE_Fatal = 4
} LASError;

void LASError_Reset(void);
void LASError_Pop(void);
LASError LASError_GetLastErrorNum(void);
const char* LASError_GetLastErrorMsg(void);
const char* LASError_GetLastErrorMethod(void);
int LASError_GetErrorCount(void);

LASReaderH LAS_DLL LASReader_Create(const char * filename);
LASPointH LAS_DLL LASReader_GetNextPoint(const LASReaderH hReader);

void LAS_DLL LASReader_Destroy(LASReaderH hReader);

double LAS_DLL LASPoint_GetX(const LASPointH);
LASError LASPoint_SetX(LASPointH hPoint, double value);
double LAS_DLL LASPoint_GetY(const LASPointH);
LASError LASPoint_SetY(LASPointH hPoint, double value);
double LAS_DLL LASPoint_GetZ(const LASPointH);
LASError LASPoint_SetZ(LASPointH hPoint, double value);

uint16_t LAS_DLL LASPoint_GetIntensity(const LASPointH);
LASError LASPoint_SetIntensity(LASPointH hPoint, uint16_t value);

uint16_t LAS_DLL LASPoint_GetReturnNumber(const LASPointH);
LASError LASPoint_SetReturnNumber(LASPointH hPoint, uint16_t value);
uint16_t LAS_DLL LASPoint_GetNumberOfReturns(const LASPointH);
LASError LASPoint_SetNumberOfReturns(LASPointH hPoint, uint16_t value);
uint16_t LAS_DLL LASPoint_GetScanDirection(const LASPointH);
LASError LASPoint_SetScanDirection(LASPointH hPoint, uint16_t value);
uint16_t LAS_DLL LASPoint_GetFlightLineEdge(const LASPointH);
LASError LASPoint_SetFlightLineEdge(LASPointH hPoint, uint16_t value);
uint8_t LAS_DLL LASPoint_GetScanFlags(const LASPointH);
LASError LASPoint_SetScanFlags(LASPointH hPoint, uint8_t value);
uint8_t LAS_DLL LASPoint_GetClassification(const LASPointH);
LASError LASPoint_SetClassification(LASPointH hPoint, uint8_t value);
double LAS_DLL LASPoint_GetTime(const LASPointH);
LASError LASPoint_SetTime(LASPointH hPoint, double value);
uint8_t LAS_DLL LASPoint_GetScanAngleRank(const LASPointH);
LASError LASPoint_SetScanAngleRank(LASPointH hPoint, uint8_t value);
uint8_t LAS_DLL LASPoint_GetUserData(const LASPointH);
LASError LASPoint_SetUserData(LASPointH hPoint, uint8_t value);


LASPointH LAS_DLL LASPoint_Create(void);
LASPointH LASPoint_Copy(const LASPointH);
void LASPoint_Destroy(LASPointH hPoint);

int LAS_DLL LASHeader_Read(LASHeaderH hHeader);
LASHeaderH LAS_DLL LASReader_GetHeader(const LASReaderH hReader);
void LAS_DLL LASHeader_Destroy(LASHeaderH hHeader);
LASHeaderH LASHeader_Copy(const LASHeaderH hHeader);
LASHeaderH LASHeader_Create(void);

char* LASHeader_GetFileSignature(const LASHeaderH hHeader);
uint16_t LAS_DLL LASHeader_GetFileSourceId(const LASHeaderH hHeader);


char* LASHeader_GetProjectId(const LASHeaderH hHeader);

uint8_t LAS_DLL LASHeader_GetVersionMajor(const LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetVersionMajor(LASHeaderH hHeader, uint8_t value);
uint8_t LAS_DLL LASHeader_GetVersionMinor(const LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetVersionMinor(LASHeaderH hHeader, uint8_t value);

char* LASHeader_GetSystemId(const LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetSystemId(LASHeaderH hHeader, const char* value);

char* LASHeader_GetSoftwareId(const LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetSoftwareId(LASHeaderH hHeader, const char* value);

int16_t LAS_DLL LASHeader_GetReserved(const LASHeaderH hHeader);

int16_t LAS_DLL LASHeader_GetCreationDOY(const LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetCreationDOY(LASHeaderH hHeader, uint16_t value);
int16_t LAS_DLL LASHeader_GetCreationYear(const LASHeaderH hHeader);
LASError LAS_DLL LASHeader_SetCreationYear(LASHeaderH hHeader, uint16_t value);


uint16_t LAS_DLL LASHeader_GetHeaderSize(const LASHeaderH hHeader);
uint32_t LAS_DLL LASHeader_GetDataOffset(const LASHeaderH hHeader);
uint32_t LAS_DLL LASHeader_GetRecordsCount(const LASHeaderH hHeader);
uint8_t LAS_DLL LASHeader_GetDataFormatId(const LASHeaderH hHeader);
uint16_t LAS_DLL LASHeader_GetDataRecordLength(const LASHeaderH hHeader);
uint8_t LAS_DLL LASHeader_GetDataFormatId(const LASHeaderH hHeader);
uint32_t LAS_DLL LASHeader_GetPointRecordsCount(const LASHeaderH hHeader);
void LASHeader_SetPointRecordsCount(const LASHeaderH hHeader, uint32_t value);
uint32_t LAS_DLL LASHeader_GetPointRecordsByReturnCount(const LASHeaderH hHeader, int index);
LASError LASHeader_SetPointRecordsByReturnCount(const LASHeaderH hHeader, int index, uint32_t value);

double LAS_DLL LASHeader_GetScaleX(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetScaleY(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetScaleZ(const LASHeaderH hHeader);
LASError LASHeader_SetScale(LASHeaderH hHeader, double x, double y, double z);

double LAS_DLL LASHeader_GetOffsetX(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetOffsetY(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetOffsetZ(const LASHeaderH hHeader);
LASError LASHeader_SetOffset(LASHeaderH hHeader, double x, double y, double z);

double LAS_DLL LASHeader_GetMinX(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMinY(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMinZ(const LASHeaderH hHeader);
LASError LASHeader_SetMin(LASHeaderH hHeader, double x, double y, double z);

double LAS_DLL LASHeader_GetMaxX(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMaxY(const LASHeaderH hHeader);
double LAS_DLL LASHeader_GetMaxZ(const LASHeaderH hHeader);
LASError LASHeader_SetMax(LASHeaderH hHeader, double x, double y, double z);

LASWriterH LASWriter_Create(const char* filename, const LASHeaderH hHeader);
LASError LASWriter_WritePoint(const LASWriterH hWriter, const LASPointH hPoint);
void LASWriter_Destroy(LASWriterH hWriter);

LAS_C_END
#endif