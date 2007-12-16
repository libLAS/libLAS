#include "liblas.h"

#include "lasdefinitions.h"
#include "laswriter.h"
#include "lasreader.h"
#include "lasheader.h"

LASReaderH LAS_Reader_Create() 

{
    return (LASReaderH) new LASreader();
    
}

LASHeaderH LAS_Reader_Get_Header(LASReaderH hReader)
{
    return (LASHeaderH) &(((LASreader*) hReader)->header);
}

int LAS_Reader_Open(LASReaderH hReader, FILE* file, int bSkipHeader, int bSkipVariableHeader)
{
    bool output = false;
    output = ((LASreader*)hReader)->open(file, bSkipHeader, bSkipVariableHeader);   
    return output;
}


LASWriterH LAS_Writer_Create()
{
    return (LASWriterH) new LASwriter();
}