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
typedef void *LASGuidH;

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


/** Returns the version string for this library.
 *  @return the version string for this library.
*/
LAS_DLL const char* LAS_GetVersion();

/****************************************************************************/
/* Error handling                                                           */
/****************************************************************************/

/** Resets the error stack for the libLAS C API.  
*/
LAS_DLL void LASError_Reset(void);

/** Pops the top error off of the error stack for the libLAS C API.
*/
LAS_DLL void LASError_Pop(void);

/** Returns the error number of the last error on the error stack.
 *  @return the error number of the last error on the error stack.
*/
LAS_DLL LASError LASError_GetLastErrorNum(void);

/** Returns the name of the method the last error message happened in.
 *  @return the name of the method the last error message happened in.
*/
LAS_DLL char * LASError_GetLastErrorMsg(void);

/** Returns the name of the method the last error message happened in.
 *  @return the name of the method the last error message happened in.
*/
LAS_DLL char * LASError_GetLastErrorMethod(void);

/** Returns the number of error messages on the error stack.
 *  @return the number of error messages on the error stack.
*/
LAS_DLL int LASError_GetErrorCount(void);

/** Prints the last error message in the error stack to stderr.  If 
 *  there is no error on the error stack, only the message is printed.
 *  The function does not alter the error stack in any way.
 *  @param message Message to include in the stderr output
*/
LAS_DLL void LASError_Print(const char* message);

/****************************************************************************/
/* Reader operations                                                        */
/****************************************************************************/

/** Creates a LASReaderH object that can be used to read LASHeaderH and 
 *  LASPointH objects with.  The LASReaderH must not be created with a 
 *  filename that is opened for read or write by any other API functions.
 *  @return opaque pointer to a LASReaderH instance.
 *  @param filename Filename to open for read 
*/
LAS_DLL LASReaderH LASReader_Create(const char * filename);

/** Reads the next available point on the LASReaderH instance.  If no point 
 *  is available to read, NULL is returned.  If an error happens during 
 *  the reading of the next available point, an error will be added to the 
 *  error stack and can be returned with the LASError_GetLastError* methods.
 *  @param hReader the opaque handle to the LASReaderH 
 *  @return an opaque handle to a LASPointH object, or NULL if no point is 
 *  available to read or an error occured.  Use the 
 *  LASError_GetLastError* methods to confirm the existence of an error 
 *  if NULL is returned.
*/
LAS_DLL LASPointH LASReader_GetNextPoint(const LASReaderH hReader);

/** Reads a LASPointH from the given position in the LAS file represented 
 *  by the LASReaderH instance.  If no point is available at that location, 
 *  NULL is returned.  If an error happens during the reading of the point, 
 *  an error will be added to the error stack and can be returned with the 
 *  LASError_GetLastError* methods.
 *  @param hReader the opaque handle to the LASReaderH
 *  @param position the integer position of the point in the file to read.
 *  @return an opaque handle to a LASPointH object, or NULL if no point is 
 *  available at the given location or an error occured.  Use the 
 *  LASError_GetLastError* methods to confirm the existence of an error 
 *  if NULL is returned.
*/
LAS_DLL LASPointH LASReader_GetPointAt(const LASReaderH hReader, uint32_t position);

/** Closes the file for reading operations represented by the LASReaderH instance.
 *  @param hReader the opqaue handle to the LASReaderH
*/
LAS_DLL void LASReader_Destroy(LASReaderH hReader);

/** Returns a LASHeaderH representing the header for the file
 *  @param hReader the LASReaderH instance
 *  @return a LASHeaderH representing the header for the file.  NULL is returned 
 *  in the event of an error.  Use the LASError_GetLastError* methods to check
 *  in the event of a NULL return.
*/
LAS_DLL LASHeaderH LASReader_GetHeader(const LASReaderH hReader);

/****************************************************************************/
/* Point operations                                                         */
/****************************************************************************/

/** Returns the X value for the point.  This value is not scaled or offset
 *  by any header values and stands on its own.  If you need points to have 
 *  a scale and/or offset applied, this must be done in conjunction with the 
 *  header values after the value is read.
 *  @param hPoint the opaque pointer to the LASPointH instance  
 *  @return the X value for the LASPointH
*/
LAS_DLL double LASPoint_GetX(const LASPointH hPoint);

/** Sets the X value for the point.  This value must be scaled or offset 
 *  by any header values before being set.
 *  @param hPoint the opaque pointer to the LASPointH instance
 *  @param value the double value to set for the X value of the point
 *  @return an error number if an error occured during the setting of the point.
*/
LAS_DLL LASError LASPoint_SetX(LASPointH hPoint, double value);

/** Returns the Y value for the point.  This value is not scaled or offset
 *  by any header values and stands on its own.  If you need points to have 
 *  a scale and/or offset applied, this must be done in conjunction with the 
 *  header values after the value is read.
 *  @param hPoint the opaque pointer to the LASPointH instance  
 *  @return the Y value for the LASPointH
*/
LAS_DLL double LASPoint_GetY(const LASPointH hPoint);

