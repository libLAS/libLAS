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
    if (hReader)
        return (LASHeaderH) &(((LASReader*) hReader)->GetHeader());
    else
        return NULL;
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