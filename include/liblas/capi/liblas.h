
#ifndef LIBLAS_H_INCLUDED
#define LIBLAS_H_INCLUDED



#define LIBLAS_C_API 1

#include "las_version.h"
#include "las_config.h"


#include <liblas/cstdint.hpp>


typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

LAS_C_START


/*#ifndef _WIN32
#include <stdint.h>
#endif
*/

#define LAS_MODE_READ 0
#define LAS_MODE_WRITE 1
#define LAS_MODE_APPEND 2

typedef enum
{
    LE_None = 0,
    LE_Debug = 1,
    LE_Warning = 2,
    LE_Failure = 3,
    LE_Fatal = 4
} LASError;

LAS_DLL void LASError_Reset(void);
LAS_DLL void LASError_Pop(void);
LAS_DLL LASError LASError_GetLastErrorNum(void);
LAS_DLL const char * LASError_GetLastErrorMsg(void);
LAS_DLL const char * LASError_GetLastErrorMethod(void);
LAS_DLL int LASError_GetErrorCount(void);

LAS_DLL LASReaderH LASReader_Create(const char * filename);
LAS_DLL LASPointH LASReader_GetNextPoint(const LASReaderH hReader);

LAS_DLL void LASReader_Destroy(LASReaderH hReader);

LAS_DLL double LASPoint_GetX(const LASPointH);
LAS_DLL LASError LASPoint_SetX(LASPointH hPoint, double value);
LAS_DLL double LASPoint_GetY(const LASPointH);
LAS_DLL LASError LASPoint_SetY(LASPointH hPoint, double value);
LAS_DLL double LASPoint_GetZ(const LASPointH);
LAS_DLL LASError LASPoint_SetZ(LASPointH hPoint, double value);

LAS_DLL uint16_t LASPoint_GetIntensity(const LASPointH);
LAS_DLL LASError LASPoint_SetIntensity(LASPointH hPoint, uint16_t value);

LAS_DLL uint16_t LASPoint_GetReturnNumber(const LASPointH);
LAS_DLL LASError LASPoint_SetReturnNumber(LASPointH hPoint, uint16_t value);
LAS_DLL uint16_t LASPoint_GetNumberOfReturns(const LASPointH);
LAS_DLL LASError LASPoint_SetNumberOfReturns(LASPointH hPoint, uint16_t value);
LAS_DLL uint16_t LASPoint_GetScanDirection(const LASPointH);
LAS_DLL LASError LASPoint_SetScanDirection(LASPointH hPoint, uint16_t value);
LAS_DLL uint16_t LASPoint_GetFlightLineEdge(const LASPointH);
LAS_DLL LASError LASPoint_SetFlightLineEdge(LASPointH hPoint, uint16_t value);
LAS_DLL uint8_t LASPoint_GetScanFlags(const LASPointH);
LAS_DLL LASError LASPoint_SetScanFlags(LASPointH hPoint, uint8_t value);
LAS_DLL uint8_t LASPoint_GetClassification(const LASPointH);
LAS_DLL LASError LASPoint_SetClassification(LASPointH hPoint, uint8_t value);
LAS_DLL double LASPoint_GetTime(const LASPointH);
LAS_DLL LASError LASPoint_SetTime(LASPointH hPoint, double value);
LAS_DLL uint8_t LASPoint_GetScanAngleRank(const LASPointH);
LAS_DLL LASError LASPoint_SetScanAngleRank(LASPointH hPoint, uint8_t value);
LAS_DLL uint8_t LASPoint_GetUserData(const LASPointH);
LAS_DLL LASError LASPoint_SetUserData(LASPointH hPoint, uint8_t value);


LAS_DLL LASPointH LASPoint_Create(void);
LAS_DLL LASPointH LASPoint_Copy(const LASPointH);
LAS_DLL void LASPoint_Destroy(LASPointH hPoint);

LAS_DLL int LASHeader_Read(LASHeaderH hHeader);
LAS_DLL LASHeaderH LASReader_GetHeader(const LASReaderH hReader);
LAS_DLL void LASHeader_Destroy(LASHeaderH hHeader);
LAS_DLL LASHeaderH LASHeader_Copy(const LASHeaderH hHeader);
LAS_DLL LASHeaderH LASHeader_Create(void);

LAS_DLL char *LASHeader_GetFileSignature(const LASHeaderH hHeader);
LAS_DLL uint16_t LASHeader_GetFileSourceId(const LASHeaderH hHeader);


LAS_DLL char *LASHeader_GetProjectId(const LASHeaderH hHeader);

LAS_DLL uint8_t LASHeader_GetVersionMajor(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetVersionMajor(LASHeaderH hHeader, uint8_t value);
LAS_DLL uint8_t LASHeader_GetVersionMinor(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetVersionMinor(LASHeaderH hHeader, uint8_t value);

LAS_DLL char *LASHeader_GetSystemId(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetSystemId(LASHeaderH hHeader, const char* value);

LAS_DLL char *LASHeader_GetSoftwareId(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetSoftwareId(LASHeaderH hHeader, const char* value);

LAS_DLL int16_t LASHeader_GetReserved(const LASHeaderH hHeader);

LAS_DLL int16_t LASHeader_GetCreationDOY(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetCreationDOY(LASHeaderH hHeader, uint16_t value);
LAS_DLL int16_t LASHeader_GetCreationYear(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetCreationYear(LASHeaderH hHeader, uint16_t value);


LAS_DLL uint16_t LASHeader_GetHeaderSize(const LASHeaderH hHeader);
LAS_DLL uint32_t LASHeader_GetDataOffset(const LASHeaderH hHeader);
LAS_DLL uint32_t LASHeader_GetRecordsCount(const LASHeaderH hHeader);
LAS_DLL uint8_t LASHeader_GetDataFormatId(const LASHeaderH hHeader);
LAS_DLL uint16_t LASHeader_GetDataRecordLength(const LASHeaderH hHeader);
LAS_DLL uint8_t LASHeader_GetDataFormatId(const LASHeaderH hHeader);
LAS_DLL uint32_t LASHeader_GetPointRecordsCount(const LASHeaderH hHeader);
LAS_DLL void LASHeader_SetPointRecordsCount(const LASHeaderH hHeader, uint32_t value);
LAS_DLL uint32_t LASHeader_GetPointRecordsByReturnCount(const LASHeaderH hHeader, int index);
LAS_DLL LASError LASHeader_SetPointRecordsByReturnCount(const LASHeaderH hHeader, int index, uint32_t value);

LAS_DLL double LASHeader_GetScaleX(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetScaleY(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetScaleZ(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetScale(LASHeaderH hHeader, double x, double y, double z);

LAS_DLL double LASHeader_GetOffsetX(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetOffsetY(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetOffsetZ(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetOffset(LASHeaderH hHeader, double x, double y, double z);

LAS_DLL double LASHeader_GetMinX(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetMinY(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetMinZ(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetMin(LASHeaderH hHeader, double x, double y, double z);

LAS_DLL double LASHeader_GetMaxX(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetMaxY(const LASHeaderH hHeader);
LAS_DLL double LASHeader_GetMaxZ(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetMax(LASHeaderH hHeader, double x, double y, double z);

LAS_DLL LASWriterH LASWriter_Create(const char* filename, const LASHeaderH hHeader, int mode);
LAS_DLL LASError LASWriter_WritePoint(const LASWriterH hWriter, const LASPointH hPoint);
LAS_DLL LASError LASWriter_WriteHeader(const LASWriterH hWriter, const LASHeaderH hHeader);
LAS_DLL void LASWriter_Destroy(LASWriterH hWriter);

LAS_C_END
#endif

