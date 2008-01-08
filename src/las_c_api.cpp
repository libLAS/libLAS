#include <liblas/capi/liblas.h>

#include <liblas/lasreader.hpp>


#include <string>
#include <cstdio>

using namespace liblas;
LAS_C_START

LASReaderH LASReader_Create(const char* filename) 

{
    if (filename == NULL) {
        return NULL;
    }
    return (LASReaderH) new LASReader(std::string(filename));   
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

long LASHeader_GetFileSourceId(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetFileSourceId();
        return value;
    }
    else return 0;
}

long LASHeader_GetReserved(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetReserved();
        return value;
    }
    else return 0;
}

long long LASHeader_GetProjectId1(LASHeaderH hHeader) {
    if (hHeader){
        long long value = ((LASHeader*) hHeader)->GetProjectId1();
        return value;
    }
    else return 0;
}

long LASHeader_GetProjectId2(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetProjectId2();
        return value;
    }
    else return 0;
}

long LASHeader_GetProjectId3(LASHeaderH hHeader) {
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

long LASHeader_GetVersionMajor(LASHeaderH hHeader) {
    if (hHeader){
        long value = ((LASHeader*) hHeader)->GetVersionMajor();
        return value;
    }
    else return 0;
}

long LASHeader_GetVersionMinor(LASHeaderH hHeader) {
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

void LASHeader_Destroy(LASHeaderH hHeader)
{
    delete ((LASHeader*) hHeader);
}


LAS_C_END

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