// #include <liblas/capi/liblas.h>

#include <liblas/lasreader.hpp>
#include <liblas/laserror.hpp>

typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

#include <string>
#include <stack>
#include <map>
#include <iosfwd>
//#include <cstdio>
#include <exception>
#include <iostream>
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
    LASError err = LASError(code, message, method);
    errors.push(err);
}

LASReaderH LASReader_Create(const char* filename) 

{
    if (filename == NULL) {
        LASError_PushError(LE_Failure, "Inputted filename was null", "LASReader_Create");
        return NULL;
    }
    // try {
    //     
    // } catch (std::exception const& e) 
    // {
    //     
    // }
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
    delete ((LASReader*) hReader);
    g_ifs.close();
}


LASHeaderH LASReader_GetHeader(LASReaderH hReader)
{
    if (hReader) {
        LASHeader header = ((LASReader*) hReader)->GetHeader();
        return (LASHeaderH) new LASHeader( header );
        
    }
    else
        return NULL;
}


LASPointH LASReader_GetPoint(LASReaderH hReader)
{
    if (hReader) {
        LASReader *reader = ((LASReader*) hReader);
        if (reader->ReadNextPoint()) 
            return (LASPointH) new LASPoint(reader->GetPoint());
        else return NULL;
    }
    else
        return NULL;
}

double LASPoint_GetX(LASPointH hPoint) {
    
    if (hPoint){
        double value = ((LASPoint*) hPoint)->GetX();
        return value;
    }
    else return 0;
}

double LASPoint_GetY(LASPointH hPoint) {
    
    if (hPoint){
        double value = ((LASPoint*) hPoint)->GetY();
        return value;
    }
    else return 0;
}

double LASPoint_GetZ(LASPointH hPoint) {
    
    if (hPoint){
        double value = ((LASPoint*) hPoint)->GetZ();
        return value;
    }
    else return 0;
}




char* LASHeader_GetFileSignature(LASHeaderH hHeader) {
    // caller owns it
    if (hHeader){
        std::string signature = ((LASHeader*) hHeader)->GetFileSignature();
        char* value = (char*) malloc(signature.size() * sizeof(char*) + 1);
        strcpy(value, signature.c_str());
        return value;
    }
    else return NULL;
}

liblas::uint16_t LASHeader_GetFileSourceId(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetFileSourceId();
        return value;
    }
    else return 0;
}

liblas::uint16_t LASHeader_GetReserved(LASHeaderH hHeader) {
    if (hHeader){
        unsigned short value = ((LASHeader*) hHeader)->GetReserved();
        return value;
    }
    else return 0;
}

liblas::uint32_t LASHeader_GetProjectId1(LASHeaderH hHeader) {
    if (hHeader){
        long long value = ((LASHeader*) hHeader)->GetProjectId1();
        return value;
    }
    else return 0;
}

liblas::uint16_t LASHeader_GetProjectId2(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetProjectId2();
        return value;
    }
    else return 0;
}

liblas::uint16_t LASHeader_GetProjectId3(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetProjectId3();
        return value;
    }
    else return 0;
}

char* LASHeader_GetProjectId4(LASHeaderH hHeader) {
    // caller owns it
    if (hHeader){
        std::string projectid = ((LASHeader*) hHeader)->GetProjectId4();
        char* output = (char*) malloc(projectid.size() * sizeof(char*) + 1);
        strcpy(output, projectid.c_str());
        return output;
    }
    else return NULL;
}

liblas::uint8_t LASHeader_GetVersionMajor(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetVersionMajor();
        return value;
    }
    else return 0;
}

liblas::uint8_t LASHeader_GetVersionMinor(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetVersionMinor();
        return value;
    }
    else return 0;
}

char* LASHeader_GetSystemId(LASHeaderH hHeader) {
    // caller owns it
    if (hHeader){
        std::string sysid = ((LASHeader*) hHeader)->GetSystemId();
        char* value = (char*) malloc(sysid.size() * sizeof(char*) + 1);
        strcpy(value, sysid.c_str());
        return value;
    }
    else return NULL;
}

