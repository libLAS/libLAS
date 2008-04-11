// $Id$
//
// (C) Copyright Howard Butler 2008
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//

#include <liblas/lasreader.hpp>
#include <liblas/laserror.hpp>
#include <liblas/laswriter.hpp>
#include <liblas/lasfile.hpp>
#include <liblas/guid.hpp>
#include <liblas/capi/las_config.h>

typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

#include <string>
#include <stack>
#include <map>
#include <cstdio>
#include <exception>

#include <iostream>
#include <fstream>
using namespace liblas;

LAS_C_START

#ifndef _WIN32
#include <stdint.h>
#endif

// Error stuff


typedef enum
{
    LE_None = 0,
    LE_Debug = 1,
    LE_Warning = 2,
    LE_Failure = 3,
    LE_Fatal = 4
} LASErrorEnum;



typedef std::map<std::string, LASFile> StrLASFileMap;
typedef std::map<std::string, LASFile>::const_iterator StrLASFileMapIt;

static StrLASFileMap files;
static std::stack<LASError > errors;

#define VALIDATE_POINTER0(ptr, func) \
   do { if( NULL == ptr ) \
      { \
        LASErrorEnum const ret = LE_Failure; \
        std::ostringstream msg; \
        msg << "Pointer \'" << #ptr << "\' is NULL in \'" << (func) <<"\'."; \
        std::string message(msg.str()); \
        LASError_PushError( ret, \
           message.c_str(), (func)); \
         return; }} while(0)

#define VALIDATE_POINTER1(ptr, func, rc) \
   do { if( NULL == ptr ) \
      { \
        LASErrorEnum const ret = LE_Failure; \
        std::ostringstream msg; \
        msg << "Pointer \'" << #ptr << "\' is NULL in \'" << (func) <<"\'."; \
        std::string message(msg.str()); \
        LASError_PushError( ret, \
           message.c_str(), (func)); \
        return (rc); }} while(0)

LAS_DLL void LASError_Reset(void) {
    if (errors.empty()) return;
    for (std::size_t i=0;i<errors.size();i++) errors.pop();
}

LAS_DLL void LASError_Pop(void) {
    if (errors.empty()) return;
    errors.pop();
}

LAS_DLL int LASError_GetLastErrorNum(void){
    if (errors.empty())
        return 0;
    else {
        LASError err = errors.top();
        return err.GetCode();
    }
}

LAS_DLL char* LASError_GetLastErrorMsg(void){
    if (errors.empty()) 
        return NULL;
    else {
        LASError err = errors.top();
        return strdup(err.GetMessage().c_str());
    }
}

LAS_DLL char* LASError_GetLastErrorMethod(void){
    if (errors.empty()) 
        return NULL;
    else {
        LASError err = errors.top();
        return strdup(err.GetMethod().c_str());
    }
}

LAS_DLL void LASError_PushError(int code, const char *message, const char *method) {
    LASError err = LASError(code, std::string(message), std::string(method));
    errors.push(err);
}

LAS_DLL int LASError_GetErrorCount(void) {
    return static_cast<int>(errors.size());
}

LAS_DLL LASReaderH LASReader_Create(const char* filename) 

{
    VALIDATE_POINTER1(filename, "LASReader_Create", NULL);

    try {
        StrLASFileMap::const_iterator p;
        
        p = files.find(filename);
        
        if (p==files.end()) {

            /* FIXME : not freed by LASReader_Destroy */
            LASFile lasfile(filename);
            files[filename] = lasfile;

            LASReader* reader = NULL;
            try {
                reader = &(lasfile.GetReader());
            }
            catch (...) {
                files.erase(filename);
                throw std::runtime_error("LASReader_Create rethrowing");
            }
            
            return (LASReaderH) reader;

        }
        LASError_PushError(LE_Failure, "not able to create map entry", "LASReader_Create");
        return NULL;
    
    } catch (std::exception const& e)
     {
         LASError_PushError(LE_Failure, e.what(), "LASReader_Create");
         return NULL;
     }

    
}

