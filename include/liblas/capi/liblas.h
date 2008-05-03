/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Main prototypes for the libLAS C API
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2008, Howard Butler
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/
 
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

typedef struct  {

    double t;
    double x, y, z;
    uint16_t intensity;
    uint8_t cls;
    int8_t scan_angle;
    uint8_t user_data;
    uint16_t retnum;
    uint16_t numret;
    uint16_t scandir;
    uint16_t fedge;
    long rgpsum;    
    int number_of_point_records;
    int number_of_points_by_return[8];
    int number_of_returns_of_given_pulse[8];
    int classification[32];
    int classification_synthetic;
    int classification_keypoint;
    int classification_withheld;
    LASPointH pmax;
    LASPointH pmin;
} LASPointSummary;


/// Fetches the compiled-in version string
/// @return the version string for this library
LAS_DLL const char* LAS_GetVersion();

/// Resets the error stack for the libLAS C API.  
LAS_DLL void LASError_Reset(void);

/// Pops the top error off of the error stack for the libLAS C API.
LAS_DLL void LASError_Pop(void);

/// Returns the error number of the last error on the error stack.
LAS_DLL LASError LASError_GetLastErrorNum(void);

/// Returns the error message of the last error on the error stack.
LAS_DLL char * LASError_GetLastErrorMsg(void);

/// Returns the name of the method the last error message happened in
LAS_DLL char * LASError_GetLastErrorMethod(void);

/// Returns the number of error messages on the error stack.
LAS_DLL int LASError_GetErrorCount(void);

/// Prints the last error message in the error stack to stderr.  If 
/// there is no error on the error stack, only the message is printed.
/// The function does not alter the error stack in any way.
/// @param message Message to include in the stderr output
LAS_DLL void LASError_Print(const char* message);

/// Creates a LASReaderH object that can be used to read LASHeaderH and 
/// LASPointH objects with.  The LASReaderH must not be created with a 
/// filename that is opened for read or write by any other API functions.
/// @return LASReaderH opaque pointer to a LASReader instance.
/// @param filename Filename to open for read 
LAS_DLL LASReaderH LASReader_Create(const char * filename);

LAS_DLL LASPointH LASReader_GetNextPoint(const LASReaderH hReader);
LAS_DLL LASPointH LASReader_GetPointAt(const LASReaderH hReader, uint32_t position);

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
LAS_DLL int8_t LASPoint_GetScanAngleRank(const LASPointH);
LAS_DLL LASError LASPoint_SetScanAngleRank(LASPointH hPoint, int8_t value);
LAS_DLL uint8_t LASPoint_GetUserData(const LASPointH);
LAS_DLL LASError LASPoint_SetUserData(LASPointH hPoint, uint8_t value);
LAS_DLL int LASPoint_Validate(LASPointH hPoint);
LAS_DLL int LASPoint_IsValid(LASPointH hPoint);

LAS_DLL LASPointH LASPoint_Create(void);
LAS_DLL LASPointH LASPoint_Copy(const LASPointH);
LAS_DLL void LASPoint_Destroy(LASPointH hPoint);

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
LAS_DLL LASError LASHeader_SetDataFormatId(const LASHeaderH hHeader, int value);
LAS_DLL uint32_t LASHeader_GetPointRecordsCount(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetPointRecordsCount(const LASHeaderH hHeader, uint32_t value);
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

