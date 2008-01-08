#include <liblas/capi/liblas.h>

#include <liblas/lasreader.hpp>


#include <string>
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
        char* output = (char*) malloc(signature.size() * sizeof(char*) + 1);
        strcpy(output, signature.c_str());
        return output;
    }
    else return NULL;
}

long LASHeader_GetFileSourceId(LASHeaderH hHeader) {
    if (hHeader){
        long fileid = ((LASHeader*) hHeader)->GetFileSourceId();
        return fileid;
    }
    else return 0;
}

long LASHeader_GetReserved(LASHeaderH hHeader) {
    if (hHeader){
        long fileid = ((LASHeader*) hHeader)->GetReserved();
        return fileid;
    }
    else return 0;
}

long long LASHeader_GetProjectId1(LASHeaderH hHeader) {
    if (hHeader){
        long long fileid = ((LASHeader*) hHeader)->GetProjectId1();
        return fileid;
    }
    else return 0;
}

long LASHeader_GetProjectId2(LASHeaderH hHeader) {
    if (hHeader){
        long long fileid = ((LASHeader*) hHeader)->GetProjectId2();
        return fileid;
    }
    else return 0;
}

long LASHeader_GetProjectId3(LASHeaderH hHeader) {
    if (hHeader){
        long long fileid = ((LASHeader*) hHeader)->GetProjectId3();
        return fileid;
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

// uint32_t GetProjectId1() const;
// uint16_t GetProjectId2() const;
// uint16_t GetProjectId3() const;
// std::string GetProjectId4() const;

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