// #include <liblas/capi/liblas.h>

#include <liblas/lasreader.hpp>
#include <liblas/laserror.hpp>
#include <liblas/guid.hpp>

typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

#include <string>
#include <stack>
#include <map>
//#include <cstdio>
#include <exception>
#include <fstream>
using namespace liblas;

extern "C" {

#include <stdint.h>

// Error stuff


typedef enum
{
    LE_None = 0,
    LE_Debug = 1,
    LE_Warning = 2,
    LE_Failure = 3,
    LE_Fatal = 4
} LASErrorEnum;

std::ifstream g_ifs;

std::stack<LASError > errors;

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

void LASError_Reset() {
    if (errors.empty()) return;
    for (std::size_t i=0;i<errors.size();i++) errors.pop();
}

void LASError_Pop() {
    if (errors.empty()) return;
    errors.pop();
}

int LASError_GetLastErrorNum(){
    if (errors.empty()) return 0;
    if (errors.size() > 0) {
        LASError err = errors.top();
        return err.GetCode();
    }
    return 0;
}

const char* LASError_GetLastErrorMsg(){
    if (errors.empty()) return NULL;
    if (errors.size() > 0) {
        LASError err = errors.top();
        return err.GetMessage().c_str();
    }
    return NULL;
}

const char* LASError_GetLastErrorMethod(){
    if (errors.empty()) return NULL;
    if (errors.size() > 0) {
        LASError err = errors.top();
        return err.GetMethod().c_str();
    }
    return NULL;    
}

void LASError_PushError(int code, const char *message, const char *method) {
    LASError err = LASError(code, std::string(message), std::string(method));
    errors.push(err);
}

int LASError_GetErrorCount(void) {
    return errors.size();
}
LASReaderH LASReader_Create(const char* filename) 

{
    if (filename == NULL) {
        LASError_PushError(LE_Failure, "Inputted filename was null", "LASReader_Create");
        return NULL;
    }
    try {
        g_ifs.open(filename, std::ios::in | std::ios::binary);
        LASReader* reader = new LASReader(g_ifs);
        return (LASReaderH) reader;
    } catch (std::exception const& e)
     {
         LASError_PushError(LE_Failure, e.what(), "LASReader_Create");
         return NULL;
     }

    
}

void LASReader_Destroy(LASReaderH hReader)
{
    VALIDATE_POINTER0(hReader, "LASReader_Destroy");
    
    delete ((LASReader*) hReader);
    g_ifs.close();
    hReader = NULL;
}


LASHeaderH LASReader_GetHeader(LASReaderH hReader)
{
    VALIDATE_POINTER1(hReader, "LASReader_GetHeader", NULL);

    LASHeader header = ((LASReader*) hReader)->GetHeader();
    return (LASHeaderH) new LASHeader( header );
}


const LASPointH LASReader_GetNextPoint(LASReaderH hReader)
{
    VALIDATE_POINTER1(hReader, "LASReader_GetNextPoint", NULL);

    try {
        LASReader *reader = ((LASReader*) hReader);
        if (reader->ReadNextPoint()) 
            // return (LASPointH) new LASPoint(reader->GetPoint());
            return (LASPointH) &(reader->GetPoint());
        else 
            return NULL;
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASReader_GetNextPoint");
        printf("cought exception...returning nulll\n.");
        return NULL;
    }
 
    return NULL;

}

LASPointH LASPoint_Create(void) {
        return (LASPointH) new LASPoint();
}

LASPointH LASPoint_Copy(LASPointH hPoint) {
        return (LASPointH) new LASPoint(*((LASPoint*) hPoint));
}

void LASPoint_Destroy(LASPointH hPoint) {
    VALIDATE_POINTER0(hPoint, "LASPoint_GetX");
    delete (LASPoint*) hPoint;
    hPoint = NULL;
}

double LASPoint_GetX(LASPointH hPoint) {

    VALIDATE_POINTER1(hPoint, "LASPoint_GetX", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetX();
    return value;
}

LASErrorEnum LASPoint_SetX(LASPointH hPoint, double value) {

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

double LASPoint_GetY(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetY", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetY();
    return value;
}

LASErrorEnum LASPoint_SetY(LASPointH hPoint, double value) {

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

double LASPoint_GetZ(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetZ", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetZ();
    return value;
}

LASErrorEnum LASPoint_SetZ(LASPointH hPoint, double value) {

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

liblas::uint16_t LASPoint_GetIntensity(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetIntensity", 0);
    
    double value = ((LASPoint*) hPoint)->GetIntensity();
    return value;
}

LASErrorEnum LASPoint_SetIntensity(LASPointH hPoint, liblas::uint16_t value) {

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

liblas::uint16_t LASPoint_GetReturnNumber(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetReturnNumber", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetReturnNumber();
    return value;
}

LASErrorEnum LASPoint_SetReturnNumber(LASPointH hPoint, liblas::uint16_t value) {

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

liblas::uint16_t LASPoint_GetNumberOfReturns(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetNumberOfReturns", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetNumberOfReturns();
    return value;
}

LASErrorEnum LASPoint_SetNumberOfReturns(LASPointH hPoint, liblas::uint16_t value) {

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

liblas::uint16_t LASPoint_GetScanDirection(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetScanDirection", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetScanDirection();
    return value;
}

LASErrorEnum LASPoint_SetScanDirection(LASPointH hPoint, liblas::uint16_t value) {

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

liblas::uint16_t LASPoint_GetFlightLineEdge(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetFlightLineEdge", 0);
    
    liblas::uint16_t value = ((LASPoint*) hPoint)->GetFlightLineEdge();
    return value;
}

LASErrorEnum LASPoint_SetFlightLineEdge(LASPointH hPoint, liblas::uint16_t value) {

    VALIDATE_POINTER1(hPoint, "LASPoint_SetFlightLineEdge", LE_Failure);

    try {
            ((LASPoint*) hPoint)->SetScanDirection(value);
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASPoint_SetFlightLineEdge");
        return LE_Failure;
    }

    return LE_None;

}
liblas::uint8_t LASPoint_GetScanFlags(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetScanFlags", 0);
    
    liblas::uint8_t value = ((LASPoint*) hPoint)->GetScanFlags();
    return value;
}

LASErrorEnum LASPoint_SetScanFlags(LASPointH hPoint, liblas::uint8_t value) {

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

liblas::uint8_t LASPoint_GetClassification(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetClassification", 0);
    
    liblas::uint8_t value = ((LASPoint*) hPoint)->GetClassification();
    return value;
}

LASErrorEnum LASPoint_SetClassification(LASPointH hPoint, liblas::uint8_t value) {

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

LASErrorEnum LASPoint_SetTime(LASPointH hPoint, double value) {

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

double LASPoint_GetTime(LASPointH hPoint) {
    
    VALIDATE_POINTER1(hPoint, "LASPoint_GetTime", 0.0);
    
    double value = ((LASPoint*) hPoint)->GetTime();
    return value;
}

char* LASHeader_GetFileSignature(LASHeaderH hHeader) {
    // caller owns it
    VALIDATE_POINTER1(hHeader, "LASHeader_GetFileSignature", NULL);
    
    std::string signature = ((LASHeader*) hHeader)->GetFileSignature();
    char* value = (char*) malloc(signature.size() * sizeof(char*) + 1);
    strcpy(value, signature.c_str());
    return value;
}

liblas::uint16_t LASHeader_GetFileSourceId(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetFileSourceId", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetFileSourceId();
    return value;
}

liblas::uint16_t LASHeader_GetReserved(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetReserved", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetReserved();
    return value;
}

char* LASHeader_GetProjectId(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetProjectId", 0);
    
    liblas::guid id = ((LASHeader*) hHeader)->GetProjectId();
    char* output = (char*) malloc(id.to_string().size() * sizeof(char*) + 1);
    strcpy(output, id.to_string().c_str());
    return output;
}

liblas::uint8_t LASHeader_GetVersionMajor(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetVersionMajor", 0);

    long value = ((LASHeader*) hHeader)->GetVersionMajor();
    return value;
}

LASErrorEnum LASHeader_SetVersionMajor(LASHeaderH hHeader, liblas::uint8_t value) {
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

liblas::uint8_t LASHeader_GetVersionMinor(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetVersionMinor", 0);

    long value = ((LASHeader*) hHeader)->GetVersionMinor();
    return value;
}

LASErrorEnum LASHeader_SetVersionMinor(LASHeaderH hHeader, liblas::uint8_t value) {
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

char* LASHeader_GetSystemId(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetSystemId", NULL);

    // caller owns it
    std::string sysid = ((LASHeader*) hHeader)->GetSystemId();
    char* value = (char*) malloc(sysid.size() * sizeof(char*) + 1);
    strcpy(value, sysid.c_str());
    return value;
}

LASErrorEnum LASHeader_SetSystemId(LASHeaderH hHeader, const char* value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetSystemId", LE_Failure); 

    try {
            ((LASHeader*) hHeader)->SetSystemId(std::string(value));
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetSystemId");
        return LE_Failure;
    }

    return LE_None;
}

char* LASHeader_GetSoftwareId(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetSoftwareId", NULL);

    // caller owns it
    std::string softid = ((LASHeader*) hHeader)->GetSoftwareId();
    char* value = (char*) malloc(softid.size() * sizeof(char*) + 1);
    strcpy(value, softid.c_str());
    return value;
}

LASErrorEnum LASHeader_SetSoftwareId(LASHeaderH hHeader, const char* value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetSoftwareId", LE_Failure); 

    try {
            ((LASHeader*) hHeader)->SetSoftwareId(std::string(value));
    } catch (std::exception const& e)
    {
        LASError_PushError(LE_Failure, e.what(), "LASHeader_SetSoftwareId");
        return LE_Failure;
    }

    return LE_None;
}

liblas::uint16_t LASHeader_GetCreationDOY(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetCreationDOY", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetCreationDOY();
    return value;
}

LASErrorEnum LASHeader_SetCreationDOY(LASHeaderH hHeader, liblas::uint16_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetCreationDOY", LE_Failure);
    ((LASHeader*) hHeader)->SetCreationDOY(value);    
    return LE_None;
}

liblas::uint16_t LASHeader_GetCreationYear(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetCreationYear", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetCreationYear();
    return value;
}

LASErrorEnum LASHeader_SetCreationYear(LASHeaderH hHeader, liblas::uint16_t value) {
    VALIDATE_POINTER1(hHeader, "LASHeader_SetCreationYear", LE_Failure);
    ((LASHeader*) hHeader)->SetCreationYear(value);    
    return LE_None;
}

liblas::uint16_t LASHeader_GetHeaderSize(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetHeaderSize", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetHeaderSize();
    return value;
}

liblas::uint32_t LASHeader_GetDataOffset(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetDataOffset", 0);

    unsigned long value = ((LASHeader*) hHeader)->GetDataOffset();
    return value;
}




liblas::uint32_t LASHeader_GetRecordsCount(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetRecordsCount", 0);

    unsigned long value = ((LASHeader*) hHeader)->GetRecordsCount();
    return value;
}

liblas::uint8_t LASHeader_GetDataFormatId(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetDataFormatId", 0);

    unsigned char value = ((LASHeader*) hHeader)->GetDataFormatId();
    return value;
}

LASErrorEnum LASHeader_SetDataFormatId(LASHeaderH hHeader, int value) {
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

liblas::uint16_t LASHeader_GetDataRecordLength(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetDataRecordLength", 0);

    unsigned short value = ((LASHeader*) hHeader)->GetDataRecordLength();
    return value;
}


liblas::uint32_t LASHeader_GetPointRecordsByReturnCount(LASHeaderH hHeader, int index) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetPointRecordsByReturnCount", 0);

    std::vector<liblas::uint32_t> counts  = ((LASHeader*) hHeader)->GetPointRecordsByReturnCount();
    if ( (index < 5) && (index >= 0)) {
        return counts[index];
    } 

    return 0;
    
}

liblas::uint32_t LASHeader_GetPointRecordsCount(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetPointRecordsCount", 0);

    unsigned long value = ((LASHeader*) hHeader)->GetPointRecordsCount();
    return value;
}
double LASHeader_GetScaleX(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetScaleX", 0.0);

    double value = ((LASHeader*) hHeader)->GetScaleX();
    return value;
}

double LASHeader_GetScaleY(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetScaleY", 0.0);

    double value = ((LASHeader*) hHeader)->GetScaleY();
    return value;
}

double LASHeader_GetScaleZ(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetScaleZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetScaleZ();
    return value;
}

double LASHeader_GetOffsetX(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetOffsetX", 0.0);

    double value = ((LASHeader*) hHeader)->GetOffsetX();
    return value;
}

double LASHeader_GetOffsetY(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetOffsetY", 0.0);

    double value = ((LASHeader*) hHeader)->GetOffsetY();
    return value;
}

double LASHeader_GetOffsetZ(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetOffsetZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetOffsetZ();
    return value;
}

double LASHeader_GetMinX(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMinX", 0.0);

    double value = ((LASHeader*) hHeader)->GetMinX();
    return value;
}

double LASHeader_GetMinY(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMinY", 0.0);

    double value = ((LASHeader*) hHeader)->GetMinY();
    return value;
}

double LASHeader_GetMinZ(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMinZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetMinZ();
    return value;
}

double LASHeader_GetMaxX(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMaxX", 0.0);

    double value = ((LASHeader*) hHeader)->GetMaxX();
    return value;
}

double LASHeader_GetMaxY(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMaxY", 0.0);

    double value = ((LASHeader*) hHeader)->GetMaxY();
    return value;
}

double LASHeader_GetMaxZ(LASHeaderH hHeader) {
    VALIDATE_POINTER1(hHeader, "LASHeader_GetMaxZ", 0.0);

    double value = ((LASHeader*) hHeader)->GetMaxZ();
    return value;
}

void LASHeader_Destroy(LASHeaderH hHeader)
{
    VALIDATE_POINTER0(hHeader, "LASHeader_Destroy");
    delete ((LASHeader*) hHeader);
    hHeader=NULL;
}


} // extern "C"
