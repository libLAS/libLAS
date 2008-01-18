// #include <liblas/capi/liblas.h>

#include <liblas/lasreader.hpp>


typedef void *LASWriterH;
typedef void *LASReaderH;
typedef void *LASPointH;
typedef void *LASHeaderH;

#include <string>
//#include <cstdio>
#include <exception>
#include <iostream>
using namespace liblas;

extern "C" {

#include <stdint.h>

LASReaderH LASReader_Create(const char* filename) 

{
    // try {
    std::ifstream strm(filename, std::ios::in | std::ios::binary);
    LASReader* reader = new LASReader(strm);
    // LASHeader header = reader->GetHeader();
    // header.Read(strm);
    return (LASReaderH) reader;
//        return (LASReaderH) new LASReader(strm);   
    // } catch (std::exception const& e)
    // {
    //     std::cout << "Error: " << e.what() << std::endl;
    //     return NULL;
    // }

    
}

void LASReader_Destroy(LASReaderH hReader)
{
    delete ((LASReader*) hReader);
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
        reader->ReadPoint();
        return (LASPointH) new LASPoint(reader->GetPoint());
        
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
    else return 0;
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

// 
// #include "lasdefinitions.h"
// #include "laswriter.h"
// #include "lasreader.h"
// #include "lasheader.h"
// 
// LASReaderH LAS_Reader_Create() 
// 
// {
//     return (LASReaderH) new LASreader();
//     
// }
// 

// 
// int LAS_Reader_Open(LASReaderH hReader, FILE* file, int bSkipHeader, int bSkipVariableHeader)
// {
//     bool output = false;
//     output = ((LASreader*)hReader)->open(file, bSkipHeader, bSkipVariableHeader);   
//     return output;
// }
// 
// 
// LASWriterH LAS_Writer_Create()
// {
//     return (LASWriterH) new LASwriter();
// }