/** Sets the Y value for the point.  This value must be scaled or offset 
 *  by any header values before being set.
 *  @param hPoint the opaque pointer to the LASPointH instance
 *  @param value the double value to set for the Y value of the point
 *  @return an error number if an error occured during the setting of the point.
*/
LAS_DLL LASError LASPoint_SetY(LASPointH hPoint, double value);

/** Returns the Z value for the point.  This value is not scaled or offset
 *  by any header values and stands on its own.  If you need points to have 
 *  a scale and/or offset applied, this must be done in conjunction with the 
 *  header values after the value is read.
 *  @param hPoint the opaque pointer to the LASPointH instance  
 *  @return the Z value for the LASPointH
*/
LAS_DLL double LASPoint_GetZ(const LASPointH hPoint);

/** Sets the Z value for the point.  This value must be scaled or offset 
 *  by any header values before being set.
 *  @param hPoint the opaque pointer to the LASPointH instance
 *  @param value the double value to set for the Z value of the point
 *  @return an error number if an error occured during the setting of the point.
*/
LAS_DLL LASError LASPoint_SetZ(LASPointH hPoint, double value);

/** Returns the intensity value for the point.  This value is the pulse return 
 *  magnitude, it is optional, and it is LiDAR system specific.
 *  @return the intensity value for the point.
*/
LAS_DLL uint16_t LASPoint_GetIntensity(const LASPointH hPoint);

/** Sets the intensity value for the point.
 *  @param hPoint the opaque pointer to the LASPointH instance
 *  @param value the value to set the intensity to
 *  @return an error number if an error occured.
*/
LAS_DLL LASError LASPoint_SetIntensity(LASPointH hPoint, uint16_t value);

/** Returns the return number for the point.  The return number is "the pulse
 *  return number for a given output pulse."  The first return number starts with
 *  the value 1.
 *  @param hPoint LASPointH instance
 *  @return a return number, valid from 1-6, for the point.  Use the LASError 
 *  methods to determine if an error occurred during this operation if 0 
 *  is returned.
*/
LAS_DLL uint16_t LASPoint_GetReturnNumber(const LASPointH hPoint);

/** Sets the return number for the point.  Valid values are from 1-6.
 *  @param hPoint LASPointH instance
 *  @param value the value to set for the return number
 *  @return LASError value determine success or failure.
*/
LAS_DLL LASError LASPoint_SetReturnNumber(LASPointH hPoint, uint16_t value);

/** Returns the total number of returns for a given pulse.
 *  @param hPoint LASPointH instance
 *  @return total number of returns for this pulse.
*/
LAS_DLL uint16_t LASPoint_GetNumberOfReturns(const LASPointH hPoint);

/** Sets the number of returns for the point.  Valid values are from 1-5.
 *  @param hPoint LASPointH instance
 *  @param value the value to set for the number of returns
 *  @return LASError value determine success or failure.
*/
LAS_DLL LASError LASPoint_SetNumberOfReturns(LASPointH hPoint, uint16_t value);

/** Returns the scan direction for a given pulse.
 *  @param hPoint LASPointH instance
 *  @return the scan direction for a given pulse.
*/
LAS_DLL uint16_t LASPoint_GetScanDirection(const LASPointH hPoint);

/** Sets the scan direction for a given pulse.  Valid values are 0 or 1, with 
 *  1 being a positive scan direction and 0 being a negative scan direction.
 *  @param hPoint LASPointH instance
 *  @param value the value to set for scan direction
 *  @return LASError value determine success or failure.
*/
LAS_DLL LASError LASPoint_SetScanDirection(LASPointH hPoint, uint16_t value);

/** Returns whether or not a given pulse is an edge point
 *  @param hPoint LASPointH instance
 *  @return whether or not a given pulse is an edge point.
*/
LAS_DLL uint16_t LASPoint_GetFlightLineEdge(const LASPointH hPoint);

/** Sets the edge marker for a given pulse.  Valid values are 0 or 1, with 
 *  1 being an edge point and 0 being interior.
 *  @param hPoint LASPointH instance
 *  @param value the value to set for flightline edge
 *  @return LASError value determine success or failure.
*/
LAS_DLL LASError LASPoint_SetFlightLineEdge(LASPointH hPoint, uint16_t value);
LAS_DLL uint8_t LASPoint_GetScanFlags(const LASPointH hPoint);
LAS_DLL LASError LASPoint_SetScanFlags(LASPointH hPoint, uint8_t value);
LAS_DLL uint8_t LASPoint_GetClassification(const LASPointH hPoint);
LAS_DLL LASError LASPoint_SetClassification(LASPointH hPoint, uint8_t value);
LAS_DLL double LASPoint_GetTime(const LASPointH hPoint);
LAS_DLL LASError LASPoint_SetTime(LASPointH hPoint, double value);
LAS_DLL int8_t LASPoint_GetScanAngleRank(const LASPointH hPoint);
LAS_DLL LASError LASPoint_SetScanAngleRank(LASPointH hPoint, int8_t value);
LAS_DLL uint8_t LASPoint_GetUserData(const LASPointH hPoint);
LAS_DLL LASError LASPoint_SetUserData(LASPointH hPoint, uint8_t value);
LAS_DLL int LASPoint_Validate(LASPointH hPoint);
LAS_DLL int LASPoint_IsValid(LASPointH hPoint);