LAS_DLL void LASReader_Destroy(LASReaderH hReader)
{
    VALIDATE_POINTER0(hReader, "LASReader_Destroy");

    StrLASFileMap::iterator p;    
    LASReader* reader = (LASReader*)hReader;

    for (p=files.begin(); p!=files.end(); ++p) {
        LASFile f = p->second;
        

        try {
            
            LASReader& freader = f.GetReader();

            try {
                std::ifstream& a = static_cast<std::ifstream&>(freader.GetStream());
                std::ifstream& r = static_cast<std::ifstream&>(reader->GetStream());
                if (&a == &r) {
                    files.erase(p);
                    hReader = NULL;
                    return;
                }

        
            } catch (std::bad_cast const& e)
            {
                std::istream& a = static_cast<std::istream&>(freader.GetStream());
                std::istream& r = reader->GetStream();
                if (&a == &r) {
                    files.erase(p);
                    hReader = NULL;
                    return;
                }     

            } catch (std::exception const& e)
            {
                hReader=NULL;
                LASError_PushError(LE_Failure, e.what(), "LASReader_Destroy");
                return;
            }
    
        }  catch (std::runtime_error const& e) 
        {
            continue;
        }
    }

    hReader = NULL;
}



LAS_DLL const LASPointH LASReader_GetNextPoint(const LASReaderH hReader)
{
    VALIDATE_POINTER1(hReader, "LASReader_GetNextPoint", NULL);

    try {
        LASReader *reader = ((LASReader*) hReader);
        if (reader->ReadNextPoint()) 
            // return (LASPointH) new LASPoint(reader->GetPoint());
            return (LASPointH) &(reader->GetPoint());
        else 
            return NULL;
    } catch (std::out_of_range const& e) {
        return NULL;
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASReader_GetNextPoint");
        return NULL;
    }
 
    return NULL;

}

LAS_DLL const LASPointH LASReader_GetPointAt(const LASReaderH hReader, liblas::uint32_t position)
{
    VALIDATE_POINTER1(hReader, "LASReader_GetPointAt", NULL);

    try {
        LASReader *reader = ((LASReader*) hReader);
        if (reader->ReadPointAt((std::size_t) position)) 
            // return (LASPointH) new LASPoint(reader->GetPoint());
            return (LASPointH) &(reader->GetPoint());
        else 
            return NULL;
    } catch (std::out_of_range const& e) {
        return NULL;
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASReader_GetPointAt");
        return NULL;
    }
 
    return NULL;

}
LAS_DLL LASHeaderH LASReader_GetHeader(const LASReaderH hReader)
{
    VALIDATE_POINTER1(hReader, "LASReader_GetHeader", NULL);

    LASHeader header = ((LASReader*) hReader)->GetHeader();
    return (LASHeaderH) new LASHeader( header );
}

LAS_DLL LASHeaderH LASHeader_Create(void) {
        return (LASHeaderH) new LASHeader();
}

LAS_DLL LASPointH LASPoint_Create(void) {
        return (LASPointH) new LASPoint();
}

LAS_DLL LASPointH LASPoint_Copy(const LASPointH hPoint) {
        return (LASPointH) new LASPoint(*((LASPoint*) hPoint));
}

LAS_DLL void LASPoint_Destroy(LASPointH hPoint) {
    VALIDATE_POINTER0(hPoint, "LASPoint_GetX");
    delete (LASPoint*) hPoint;
    hPoint = NULL;
}

