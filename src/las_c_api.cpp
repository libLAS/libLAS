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
        return (LASReaderH) new LASReader(std::string(filename));   
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


liblas::uint32_t LASHeader_GetPointRecordsCount(LASHeaderH hHeader) {
    if (hHeader){
        unsigned long value = ((LASHeader*) hHeader)->GetPointRecordsCount();
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