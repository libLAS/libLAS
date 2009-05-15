#include "oci_wrapper.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

OWStatement* Run(OWConnection* connection, ostringstream& command) 
{
    OWStatement* statement = 0;
    statement = connection->CreateStatement(command.str().c_str());
    
    if (statement->Execute() == false) {
        delete statement;
        return 0;
    }
    
    return statement;    
}

bool Cleanup(OWConnection* connection, const char* tableName)
{
    ostringstream oss;
    OWStatement* statement = 0;
    
    oss << "DELETE FROM " << tableName;
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");

    oss << "DROP TABLE " << tableName;
    cout << oss.str() << endl;
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");    
    
    oss << "DROP TABLE BLKTAB ";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");
    
    oss << "DROP TABLE RES ";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");

    oss << "DROP TABLE INPTAB ";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");

    oss << "DROP TABLE LIDAR_DATA";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");    

    oss << "DELETE FROM USER_SDO_GEOM_METADATA WHERE TABLE_NAME='"<< tableName << "'";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");

    return true;
    
    
}


bool CreateTable(OWConnection* connection, const char* tableName)
{
    ostringstream oss;
    OWStatement* statement = 0;
    
    oss << "CREATE TABLE "<< tableName <<" (ID VARCHAR2(24), VAL_D1 NUMBER, VAL_D2 NUMBER, VAL_D3 NUMBER)";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");
    
    return true;
    

}
void usage() {}

int main(int argc, char* argv[])
{
    int rc = 0;

    std::string input;
    std::string output;
    
    for (int i = 1; i < argc; i++)
    {
        if (    strcmp(argv[i],"-h") == 0 ||
                strcmp(argv[i],"--help") == 0
            )
        {
            usage();
            exit(0);
        }
        else if (   strcmp(argv[i],"--input") == 0  ||
                    strcmp(argv[i],"-input") == 0   ||
                    strcmp(argv[i],"-i") == 0       ||
                    strcmp(argv[i],"-in") == 0
                )
        {
            i++;
            input = std::string(argv[i]);
        }
        else if (   strcmp(argv[i],"--output") == 0  ||
                    strcmp(argv[i],"--out") == 0     ||
                    strcmp(argv[i],"-out") == 0     ||
                    strcmp(argv[i],"-o") == 0       
                )
        {
            i++;
            output = std::string(argv[i]);
        }
        else if (i == argc - 2 && output.empty() && input.empty())
        {
            input = std::string(argv[i]);
        }
        else if (i == argc - 1 && output.empty() && input.empty())
        {
            input = std::string(argv[i]);
        }
        else if (i == argc - 1 && output.empty() && input.empty())
        {
            output = std::string(argv[i]);
        }
        else 
        {
            usage();
            exit(1);
        }
    }
    
    OWConnection* con = new OWConnection("lidar","lidar","ubuntu/crrel.local");
    if (con->Succed()) printf("succeded");
    
    Cleanup(con, "base");
    CreateTable(con, "inptab");

 // int   iCol = 0;
 //    char  szField[OWNAME];
 //    int   hType = 0;
 //    int   nSize = 0;
 //    int   nPrecision = 0;
 //    signed short nScale = 0;
 // 
 //    char szColumnList[OWTEXT];
 //    szColumnList[0] = '\0';
 //    OCIParam* phDesc = NULL;
 // 
 //    const char* pszVATName="base";
 //    phDesc = con->GetDescription( (char*) pszVATName );
 //    while( con->GetNextField(
 //                phDesc, iCol, szField, &hType, &nSize, &nPrecision, &nScale ) )
 //    {
 //        printf("field ... %s",szField);
 //        switch( hType )
 //        {
 //            case SQLT_FLT:
 //                printf("float...\n");
 //                break;
 //            case SQLT_NUM:
 //                printf ("number...\n");
 //                break;
 //            case SQLT_CHR:
 //            case SQLT_AFC:
 //            case SQLT_DAT:
 //            case SQLT_DATE:
 //            case SQLT_TIMESTAMP:
 //            case SQLT_TIMESTAMP_TZ:
 //            case SQLT_TIMESTAMP_LTZ:
 //            case SQLT_TIME:
 //            case SQLT_TIME_TZ:
 //                printf ("character...\n");
 //                break;
 //            default:
 //                CPLDebug("GEORASTER", "VAT (%s) Column (%s) type (%d) not supported"
 //                    "as GDAL RAT", pszVATName, szField, hType );
 //                break;
 //        }
 //        // strcpy( szColumnList, CPLSPrintf( "%s substr(%s,1,%d),",
 //        //     szColumnList, szField, MIN(nSize,OWNAME) ) );
 // 
 //        iCol++;
 //    }



}
