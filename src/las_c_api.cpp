#include <liblas/capi/liblas.h>

#include <liblas/lasreader.hpp>


#include <string>
using namespace liblas;
LAS_C_START

LASReaderH LASReader_Create(const char* filename) 

{
    return (LASReaderH) new LASReader(std::string(filename));   
}

LASHeaderH LASReader_GetHeader(LASReaderH hReader)
{
    return (LASHeaderH) &(((LASReader*) hReader)->GetHeader());
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