LAS_DLL LASPointH LASPoint_Create(void);
LAS_DLL LASPointH LASPoint_Copy(const LASPointH);
LAS_DLL void LASPoint_Destroy(LASPointH hPoint);


LAS_DLL void LASHeader_Destroy(LASHeaderH hHeader);
LAS_DLL LASHeaderH LASHeader_Copy(const LASHeaderH hHeader);
LAS_DLL LASHeaderH LASHeader_Create(void);

LAS_DLL char *LASHeader_GetFileSignature(const LASHeaderH hHeader);
LAS_DLL uint16_t LASHeader_GetFileSourceId(const LASHeaderH hHeader);


LAS_DLL char *LASHeader_GetProjectId(const LASHeaderH hHeader);
LAS_DLL LASError LASHeader_SetGUID(LASHeaderH hHeader, LASGuidH hId);

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

/****************************************************************************/
/* Writer Operations                                                        */
/****************************************************************************/

/** Creates a new LASWriterH for write operations on LAS files.  The file may 
 *  be opened in either LAS_MODE_APPEND or LAS_MODE_WRITE, but the file cannot 
 *  be open by another other operations (another LASReaderH or LASWriterH).  
 *  @param filename The filename to open to write
 *  @param hHeader an opaque pointer to a LASHeaderH that will be written to 
 *  the file as part of the opening for write operation.
 *  @param mode a mode value to denote whether to open for write or append 
 *  operations.  Valid values are LAS_MODE_APPEND and LAS_MODE_WRITE.
*/
LAS_DLL LASWriterH LASWriter_Create(const char* filename, const LASHeaderH hHeader, int mode);

/** Writes a point to the file.  The location of where the point is writen is 
 *  determined by the mode the file is opened in, and what the last operation was.  
 *  For example, if the file was opened for append, the next point would be written 
 *  at the end of the file.  Likewise, if the file is opened in write mode, even 
 *  if the file already existed, the next WritePoint operation will happen at the 
 *  end of the header and all of the existing points in the file will be overwritten.
 *  @param hWriter opaque pointer to the LASWriterH instance
 *  @param hPoint the opaque LASPointH pointer to write
 *  @return LE_None if no error occurred during the write operation.
*/
LAS_DLL LASError LASWriter_WritePoint(const LASWriterH hWriter, const LASPointH hPoint);

/** Overwrites the header for the file represented by the LASWriterH.  It does 
 *  not matter if the file is opened for append or for write.
 *  @param hWriter opaque pointer to the LASWriterH instance
 *  @param hHeader LASHeaderH instance to write into the file
 *  @return LE_None if no error occurred during the operation.
*/

LAS_DLL LASError LASWriter_WriteHeader(const LASWriterH hWriter, const LASHeaderH hHeader);

/** Destroys the LASWriterH instance, effectively closing the file and performing 
 *  housekeeping operations.
 *  @param hWriter LASWriterH instance to close
*/
LAS_DLL void LASWriter_Destroy(LASWriterH hWriter);

/****************************************************************************/
/* GUID Operations                                                          */
/****************************************************************************/

/** Returns the GUID value for the header as an opaque LASGuidH pointer.
 *  @param hHeader the opaque pointer to the LASHeaderH
 *  @return the GUID value for the header as an opaque LASGuidH pointer.
*/
LAS_DLL LASGuidH LASHeader_GetGUID(const LASHeaderH hHeader);

/** Returns a new random GUID.
 *  @return a new random GUID
*/
LAS_DLL LASGuidH LASGuid_Create();

/** Creates a new GUID opaque pointer using the given string.  
 *  @param string A GUID string in the form "00000000-0000-0000-0000-000000000000"
 *  An example GUID might be something like '8388F1B8-AA1B-4108-BCA3-6BC68E7B062E'
 *  @return the GUID value as an opaque LASGuidH pointer.
*/
LAS_DLL LASGuidH LASGuid_CreateFromString(const char* string);

/** Destroys a GUID opaque pointer and removes it from the heap
 *  @param hId the GUID value to destroy as an opaque LASGuidH pointer.
*/
LAS_DLL void LASGuid_Destroy(LASGuidH hId);

/** Determines if two GUIDs are equal.
 *  @param hId1 the first GUID
 *  @param hId2 the second GUID
 *  @return 0 if false, 1 if true.  Use the LASError_GetLastError* methods to 
 *  determine if an error occured during the operation of this function.
*/
LAS_DLL int LASGuid_Equals(LASGuidH hId1, LASGuidH hId2);

/** Returns a string representation of the GUID opqaue pointer.  The caller 
 *  owns the string.
 *  @param hId the LASGuidH pointer
 *  @return a string representation of the GUID opaque pointer.
*/
LAS_DLL char* LASGuid_AsString(LASGuidH hId);


LAS_C_END
#endif