liblas::uint16_t LASHeader_GetCreationDOY(LASHeaderH hHeader) {
    if (hHeader){
        unsigned short value = ((LASHeader*) hHeader)->GetCreationDOY();
        return value;
    }
    else return 0;
}

liblas::uint16_t LASHeader_GetCreationYear(LASHeaderH hHeader) {
    if (hHeader){
        unsigned short value = ((LASHeader*) hHeader)->GetCreationYear();
        return value;
    }
    else return 0;
}

char* LASHeader_GetSoftwareId(LASHeaderH hHeader) {
    // caller owns it
    if (hHeader){
        std::string softid = ((LASHeader*) hHeader)->GetSoftwareId();
        char* value = (char*) malloc(softid.size() * sizeof(char*) + 1);
        strcpy(value, softid.c_str());
        return value;
    }
    else return NULL;
}

liblas::uint16_t LASHeader_GetHeaderSize(LASHeaderH hHeader) {
    if (hHeader){
        unsigned short value = ((LASHeader*) hHeader)->GetHeaderSize();
        return value;
    }
    else return 0;
}

liblas::uint32_t LASHeader_GetDataOffset(LASHeaderH hHeader) {
    if (hHeader){
        unsigned long value = ((LASHeader*) hHeader)->GetDataOffset();
        return value;
    }
    else return 0;
}




liblas::uint32_t LASHeader_GetRecordsCount(LASHeaderH hHeader) {
    if (hHeader){
        unsigned long value = ((LASHeader*) hHeader)->GetRecordsCount();
        return value;
    }
    else return 0;
}

liblas::uint8_t LASHeader_GetDataFormatId(LASHeaderH hHeader) {
    if (hHeader){
        unsigned char value = ((LASHeader*) hHeader)->GetDataFormatId();
        return value;
    }
    else return 0;
}

liblas::uint16_t LASHeader_GetDataRecordLength(LASHeaderH hHeader) {
    if (hHeader){
        unsigned short value = ((LASHeader*) hHeader)->GetDataRecordLength();
        return value;
    }
    else return 0;
}


liblas::uint32_t LASHeader_GetPointRecordsByReturnCount(LASHeaderH hHeader, int index) {
    if (hHeader){
        std::vector<liblas::uint32_t> counts  = ((LASHeader*) hHeader)->GetPointRecordsByReturnCount();
        if ( (index < 5) && (index >= 0)) {
            return counts[index];
        } else {
            return 0;
        }

    }
    return 0;
}

liblas::uint32_t LASHeader_GetPointRecordsCount(LASHeaderH hHeader) {
    if (hHeader){
        unsigned long value = ((LASHeader*) hHeader)->GetPointRecordsCount();
        return value;
    }
    else return 0;
}
double LASHeader_GetScaleX(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetScaleX();
        return value;
    }
    else return 0;
}

double LASHeader_GetScaleY(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetScaleY();
        return value;
    }
    else return 0;
}

double LASHeader_GetScaleZ(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetScaleZ();
        return value;
    }
    else return 0;
}

double LASHeader_GetOffsetX(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetOffsetX();
        return value;
    }
    else return 0;
}

double LASHeader_GetOffsetY(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetOffsetY();
        return value;
    }
    else return 0;
}

double LASHeader_GetOffsetZ(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetOffsetZ();
        return value;
    }
    else return 0;
}

double LASHeader_GetMinX(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetMinX();
        return value;
    }
    else return 0;
}

double LASHeader_GetMinY(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetMinY();
        return value;
    }
    else return 0;
}

double LASHeader_GetMinZ(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetMinZ();
        return value;
    }
    else return 0;
}

double LASHeader_GetMaxX(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetMaxX();
        return value;
    }
    else return 0;
}

double LASHeader_GetMaxY(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetMaxY();
        return value;
    }
    else return 0;
}

double LASHeader_GetMaxZ(LASHeaderH hHeader) {
    if (hHeader){
        double value = ((LASHeader*) hHeader)->GetMaxZ();
        return value;
    }
    else return 0;
}

void LASHeader_Destroy(LASHeaderH hHeader)
{
    delete ((LASHeader*) hHeader);
}


} // extern "C"