LAS_DLL double LASPoint_GetX(const LASPointH hPoint) {

    VALIDATE_POINTER1(hPoint, "LASPoint_GetX", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetX();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetX(LASPointH hPoint, double value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetX", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetX(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetX");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL double LASPoint_GetY(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetY", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetY();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetY(LASPointH hPoint, double value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetY", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetY(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetY");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL double LASPoint_GetZ(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetZ", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetZ();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetZ(LASPointH hPoint, double value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetZ", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetZ(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetZ");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint16_t LASPoint_GetIntensity(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetIntensity", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetIntensity();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetIntensity(LASPointH hPoint, liblas::uint16_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetIntensity", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetIntensity(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetIntensity");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint16_t LASPoint_GetReturnNumber(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetReturnNumber", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetReturnNumber();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetReturnNumber(LASPointH hPoint, liblas::uint16_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetReturnNumber", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetReturnNumber(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetReturnNumber");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint16_t LASPoint_GetNumberOfReturns(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetNumberOfReturns", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetNumberOfReturns();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetNumberOfReturns(LASPointH hPoint, liblas::uint16_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetNumberOfReturns", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetNumberOfReturns(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetNumberOfReturns");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint16_t LASPoint_GetScanDirection(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetScanDirection", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetScanDirection();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetScanDirection(LASPointH hPoint, liblas::uint16_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetScanDirection", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetScanDirection(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetScanDirection");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint16_t LASPoint_GetFlightLineEdge(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetFlightLineEdge", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetFlightLineEdge();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetFlightLineEdge(LASPointH hPoint, liblas::uint16_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetFlightLineEdge", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetFlightLineEdge(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetFlightLineEdge");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint8_t LASPoint_GetScanFlags(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetScanFlags", 0);
    
    liblas::uint8_t value = ((LASPoint*) hPoint)->GetScanFlags();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetScanFlags(LASPointH hPoint, liblas::uint8_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetScanFlags", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetScanFlags(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetScanFlags");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint8_t LASPoint_GetClassification(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetClassification", 0);
    
    liblas::uint8_t value = ((LASPoint*) hPoint)->GetClassification();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetClassification(LASPointH hPoint, liblas::uint8_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetClassification", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetClassification(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetClassification");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL LASErrorEnum LASPoint_SetTime(LASPointH hPoint, double value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetTime", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetTime(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetTime");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL double LASPoint_GetTime(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetTime", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetTime();
    return value;
}

LAS_DLL liblas::uint8_t LASPoint_GetScanAngleRank(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetScanAngleRank", 0);
    
    liblas::uint8_t value = ((LASPoint*) hPoint)->GetScanAngleRank();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetScanAngleRank(LASPointH hPoint, liblas::uint8_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetScanAngleRank", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetScanAngleRank(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetScanAngleRank");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL liblas::uint8_t LASPoint_GetUserData(const LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetUserData", 0);
    
    liblas::uint8_t value = ((LASPoint*) hPoint)->GetUserData();
    return value;
}

LAS_DLL LASErrorEnum LASPoint_SetUserData(LASPointH hPoint, liblas::uint8_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetUserData", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetUserData(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetUserData");
        return LE_Failure;
    }

    return LE_None;

}

LAS_DLL int LASPoint_Equal(const LASPointH hPoint1, const LASPointH hPoint2) {
    VALIDATE_POINTER1(hPoint1, "LASPoint_Equal", 0);
    VALIDATE_POINTER1(hPoint2, "LASPoint_Equal", 0);

    LASPoint* point1 = ((LASPoint*) hPoint1);
    LASPoint* point2 = ((LASPoint*) hPoint2);

    return (point1 == point2);

}

LAS_DLL char* LASHeader_GetFileSignature(const LASHeaderH hHeader) {
    // caller owns it
    VALIDATE_POINTER1(hHeader, "LASHeader_GetFileSignature", NULL);
    
    std::string signature = ((LASHeader*) hHeader)->GetFileSignature();
    return strdup(signature.c_str());
}

LAS_DLL liblas::uint16_t LASHeader_GetFileSourceId(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetFileSourceId", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetFileSourceId();
    return value;
}

LAS_DLL liblas::uint16_t LASHeader_GetReserved(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetReserved", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetReserved();
    return value;
}

LAS_DLL char* LASHeader_GetProjectId(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetProjectId", 0);
    
    liblas::guid id = ((LASHeader*) hHeader)->GetProjectId();
    return strdup(id.to_string().c_str());
}

LAS_DLL liblas::uint8_t LASHeader_GetVersionMajor(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetVersionMajor", 0);

    long value = ((LASHeader*) hHeader)->GetVersionMajor();
    return liblas::uint8_t(value);
}

LAS_DLL LASErrorEnum LASHeader_SetVersionMajor(LASHeaderH hHeader, liblas::uint8_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetVersionMajor", LE_Failure);

    try {
        ((LASHeader*) hHeader)->SetVersionMajor(value);    
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetVersionMajor");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL liblas::uint8_t LASHeader_GetVersionMinor(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetVersionMinor", 0);

    long value = ((LASHeader*) hHeader)->GetVersionMinor();
    return liblas::uint8_t(value);
}

LAS_DLL LASErrorEnum LASHeader_SetVersionMinor(LASHeaderH hHeader, liblas::uint8_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetVersionMinor", LE_Failure);

    // TODO: Maybe this should be a fatal error -- hobu
    try {
        ((LASHeader*) hHeader)->SetVersionMinor(value);    
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetVersionMinor");
        return LE_Failure;
    }
 
    return LE_None;
}

LAS_DLL char* LASHeader_GetSystemId(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetSystemId", NULL);

    // caller owns it
    std::string sysid = ((LASHeader*) hHeader)->GetSystemId();
    return strdup(sysid.c_str());
}

LAS_DLL LASErrorEnum LASHeader_SetSystemId(LASHeaderH hHeader, const char* value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetSystemId", LE_Failure); 

    try {
            ((LASHeader*) hHeader)->SetSystemId(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetSystemId");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL char* LASHeader_GetSoftwareId(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetSoftwareId", NULL);

    // caller owns it
    std::string softid = ((LASHeader*) hHeader)->GetSoftwareId();
    return strdup(softid.c_str());
}

LAS_DLL LASErrorEnum LASHeader_SetSoftwareId(LASHeaderH hHeader, const char* value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetSoftwareId", LE_Failure); 

    try {
            ((LASHeader*) hHeader)->SetSoftwareId(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetSoftwareId");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL liblas::uint16_t LASHeader_GetCreationDOY(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetCreationDOY", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetCreationDOY();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetCreationDOY(LASHeaderH hHeader, liblas::uint16_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetCreationDOY", LE_Failure);
    ((LASHeader*) hHeader)->SetCreationDOY(value);    
    return LE_None;
}

LAS_DLL liblas::uint16_t LASHeader_GetCreationYear(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetCreationYear", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetCreationYear();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetCreationYear(LASHeaderH hHeader, liblas::uint16_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetCreationYear", LE_Failure);
    ((LASHeader*) hHeader)->SetCreationYear(value);    
    return LE_None;
}

LAS_DLL liblas::uint16_t LASHeader_GetHeaderSize(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetHeaderSize", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetHeaderSize();
    return value;
}

LAS_DLL liblas::uint32_t LASHeader_GetDataOffset(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetDataOffset", 0);

    unsigned long value = ((LASHeader*) hHeader)->GetDataOffset();
    return value;
}




LAS_DLL liblas::uint32_t LASHeader_GetRecordsCount(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetRecordsCount", 0);

    unsigned long value = ((LASHeader*) hHeader)->GetRecordsCount();
    return value;
}

LAS_DLL liblas::uint8_t LASHeader_GetDataFormatId(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetDataFormatId", 0);

    unsigned char value = ((LASHeader*) hHeader)->GetDataFormatId();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetDataFormatId(LASHeaderH hHeader, int value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetDataFormatId", LE_Failure); 
    
    try {
            ((LASHeader*) hHeader)->SetDataFormatId((liblas::LASHeader::PointFormat)value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetDataFormatId");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL liblas::uint16_t LASHeader_GetDataRecordLength(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetDataRecordLength", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetDataRecordLength();
    return value;
}


LAS_DLL liblas::uint32_t LASHeader_GetPointRecordsByReturnCount(const LASHeaderH hHeader, int index) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetPointRecordsByReturnCount", 0);

    std::vector<liblas::uint32_t> counts  = ((LASHeader*) hHeader)->GetPointRecordsByReturnCount();
    if ( (index < 5) && (index >= 0)) {
        return counts[index];
    } 

    return 0;
    
}

LAS_DLL LASErrorEnum LASHeader_SetPointRecordsByReturnCount(const LASHeaderH hHeader, int index, liblas::uint32_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetPointRecordsByReturnCount", LE_Failure);

    try {
        ((LASHeader*) hHeader)->SetPointRecordsByReturnCount(index, value);    
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetPointRecordsByReturnCount");
        return LE_Failure;
    }
 
    return LE_None;    
}


LAS_DLL liblas::uint32_t LASHeader_GetPointRecordsCount(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetPointRecordsCount", 0);

    unsigned long value = ((LASHeader*) hHeader)->GetPointRecordsCount();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetPointRecordsCount(const LASHeaderH hHeader, liblas::uint32_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetPointRecordsCount", LE_Failure);

    try {
        ((LASHeader*) hHeader)->SetPointRecordsCount(value);    
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetPointRecordsCount");
        return LE_Failure;
    }
 
    return LE_None;    
}
LAS_DLL double LASHeader_GetScaleX(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetScaleX", 0.0);

    double value = ((LASHeader*) hHeader)->GetScaleX();
    return value;
}

LAS_DLL double LASHeader_GetScaleY(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetScaleY", 0.0);

    double value = ((LASHeader*) hHeader)->GetScaleY();
    return value;
}

LAS_DLL double LASHeader_GetScaleZ(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetScaleZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetScaleZ();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetScale(LASHeaderH hHeader, double x, double y, double z) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetScale", LE_Failure); 
    
    try {
            ((LASHeader*) hHeader)->SetScale(x,y,z);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetScale");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL double LASHeader_GetOffsetX(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetOffsetX", 0.0);

    double value = ((LASHeader*) hHeader)->GetOffsetX();
    return value;
}

LAS_DLL double LASHeader_GetOffsetY(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetOffsetY", 0.0);

    double value = ((LASHeader*) hHeader)->GetOffsetY();
    return value;
}

LAS_DLL double LASHeader_GetOffsetZ(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetOffsetZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetOffsetZ();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetOffset(LASHeaderH hHeader, double x, double y, double z) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetOffset", LE_Failure); 
    
    try {
            ((LASHeader*) hHeader)->SetOffset(x,y,z);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetOffset");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL double LASHeader_GetMinX(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMinX", 0.0);

    double value = ((LASHeader*) hHeader)->GetMinX();
    return value;
}

LAS_DLL double LASHeader_GetMinY(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMinY", 0.0);

    double value = ((LASHeader*) hHeader)->GetMinY();
    return value;
}

LAS_DLL double LASHeader_GetMinZ(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMinZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetMinZ();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetMin(LASHeaderH hHeader, double x, double y, double z) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetMin", LE_Failure); 
    
    try {
            ((LASHeader*) hHeader)->SetMin(x,y,z);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetMin");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL double LASHeader_GetMaxX(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMaxX", 0.0);

    double value = ((LASHeader*) hHeader)->GetMaxX();
    return value;
}

LAS_DLL double LASHeader_GetMaxY(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMaxY", 0.0);

    double value = ((LASHeader*) hHeader)->GetMaxY();
    return value;
}

LAS_DLL double LASHeader_GetMaxZ(const LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMaxZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetMaxZ();
    return value;
}

LAS_DLL LASErrorEnum LASHeader_SetMax(LASHeaderH hHeader, double x, double y, double z) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetMax", LE_Failure); 
    
    try {
            ((LASHeader*) hHeader)->SetMax(x,y,z);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetMax");
        return LE_Failure;
    }

    return LE_None;
}

LAS_DLL void LASHeader_Destroy(LASHeaderH hHeader)
{
    VALIDATE_POINTER0(hHeader, "LASHeader_Destroy");
    delete ((LASHeader*) hHeader);
    hHeader=NULL;
}

LAS_DLL LASHeaderH LASHeader_Copy(const LASHeaderH hHeader) {
        return (LASHeaderH) new LASHeader(*((LASHeader*) hHeader));
}

LAS_DLL int LASHeader_Equal(const LASHeaderH hHeader1, const LASHeaderH hHeader2) {
    VALIDATE_POINTER1(hHeader1, "LASHeader_Equal", 0);
    VALIDATE_POINTER1(hHeader2, "LASHeader_Equal", 0);

    LASHeader* header1 = ((LASHeader*) hHeader1);
    LASHeader* header2 = ((LASHeader*) hHeader2);

    return (header1 == header2);
}

LAS_DLL LASWriterH LASWriter_Create(const char* filename, const LASHeaderH hHeader, int mode) {
    VALIDATE_POINTER1(hHeader, "LASWriter_Create", NULL); 
    
    if (filename == NULL) {
        LASError_PushError(LE_Failure, "Inputted filename was null", "LASWriter_Create");
        return NULL;
    }
    try {

        StrLASFileMap::const_iterator p;
        
        p = files.find(filename);
        LASHeader* header = ((LASHeader*) hHeader);
        
        if (p==files.end()) {

            LASFile lasfile;

            lasfile = LASFile(filename, *header, (liblas::LASFile::Mode)mode);
            LASWriter* writer = NULL;
            try {
                writer = &(lasfile.GetWriter());
            }
            catch (...) {
              //  files.erase(filename);
                throw std::runtime_error("LASWriter_Create rethrowing");
            }

            files[filename] = lasfile;

            return (LASWriterH) writer;
        }
        
        LASError_PushError(LE_Failure, "not able to create map entry", "LASWriter_Create");
        return NULL;

    } catch (std::exception const& e)
     {
         LASError_PushError(LE_Failure, e.what(), "LASWriter_Create");
         return NULL;
     }
    
}

LAS_DLL LASErrorEnum LASWriter_WritePoint(const LASWriterH hWriter, const LASPointH hPoint) {

    VALIDATE_POINTER1(hPoint, "LASWriter_WritePoint", LE_Failure);

    try {
            ((LASWriter*) hWriter)->WritePoint(*((LASPoint*) hPoint));
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASWriter_WritePoint");
        return LE_Failure;
    }

    return LE_None;    
}

LAS_DLL LASErrorEnum LASWriter_WriteHeader(const LASWriterH hWriter, const LASHeaderH hHeader) {

    VALIDATE_POINTER1(hHeader, "LASWriter_WriteHeader", LE_Failure);
    VALIDATE_POINTER1(hWriter, "LASWriter_WriteHeader", LE_Failure);
    
    try {
            ((LASWriter*) hWriter)->WriteHeader(*((LASHeader*) hHeader));
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASWriter_WriteHeader");
        return LE_Failure;
    }

    return LE_None;    
}

LAS_DLL void LASWriter_Destroy(LASWriterH hWriter)
{
    VALIDATE_POINTER0(hWriter, "LASWriter_Destroy");

    StrLASFileMap::iterator p;  
      
    LASWriter* writer = (LASWriter*)hWriter;

    for (p=files.begin(); p!=files.end(); ++p) {

        LASFile f = p->second;

        try {
            
            LASWriter& fwriter = f.GetWriter();

            try {
                std::ofstream& a = static_cast<std::ofstream&>(fwriter.GetStream());
                std::ofstream& r = static_cast<std::ofstream&>(writer->GetStream());
                if (&a == &r) {
                    files.erase(p);
                    hWriter = NULL;
                    return;
                }
        
            } catch (std::bad_cast const& e)
            {
                std::ostream& a = static_cast<std::ostream&>(fwriter.GetStream());
                std::ostream& r = writer->GetStream();
                if (&a == &r) {
                    files.erase(p);
                    hWriter = NULL;
                    return;
                }

            } catch (std::exception const& e)
            {
                hWriter=NULL;
                LASError_PushError(LE_Failure, e.what(), "LASWriter_Destroy");
                return ;
            }
    
        }  catch (std::runtime_error const& e) 
        {
            continue;
        }


    }

    hWriter=NULL;
}

LAS_DLL void LASError_Print(const char* message) {

    char* errmsg= NULL;
    char* errmethod = NULL;
    errmsg = LASError_GetLastErrorMsg();
    errmethod = LASError_GetLastErrorMethod();
    if (LASError_GetErrorCount()) {
        fprintf(stdout, 
            "%s: %s (%d) from method %s\n",
            message,
            errmsg,
            LASError_GetLastErrorNum(),
            errmethod
        ); 
        if (errmsg) free(errmsg);
        if (errmethod) free(errmethod);
    } else {
        fprintf(stdout, 
            "You have encountered an error. '%s'\n",
            message
        );         
    }

}

LAS_DLL const char * LAS_GetVersion() {

    /* XXX - mloskot: I'd suggest to define PACKAGE_VERSION as static object
       and return safe const pointer, instead of newly allocated string. */

    /* FIXME - mloskot: Termporarily, I defined PACKAGE_VERSION
       in las_config.h to solve Visual C++ compilation  (Ticket #23) */

    return strdup(PACKAGE_VERSION);
}

LAS_C_END