
// god-awful hack because of GDAL/GeoTIFF's shitty include structure
#define CPL_SERV_H_INCLUDED

#include "oci_wrapper.h"

#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>

#include <liblas/laspoint.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/index/index.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include <vector>
#include <cctype>
#include <cmath>

#include <sys/stat.h>

#ifdef HAVE_SPATIALINDEX
#include <spatialindex/SpatialIndex.h>
#endif

#include <oci.h>

using namespace std;
using namespace liblas;

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif

typedef struct
{
    long* pc_ids;
    long* block_ids;
    long* num_points;
    std::vector<liblas::uint8_t>** blobs;

    long* srids;
    long* gtypes;
    OCIArray** element_arrays;
    OCIArray** coordinate_arrays;
        
} blocks;

typedef struct
{
    double x0;
    double x1;
    double y0;
    double y1;
    double z0;
    double z1;
    bool bUse3d;
   
} extent;

    
bool KDTreeIndexExists(std::string& filename)
{
    struct stat stats;
    std::ostringstream os;
    os << filename << ".kdx";

    std::string indexname = os.str();
    
    // ret is -1 for no file existing and 0 for existing
    int ret = stat(indexname.c_str(),&stats);

    bool output = false;
    if (ret == 0) output= true; else output =false;
    return output;
}


std::istream* OpenInput(std::string filename, bool bEnd) 
{
    std::ios::openmode mode = std::ios::in | std::ios::binary;
    if (bEnd == true) {
        mode = mode | std::ios::ate;
    }
    std::istream* istrm;
    if (compare_no_case(filename.c_str(),"STDIN",5) == 0)
    {
        istrm = &std::cin;
    }
    else 
    {
        istrm = new std::ifstream(filename.c_str(), mode);
    }
    
    if (!istrm->good())
    {
        delete istrm;
        throw std::runtime_error("Reading stream was not able to be created");
        exit(1);
    }
    return istrm;
}

std::string ReadSQLData(char const* filename)
{
    std::istream* infile = OpenInput(filename, true);
    ifstream::pos_type size;
    char* data;
    if (infile->good()){
        size = infile->tellg();
        data = new char [size];
        infile->seekg (0, ios::beg);
        infile->read (data, size);
        // infile->close();

        std::string output = std::string(data);
        delete[] data;
        delete infile;
        return output;
    } 
    else 
    {   
        delete infile;
        return std::string("");
    }
}

bool EnableTracing(OWConnection* connection)
{
    ostringstream oss;
// http://www.oracle-base.com/articles/10g/SQLTrace10046TrcsessAndTkprof10g.php
    oss << "ALTER SESSION SET EVENTS '10046 trace name context forever, level 12'";

    OWStatement* statement = 0;
    
    statement = connection->CreateStatement(oss.str().c_str());
    
    if (statement->Execute() == false) {
        
        std::cout << "statement execution failed "  << CPLGetLastErrorMsg() << std::endl;
        delete statement;
        return 0;
    }    
    
    return true;
}
bool IsGeographic(OWConnection* connection, long srid) {

    ostringstream oss;
    char* kind = (char* ) malloc (OWNAME * sizeof(char));
    oss << "SELECT COORD_REF_SYS_KIND from MDSYS.SDO_COORD_REF_SYSTEM WHERE SRID = :1";
    
    OWStatement* statement = 0;

    statement = connection->CreateStatement(oss.str().c_str());
    long* p_srid = (long*) malloc( 1 * sizeof(long));
    p_srid[0] = srid;
    
    statement->Bind(p_srid);
    statement->Define(kind);    
    if (statement->Execute() == false) {
        
        std::cout << "statement execution failed "  << CPLGetLastErrorMsg() << std::endl;
        delete statement;
        return false;
    }
    
    if (compare_no_case(kind, "GEOGRAPHIC2D",12) == 0) {
        delete statement;
        free(kind);
        return true;
    }
    if (compare_no_case(kind, "GEOGRAPHIC3D",12) == 0) {
        delete statement;
        free(kind);
        return true;
    }

    
    free(kind);
    return false;
}
OWStatement* Run(OWConnection* connection, ostringstream& command) 
{
    OWStatement* statement = 0;
    statement = connection->CreateStatement(command.str().c_str());
    
    if (statement->Execute() == false) {
        
        std::cout << "statement execution failed "  << CPLGetLastErrorMsg() << std::endl;
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
    if (statement != 0) delete statement;
    oss.str("");

    oss << "DROP TABLE " << tableName;
    cout << oss.str() << endl;
    statement = Run(connection, oss);
    if (statement != 0) delete statement; 
    oss.str("");       

    oss << "DELETE FROM USER_SDO_GEOM_METADATA WHERE TABLE_NAME='"<< tableName << "'";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; 
    oss.str("");

    return true;
    
    
}


bool CreateBlockTable(OWConnection* connection, const char* tableName)
{
    ostringstream oss;
    OWStatement* statement = 0;
    
    oss << "CREATE TABLE "<< tableName <<" (OBJ_ID NUMBER, BLK_ID NUMBER, "
                                         " BLK_EXTENT MDSYS.SDO_GEOMETRY, BLK_DOMAIN MDSYS.SDO_ORGSCL_TYPE,"
                                         " PCBLK_MIN_RES NUMBER, PCBLK_MAX_RES NUMBER, NUM_POINTS NUMBER, "
                                         " NUM_UNSORTED_POINTS NUMBER, PT_SORT_DIM NUMBER, POINTS BLOB)";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");

    return true;

}

bool DeleteTable(   OWConnection* connection, 
                    const char* tableName, 
                    const char* cloudTableName, 
                    const char* cloudColumnName)
{
    ostringstream oss;
    OWStatement* statement = 0;

    oss << "DELETE from " <<cloudTableName;
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else 
    {
        // if we failed, try dropping the index
        std::cout << "Dropping index ..." << std::endl;
        oss.str("");  
        oss << "DROP INDEX "<<tableName<<"_cloud_idx" ;
        statement = Run(connection, oss);
        if (statement != 0) delete statement; else return false;
        oss.str("");
        
        // redelete from the table
        oss << "DELETE from " <<cloudTableName;
        statement = Run(connection, oss);
        if (statement != 0) delete statement; else return false;
        oss.str("");
    }
    oss.str("");
    
    std::string cloudColumnName_l = std::string(cloudColumnName);
    std::string cloudColumnName_u = std::string(cloudColumnName_l);
    
    std::transform(cloudColumnName_l.begin(), cloudColumnName_l.end(), cloudColumnName_u.begin(), static_cast < int(*)(int) > (toupper));

    std::string cloudTableName_l = std::string(cloudTableName);
    std::string cloudTableName_u = std::string(cloudTableName_l);
    
    std::transform(cloudTableName_l.begin(), cloudTableName_l.end(), cloudTableName_u.begin(), static_cast < int(*)(int) > (toupper));

oss << "declare\n"
"begin \n"
"  mdsys.sdo_pc_pkg.drop_dependencies('"<<cloudTableName_u<<"', '"<<cloudColumnName_u<<"');"
"end;";
    statement = Run(connection, oss);
    if (statement != 0) delete statement; 
    oss.str("");
    
    oss << "DROP TABLE "<< tableName ;
    statement = Run(connection, oss);
    if (statement != 0) delete statement; 
    oss.str("");

    // Oracle upper cases the table name when inserting it in the 
    // USER_SDO_GEOM_METADATA.  We'll use std::transform to do it. 
    // See http://forums.devx.com/showthread.php?t=83058 for the 
    // technique
    string table(tableName);
    std::transform(table.begin(), table.end(), table.begin(), static_cast < int(*)(int) > (toupper));
    oss << "DELETE FROM USER_SDO_GEOM_METADATA WHERE TABLE_NAME='"<<table<<"'" ;
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");   

   
    return true;

}
bool GetPointData(  LASPoint const& p, 
                    bool bTime, 
                    std::vector<liblas::uint8_t>& point_data)
{
    // This function returns an array of bytes describing the 
    // x,y,z and optionally time values for the point.  

    point_data.clear();

    double x = p.GetX();
    double y = p.GetY();
    double z = p.GetZ();
    double t = p.GetTime();

    liblas::uint8_t* x_b =  reinterpret_cast<liblas::uint8_t*>(&x);
    liblas::uint8_t* y_b =  reinterpret_cast<liblas::uint8_t*>(&y);
    liblas::uint8_t* z_b =  reinterpret_cast<liblas::uint8_t*>(&z);

    liblas::uint8_t* t_b =  reinterpret_cast<liblas::uint8_t*>(&t);

    // doubles are 8 bytes long.  For each double, push back the 
    // byte.  We do this for all four values (x,y,z,t)

    // // little-endian
    // for (int i=0; i<sizeof(double); i++) {
    //     point_data.push_back(y_b[i]);
    // }
    // 

    // big-endian
    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(x_b[i]);
    }

    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(y_b[i]);
    }   

    for (int i = sizeof(double) - 1; i >= 0; i--) {
        point_data.push_back(z_b[i]);
    }

    
    if (bTime)
    {
        for (int i = sizeof(double) - 1; i >= 0; i--) {
            point_data.push_back(t_b[i]);
        }

    }

    return true;
}
bool GetResultData( const LASQueryResult& result, 
                    LASReader* reader, 
                    std::vector<liblas::uint8_t>& data, 
                    int nDimension)
{
    list<SpatialIndex::id_type> const& ids = result.GetIDs();


    // d 8-byte IEEE  big-endian doubles, where d is the PC_TOT_DIMENSIONS value
    // 4-byte big-endian integer for the BLK_ID value
    // 4-byte big-endian integer for the PT_ID value
    
    bool bTime = false;
    if (nDimension == 4)
    {
        bTime = true;
    }
    
    data.clear();
    
    list<SpatialIndex::id_type>::const_iterator i;
    vector<liblas::uint8_t>::iterator pi;
    
    liblas::uint32_t block_id = result.GetID();

    std::vector<liblas::uint8_t> point_data;
    
    for (i=ids.begin(); i!=ids.end(); ++i) 
    {
        SpatialIndex::id_type id = *i;

        bool doRead = reader->ReadPointAt(id);
        if (doRead) {
            LASPoint const& p = reader->GetPoint();

            // d 8-byte IEEE  big-endian doubles, where d is the PC_TOT_DIMENSIONS value
            bool gotdata = GetPointData(p, bTime, point_data);
            
            if (!gotdata) { throw std::runtime_error("Unable to fetch Point Data"); exit(1);}
            std::vector<liblas::uint8_t>::const_iterator d;
            for (d = point_data.begin(); d!=point_data.end(); ++d) {
                data.push_back(*d);
            }

            liblas::uint8_t* id_b = reinterpret_cast<liblas::uint8_t*>(&id);
            liblas::uint8_t* block_b = reinterpret_cast<liblas::uint8_t*>(&block_id);
            
            // 4-byte big-endian integer for the BLK_ID value
            for (int i =  sizeof(liblas::uint32_t) - 1; i >= 0; i--) {
                data.push_back(block_b[i]);
            }
            
            // 4-byte big-endian integer for the PT_ID value
            for (int i =  sizeof(liblas::uint32_t) - 1; i >= 0; i--) {
                data.push_back(id_b[i]);
            }
            

        }
    }

    return true;
}

void SetElements(   OWStatement* statement,
                    OCIArray* sdo_elem_info, 
                    bool bUseSolidGeometry)
{
    

    statement->AddElement(sdo_elem_info, 1);
    if (bUseSolidGeometry == true) {
        //"(1,1007,3)";
        statement->AddElement(sdo_elem_info, 1007);
    } else {
        //"(1,1003,3)";
        statement->AddElement(sdo_elem_info, 1003);
    }

    statement->AddElement(sdo_elem_info, 3);
 
}

void SetOrdinates(   OWStatement* statement,
                     OCIArray* sdo_ordinates, 
                     extent* extent)
{
    
    statement->AddElement(sdo_ordinates, extent->x0);
    statement->AddElement(sdo_ordinates, extent->y0);
    if (extent->bUse3d)
        statement->AddElement(sdo_ordinates, extent->z0);
    
    statement->AddElement(sdo_ordinates, extent->x1);
    statement->AddElement(sdo_ordinates, extent->y1);
    if (extent->bUse3d)
        statement->AddElement(sdo_ordinates, extent->z1);
        

}

extent* GetExtent(  const SpatialIndex::Region* b,
                    bool bUse3d
                 )
{
    double x0, x1, y0, y1, z0, z1;
    // const SpatialIndex::Region* b = result.GetBounds();

    x0 = b->getLow(0); 
    x1 = b->getHigh(0); 
    y0 = b->getLow(1); 
    y1 = b->getHigh(1);
    z0 = 0;
    z1 = 20000;
    if (bUse3d) {
        try {
            z0 = b->getLow(2);
            z1 = b->getHigh(2);
        } catch (Tools::IndexOutOfBoundsException& e) {
            
        }
    }
    extent* e = (extent*) malloc (sizeof(extent));
    e->x0 = x0; e->x1 = x1;
    e->y0 = y0; e->y1 = y1;
    e->z0 = z0; e->z1 = z1;
    e->bUse3d = bUse3d;
    
    return e;    
}
blocks* CreateBlock(int size)
{
    blocks* b = (blocks*) malloc( sizeof(blocks));
    
    b->pc_ids = (long*) malloc( size * sizeof(long));
    b->block_ids = (long*) malloc ( size * sizeof(long));
    b->num_points = (long*) malloc ( size * sizeof(long));
    b->blobs = (std::vector<liblas::uint8_t>**) malloc ( size * sizeof(std::vector<liblas::uint8_t>*));

    b->srids = (long*) malloc ( size * sizeof(long));
    b->gtypes = (long*) malloc ( size * sizeof(long));

    b->element_arrays = (OCIArray**) malloc ( size * sizeof(OCIArray*));
    b->coordinate_arrays = (OCIArray**) malloc ( size * sizeof(OCIArray*));
    return b;
}
bool FillBlock( OWConnection* connection, 
                OWStatement* statement,
                const LASQueryResult& result, 
                LASReader* reader,
                blocks* b,
                long index,
                int srid, 
                long pc_id,
                long gtype,
                bool bUseSolidGeometry,
                bool bUse3d,
                long nDimensions
              )
{


    list<SpatialIndex::id_type> const& ids = result.GetIDs();
   
    b->pc_ids[index] = pc_id;
    b->srids[index] = srid;
    b->block_ids[index] = result.GetID();
    b->num_points[index] = (long)ids.size();
    
    // TODO: This probably is a memory leak if the gotdata == false --mloskot
    std::vector<liblas::uint8_t>* blob = new std::vector<liblas::uint8_t>;
    
    bool gotdata = GetResultData(result, reader, *blob, nDimensions);
    if (! gotdata) throw std::runtime_error("unable to fetch point data byte array");
    b->blobs[index] = blob;
    // FIXME: null srids not supported 
    b->srids[index] = srid;
    b->gtypes[index] = gtype;

    OCIArray* sdo_elem_info=0;
    connection->CreateType(&sdo_elem_info, connection->GetElemInfoType());
    SetElements(statement, sdo_elem_info, bUseSolidGeometry);
    
    b->element_arrays[index] = sdo_elem_info;

    OCIArray* sdo_ordinates=0;
    connection->CreateType(&sdo_ordinates, connection->GetOrdinateType());



    extent* e = GetExtent(result.GetBounds(), bUse3d);
    SetOrdinates(statement, sdo_ordinates, e);

    b->coordinate_arrays[index] = sdo_ordinates;
    
    return true;
}

long GetGType(  bool bUse3d,
                bool bUseSolidGeometry)
{
    long gtype = 0;
    if (bUse3d) {
        if (bUseSolidGeometry == true) {
            gtype = 3008;

        } else {
            gtype = 3003;
        }
    } else {
        if (bUseSolidGeometry == true) {
            gtype = 2008;
        } else {
            gtype = 2003;
        }
    }
    
    return gtype;   
}

bool BindBlock(OWStatement* statement, blocks* b, OCILobLocator** locator, long commit_interval)
{
    // oss << "INSERT INTO "<< table_name << 
    //         "(OBJ_ID, BLK_ID, NUM_POINTS, POINTS,   "
    //         "PCBLK_MIN_RES, BLK_EXTENT, PCBLK_MAX_RES, NUM_UNSORTED_POINTS, PT_SORT_DIM) "
    //         "VALUES ( :1, :2, :3, :4, 1, mdsys.sdo_geometry(:5, :6, null,:7, :8)" 
    //         ", 1, 0, 1)";    
    
    // :1
    statement->Bind( b->pc_ids );
    
    // :2
    statement->Bind( b->block_ids );

    // :3
    statement->Bind( b->num_points);
       
    // :4
    statement->Define( locator, (int)commit_interval ); 
    
    long max_size = 0;
    for (int i = 0; i < commit_interval; i++) {
        max_size = std::max(max_size, (long)b->blobs[i]->size());
    }

    statement->Bind((char*)b->blobs,(long)max_size);    

    // :5
    statement->Bind(b->gtypes);

    // :6
    statement->Bind(b->srids);    

    
    // :7
    // OCIArray* sdo_elem_info=0;
    // connection->CreateType(&sdo_elem_info, connection->GetElemInfoType());
    // SetElements(statement, sdo_elem_info, bUseSolidGeometry);    
    // statement->Bind(&sdo_elem_info, connection->GetElemInfoType());

    // long* pc_ids;
    // long* block_ids;
    // long* num_points;
    // std::vector<liblas::uint8_t>** blobs;
    // 
    // long* srids;
    // long* gtypes;
    // OCIArray** element_arrays;
    // OCIArray** coordinate_arrays;

    return true;
}
bool InsertBlock(OWConnection* connection, 
                const LASQueryResult& result, 
                blocks* block,
                long block_index,
                int srid, 
                LASReader* reader, 
                const char* tableName, 
                long precision,
                long pc_id,
                bool bUseSolidGeometry,
                bool bUse3d)
{
    ostringstream oss;

    list<SpatialIndex::id_type> const& ids = result.GetIDs();
    // const SpatialIndex::Region* b = result.GetBounds();
    liblas::uint32_t num_points = ids.size();


    // EnableTracing(connection);
    
    long gtype = GetGType(bUse3d, bUseSolidGeometry);

    oss << "INSERT INTO "<< tableName << 
            "(OBJ_ID, BLK_ID, NUM_POINTS, POINTS,   "
            "PCBLK_MIN_RES, BLK_EXTENT, PCBLK_MAX_RES, NUM_UNSORTED_POINTS, PT_SORT_DIM) "
            "VALUES ( :1, :2, :3, :4, 1, mdsys.sdo_geometry(:5, :6, null,:7, :8)" 
            ", 1, 0, 1)";
          
    OWStatement* statement = 0;
    // TODO: If gotdata == false below, this memory probably leaks --mloskot
    OCILobLocator** locator =(OCILobLocator**) VSIMalloc( sizeof(OCILobLocator*) * 1 );

    statement = connection->CreateStatement(oss.str().c_str());
    

    
    long* p_pc_id = (long*) malloc( 1 * sizeof(long));
    p_pc_id[0] = pc_id;

    long* p_result_id = (long*) malloc( 1 * sizeof(long));
    p_result_id[0] = (long)result.GetID();
    
    long* p_num_points = (long*) malloc (1 * sizeof(long));
    p_num_points[0] = (long)num_points;
    
    
    // :1
    statement->Bind( p_pc_id );
    
    // :2
    statement->Bind( p_result_id );

    // :3
    statement->Bind( p_num_points );
       
    // :4
    statement->Define( locator, 1 ); 
    
    std::vector<liblas::uint8_t> data;
    bool gotdata = GetResultData(result, reader, data, 3);
    if (! gotdata) throw std::runtime_error("unable to fetch point data byte array");

    statement->Bind((char*)&(data[0]),(long)data.size());

    // :5
    long* p_gtype = (long*) malloc (1 * sizeof(long));
    p_gtype[0] = gtype;

    statement->Bind(p_gtype);
    
    // :6
    long* p_srid  = 0;
    
    
    if (srid != 0) {
        p_srid = (long*) malloc (1 * sizeof(long));
        p_srid[0] = srid;
    }
    statement->Bind(p_srid);
    
    // :7
    OCIArray* sdo_elem_info=0;
    connection->CreateType(&sdo_elem_info, connection->GetElemInfoType());
    SetElements(statement, sdo_elem_info, bUseSolidGeometry);    
    statement->Bind(&sdo_elem_info, connection->GetElemInfoType());
    
    // :8
    OCIArray* sdo_ordinates=0;
    connection->CreateType(&sdo_ordinates, connection->GetOrdinateType());
    
    extent* e = GetExtent(result.GetBounds(), bUse3d);

    
     // x0, x1, y0, y1, z0, z1, bUse3d
    SetOrdinates(statement, sdo_ordinates, e);
    statement->Bind(&sdo_ordinates, connection->GetOrdinateType());
    
    if (statement->Execute() == false) {
        delete statement;
        return false;
    }
    
    delete statement; statement = 0;
    oss.str("");
    

    OWStatement::Free(locator, 1);

    delete statement;
    
    return true;

}

bool InsertBlocks(
                OWConnection* con, 
                const std::list<LASQueryResult>& results,
                long nCommitInterval, 
                int srid, 
                LASReader* reader2, 
                const std::string& table_name, 
                long precision,
                long pc_id,
                bool bUseSolidGeometry,
                bool bUse3d,
                long nDimensions
                )
{
    std::list<LASQueryResult>::const_iterator i;

    long commit_interval = 1000;
    blocks* b = CreateBlock(commit_interval);

    ostringstream oss;
    oss << "INSERT INTO "<< table_name << 
            "(OBJ_ID, BLK_ID, NUM_POINTS, POINTS,   "
            "PCBLK_MIN_RES, BLK_EXTENT, PCBLK_MAX_RES, NUM_UNSORTED_POINTS, PT_SORT_DIM) "
            "VALUES ( :1, :2, :3, :4, 1, mdsys.sdo_geometry(:5, :6, null,:7, :8)" 
            ", 1, 0, 1)";
          
    OWStatement* statement = 0;
    // OCILobLocator** locator =(OCILobLocator**) VSIMalloc( sizeof(OCILobLocator*) * 1000 );

    statement = con->CreateStatement(oss.str().c_str());
    long j = 0;
    bool inserted = false;
    for (i=results.begin(); i!=results.end(); i++)
    {
        j++;
        // 
        // FillBlock( con, 
        //                 statement,
        //                 *i, 
        //                 reader2,
        //                 b,
        //                 j,
        //                 srid, 
        //                  pc_id,
        //                  GetGType(bUse3d, bUseSolidGeometry),
        //                  bUseSolidGeometry,
        //                  bUse3d,
        //                  nDimensions
        //                  );
        
        inserted = InsertBlock(con, 
                                    *i,
                                    b,
                                    j, 
                                    srid, 
                                    reader2, 
                                    table_name.c_str(), 
                                    precision, 
                                    pc_id, 
                                    bUseSolidGeometry, 
                                    bUse3d);
    }
    return inserted;
}

bool CreateSDOEntry(    OWConnection* connection, 
                        const char* tableName, 
                        LASQuery* query, 
                        long srid, 
                        long precision,
                        bool bUseSolidGeometry,
                        bool bUse3d,
                        bool bSetExtents,
                        double xmin,
                        double xmax,
                        double ymin,
                        double ymax,
                        double zmin,
                        double zmax)
{
    ostringstream oss;
    OWStatement* statement = 0;
    // SpatialIndex::Region* b = query->bounds;
    ostringstream oss_geom;
    
    oss.setf(std::ios_base::fixed, std::ios_base::floatfield);
    oss.precision(precision);

    ostringstream s_srid;
    

    if (srid == 0) {
        s_srid << "NULL";
        // bUse3d = true;
    }
    else {
        s_srid << srid;
    }

    double tolerance = 0.05;
    extent* e = GetExtent(  &(query->bounds), bUse3d );

    if (IsGeographic(connection, srid)) {
        e->x0 = -180.0; e->x1 = 180.0;
        e->y0 = -90.0; e->y1 = 90.0;
        e->z0 = 0.0; e->z1 = 20000.0;

        tolerance = 0.000000005;
    }


    if (bSetExtents){
        e->x0 = xmin; e->x1 = xmax;
        e->y0 = ymin; e->y1 = ymax;
        e->z0 = zmin; e->z1 = zmax;
    }     

     
    oss <<  "INSERT INTO user_sdo_geom_metadata VALUES ('" << tableName <<
        "','blk_extent', MDSYS.SDO_DIM_ARRAY(";
    
    oss << "MDSYS.SDO_DIM_ELEMENT('X', " << e->x0 << "," << e->x1 <<"," << tolerance << "),"
           "MDSYS.SDO_DIM_ELEMENT('Y', " << e->y0 << "," << e->y1 <<"," << tolerance << ")";
           
    if (e->bUse3d) {
        oss << ",";
        oss <<"MDSYS.SDO_DIM_ELEMENT('Z', "<< e->z0 << "," << e->z1 << "," << tolerance << ")";
    }
    oss << ")," << s_srid.str() << ")";
    
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");
    
    delete e;
    
    return true;
        
}

bool CreateBlockIndex(  OWConnection* connection, 
                        const char* tableName, 
                        long srid, 
                        bool bUseSolidGeometry,
                        bool bUse3d)
{
    ostringstream oss;
    OWStatement* statement = 0;
    
    
    // if (bUseSolidGeometry == true) {
    //     bUse3d = true;
    // }
    
    // if (srid == 0) {
        // bUse3d = true;
    // } 
    
    // if (srid == 4326) {
    //     bUse3d = true;
    // }
    
    oss << "CREATE INDEX "<< tableName <<"_cloud_idx on "<<tableName<<"(blk_extent) INDEXTYPE IS MDSYS.SPATIAL_INDEX";
    
    if (bUse3d) {
        oss <<" PARAMETERS('sdo_indx_dims=3')" ;
    }
    
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");

    oss << "CREATE INDEX "<< tableName <<"_objectid_idx on "<<tableName<<"(OBJ_ID,BLK_ID) COMPRESS 2" ;
    statement = Run(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");    
    return true;
        
}

bool BlockTableExists(OWConnection* connection, const char* tableName)
{
    ostringstream oss;

    char szTable[OWNAME]= "";
    oss << "select table_name from user_tables where table_name like upper('%%"<< tableName <<"%%') ";

    OWStatement* statement = 0;
    
    statement = connection->CreateStatement(oss.str().c_str());
    statement->Define(szTable);
    
    if (statement->Execute() == false) {
        
        std::cout << "statement execution failed "  << CPLGetLastErrorMsg() << std::endl;
        delete statement;
        return 0;
    }
    
    while( statement->Fetch() ) {
        return true;
    }
    return false;
        
}



long CreatePCEntry( OWConnection* connection, 
                    LASQuery* query, 
                    const char* blkTableName, 
                    const char* pcTableName, 
                    const char* cloudColumnName,
                    const char* aux_columns,
                    const char* aux_values,
                    int nDimension, 
                    int srid,
                    int blk_capacity,
                    long precision,
                    bool bUseSolidGeometry,
                    bool bUse3d,
                    bool bSetExtents,
                    double xmin,
                    double xmax,
                    double ymin,
                    double ymax,
                    double zmin,
                    double zmax)
{
    ostringstream oss;

    OWStatement* statement = 0;

    oss.setf(std::ios_base::fixed, std::ios_base::floatfield);
    oss.precision(precision);
    
    std::string blkTableName_l = std::string(blkTableName);
    std::string blkTableName_u = std::string(blkTableName_l);
    
    std::string pcTableName_l = std::string(pcTableName);
    std::string pcTableName_u = std::string(pcTableName_l);
    
    std::string cloudColumnName_l = std::string(cloudColumnName);
    std::string cloudColumnName_u = std::string(cloudColumnName_l);

    std::string aux_columns_l = std::string(aux_columns);
    std::string aux_columns_u = std::string(aux_columns);
 
    std::string aux_values_l = std::string(aux_values);
           
    std::transform(blkTableName_l.begin(), blkTableName_l.end(), blkTableName_u.begin(), static_cast < int(*)(int) > (toupper));
    std::transform(pcTableName_l.begin(), pcTableName_l.end(), pcTableName_u.begin(), static_cast < int(*)(int) > (toupper));
    std::transform(cloudColumnName_l.begin(), cloudColumnName_l.end(), cloudColumnName_u.begin(), static_cast < int(*)(int) > (toupper));
    std::transform(aux_columns_l.begin(), aux_columns_l.end(), aux_columns_u.begin(), static_cast < int(*)(int) > (toupper));
    
    ostringstream columns;
    ostringstream values;
    
    if (!aux_columns_u.empty() ) {
        columns << cloudColumnName_u << "," << aux_columns_u;
    
        values << "pc," << aux_values_l;
    } else {
        columns << cloudColumnName_u;
        values << "pc";
    }


    ostringstream s_srid;
    ostringstream s_gtype;
    ostringstream s_eleminfo;
    ostringstream s_geom;


    bool is_geo = IsGeographic(connection, srid);

    if (srid == 0) {
        s_srid << "NULL";
        }
    else {
        s_srid << srid;
    }

    long gtype = GetGType(bUse3d, bUseSolidGeometry);
    s_gtype << gtype;
    if (bUse3d) {
        if (bUseSolidGeometry == true) {
            // s_gtype << "3008";
            s_eleminfo << "(1,1007,3)";

        } else {
            // s_gtype << "3003";
            s_eleminfo  << "(1,1003,3)";

        }
    } else {
        if (bUseSolidGeometry == true) {
            // s_gtype << "2008";
            s_eleminfo << "(1,1007,3)";

        } else {
            // s_gtype << "2003";
            s_eleminfo  << "(1,1003,3)";

        }
    }
    

    extent* e = GetExtent(  &(query->bounds), bUse3d );


    s_geom << "           mdsys.sdo_geometry("<<s_gtype.str() <<", "<<s_srid.str()<<", null,\n"
"              mdsys.sdo_elem_info_array"<< s_eleminfo.str() <<",\n"
"              mdsys.sdo_ordinate_array(\n";

    s_geom << e->x0 << "," << e->y0 << ",";

    if (bUse3d) {
        s_geom << e->z0 << ",";
    }
    
    s_geom << e->x1 << "," << e->y1;

    if (bUse3d) {
        s_geom << "," << e->z1;
    }

    s_geom << "))";

    
    
oss << "declare\n"
"  pc_id NUMBER := :1;\n"
"  pc sdo_pc;\n"

"begin\n"
"  -- Initialize the Point Cloud object.\n"
"  pc := sdo_pc_pkg.init( \n"
"          '"<< pcTableName_u<<"', -- Table that has the SDO_POINT_CLOUD column defined\n"
"          '"<< cloudColumnName_u<<"',   -- Column name of the SDO_POINT_CLOUD object\n"
"          '"<<blkTableName_u<<"', -- Table to store blocks of the point cloud\n"
"           'blk_capacity="<<blk_capacity<<"', -- max # of points per block\n"
<< s_geom.str() <<
",  -- Extent\n"
"     0.5, -- Tolerance for point cloud\n"
"           "<<nDimension<<", -- Total number of dimensions\n"
"           null);\n"
"  :1 := pc.pc_id;\n"

"  -- Insert the Point Cloud object  into the \"base\" table.\n"
"  insert into "<< pcTableName_u<<" ("<<columns.str()<<") values ("<<values.str()<<");\n"

"  "
"end;\n";


    int* pc_id = (int*) malloc (1*sizeof(int));
    *pc_id = 0;
    long output = 0;
    statement = connection->CreateStatement(oss.str().c_str());
    statement->Bind(pc_id);
    if (statement->Execute() == false) {
        
        std::cout << "statement execution failed "  << CPLGetLastErrorMsg() << std::endl;
        delete statement;
        return 0;
    }
    
    connection->Commit();
    output = *pc_id;
    
    delete pc_id;
    return output;

    return output;
}
void usage() {
    fprintf(stderr,"----------------------------------------------------------\n");
    fprintf(stderr,"    las2oci (version ) usage:\n");
    fprintf(stderr,"----------------------------------------------------------\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"--block-table-name: (-bk)\n");
    fprintf(stderr,"--base-table-name: (-bn)\n");
    fprintf(stderr,"--cloud-table-name: (-cn) \n");
    fprintf(stderr,"--overwrite: (-d) \n");
    fprintf(stderr,"--srid: (-s) \n");
    fprintf(stderr,"--pre-sql: (-prs) \"CREATE TABLE BASE (id number, cloud mdsys.sdo_pc)\" \n");    
    fprintf(stderr,"--post-sql: (-pos) \"UPDATE TABLE BASE SET ID=1\" \n");    
    fprintf(stderr,"--pre-block-sql: (-pbs) \"UPDATE TABLE PC a set a.geometry = (SELECT b.geometry from boundaries b where a.filename = b.filename))\"");
    fprintf(stderr,"--aux-columns: \"id,description\" \n");    
    fprintf(stderr,"--aux-values: \"0,'A description'\" \n");    
    fprintf(stderr,"--precision: 8\n");    
        
    fprintf(stderr,"las2oci -i output.las lidar/lidar@oraclemachine/instance \n"
                   "--block-table-name  hobu_blocks --base-table-name hobu_base\n"
                   "--cloud-column-name PC --srid 4326 -d\n");
    
    

    fprintf(stderr, "\nFor more information, see the full documentation for las2las at:\n"
                    " http://liblas.org/browser/trunk/doc/las2las.txt\n");
    fprintf(stderr,"----------------------------------------------------------\n");    
}

bool ExternalIndexExists(std::string& filename)
{
    struct stat stats;
    std::ostringstream os;
    os << filename << ".dat";
    
    std::string indexname = os.str();
    
    // ret is -1 for no file existing and 0 for existing
    int ret = stat(indexname.c_str(),&stats);

    bool output = false;
    if (ret == 0) output= true; else output =false;
    return output;
}


// select sdo_pc_pkg.to_geometry(a.points, a.num_points, 3, 4326) from NACHES_BAREEARTH_BLOCK1 a where a.obj_id= 8907
int main(int argc, char* argv[])
{

    std::string input ("");
    std::string connection("");
    std::string username;
    std::string password;
    std::string instance;
    std::string point_cloud_name("CLOUD");
    std::string base_table_name("HOBU");
    std::string block_table_name("");
    
    std::string pre_sql("");
    std::string post_sql("");
    std::string pre_block_sql("");
    
    std::string aux_columns("");
    std::string aux_values("");
    
    bool bUseExistingBlockTable = false;
    bool bDropTable = false;
    bool bUseSolidGeometry = false;
    bool bUse3d = false;
    
    liblas::uint32_t nCapacity = 10000;
    double dFillFactor = 0.99;
    int srid = 0;
    long precision = 8;
    long idx_dimension = 3;
    
    double xmin = 0.0;
    double ymin = 0.0;
    double zmin = 0.0;
    double xmax = 0.0;
    double ymax = 0.0;
    double zmax = 0.0;
    bool bSetExtents = false;
    
    int nCommitInterval = 100;
    
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
        else if (   strcmp(argv[i],"--overwrite") == 0  ||
                    strcmp(argv[i],"-drop") == 0   ||
                    strcmp(argv[i],"-d") == 0 
                )
        {
            bDropTable=true;
        }
        else if (   strcmp(argv[i],"--blk_capacity") == 0  ||
                    strcmp(argv[i],"--capacity") == 0   ||
                    strcmp(argv[i],"-c") == 0 
                )
        {
            i++;
            nCapacity=atoi(argv[i]);
        }
        else if (   strcmp(argv[i],"--fill") == 0  ||
                    strcmp(argv[i],"-fill") == 0   ||
                    strcmp(argv[i],"-f") == 0 
                )
        {
            i++;
            dFillFactor=atof(argv[i]);
        }
        else if (   strcmp(argv[i],"--srid") == 0  ||
                    strcmp(argv[i],"-srid") == 0   ||
                    strcmp(argv[i],"-s") == 0 
                )
        {
            i++;
            srid=atoi(argv[i]);
        }

        else if (   strcmp(argv[i],"--idx-dimension") == 0  ||
                    strcmp(argv[i],"-dim") == 0  
                )
        {
            i++;
            idx_dimension=atoi(argv[i]);
        }
        
        else if (   strcmp(argv[i],"--cloud-column-name") == 0  ||
                    strcmp(argv[i],"-cn") == 0  
                )
        {
            i++;
            point_cloud_name = std::string(argv[i]);
        }
        else if (   strcmp(argv[i],"--base-table-name") == 0  ||
                    strcmp(argv[i],"-bn") == 0  
                )
        {
            i++;
            base_table_name = std::string(argv[i]);
        }
        else if (   strcmp(argv[i],"--block-table-name") == 0  ||
                    strcmp(argv[i],"-bk") == 0  
                )
        {
            i++;
            block_table_name = std::string(argv[i]);
        }
        else if (   strcmp(argv[i],"--pre-sql") == 0  ||
                    strcmp(argv[i],"-prs") == 0  
                )
        {
            i++;
            try {
                pre_sql = ReadSQLData(argv[i]);
            } catch (std::runtime_error const& e) {
                pre_sql = std::string(argv[i]);
            }
        }
        else if (   strcmp(argv[i],"--post-sql") == 0  ||
                    strcmp(argv[i],"-pos") == 0  
                )
        {
            i++;
            try {
                post_sql = ReadSQLData(argv[i]);
            } catch (std::runtime_error const& e) {
                post_sql = std::string(argv[i]);
            }
        }
        else if (   strcmp(argv[i],"--pre-block-sql") == 0  ||
                    strcmp(argv[i],"-pbs") == 0  
                )
        {
            i++;
            try {
                pre_block_sql = ReadSQLData(argv[i]);
            } catch (std::runtime_error const& e) {
                pre_block_sql = std::string(argv[i]);
            }
        }
        else if (   strcmp(argv[i],"--aux-columns") == 0  ||
                    strcmp(argv[i],"-ac") == 0  
                )
        {
            i++;
            aux_columns = std::string(argv[i]);
        }
        else if (   strcmp(argv[i],"--aux-values") == 0  ||
                    strcmp(argv[i],"-av") == 0  
                )
        {
            i++;
            aux_values = std::string(argv[i]);
        }
        else if (   strcmp(argv[i],"--precision") == 0  ||
                    strcmp(argv[i],"-p") == 0  
                )
        {
            i++;
            precision = atoi(argv[i]);
        }
        else if (   strcmp(argv[i],"--commit-interval") == 0  ||
                    strcmp(argv[i],"-ci") == 0  
                )
        {
            i++;
            nCommitInterval = atoi(argv[i]);
        }
        else if (   strcmp(argv[i],"--solid") == 0 ||
                    strcmp(argv[i],"-solid") == 0
                )
        {
            bUseSolidGeometry=true;
        }
        else if (   strcmp(argv[i],"--3d") == 0  ||
                    strcmp(argv[i],"-3d") == 0
                )
        {
            bUse3d=true;
        }
        else if (   strcmp(argv[i],"--xmin") == 0  ||
                    strcmp(argv[i],"-xmin") == 0
                )
        {
            i++;
            xmin = atof(argv[i]);
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--xmax") == 0  ||
                    strcmp(argv[i],"-xmax") == 0
                )
        {
            i++;
            xmax = atof(argv[i]);
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--ymin") == 0  ||
                    strcmp(argv[i],"-ymin") == 0
                )
        {
            i++;
            ymin = atof(argv[i]);
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--ymax") == 0  ||
                    strcmp(argv[i],"-ymax") == 0
                )
        {
            i++;
            ymax = atof(argv[i]);
            bSetExtents = true;
        }
        else if (   strcmp(argv[i],"--zmin") == 0  ||
                    strcmp(argv[i],"-zmin") == 0
                )
        {
            i++;
            zmin = atof(argv[i]);
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--zmax") == 0  ||
                    strcmp(argv[i],"-zmax") == 0
                )
        {
            i++;
            zmax = atof(argv[i]);
            bSetExtents = true;
        }
        else if (input.empty())
        {
            input = std::string(argv[i]);
        }
        else if (connection.empty())
        {
            connection = std::string(argv[i]);
        }
        else 
        {
            usage();
            exit(1);
        }
    }

    
    if (connection.empty() || input.empty()) {
        usage();
        exit(1);
    }
    string table_name ;
    if (block_table_name.size() == 0) {
        // change filename foo.las -> foo for an appropriate
        // block tablename for oracle... must be less than 30 characters
        // and no extraneous characters.
        string::size_type dot_pos = input.find_first_of(".");
        table_name = input.substr(0,dot_pos);
    } else {
        table_name = block_table_name;
    }
    string::size_type slash_pos = connection.find("/",0);
    username = connection.substr(0,slash_pos);
    string::size_type at_pos = connection.find("@",slash_pos);
    std::cout <<"slash_pos: " << slash_pos << " at_pos: " << at_pos<<std::endl;
    password = connection.substr(slash_pos+1, at_pos-slash_pos-1);
    instance = connection.substr(at_pos+1);
    std::cout << "Connecting with username: " << username << " password: "<< password<< " instance: " << instance << std::endl;    

    std::cout << "Base table name " << base_table_name << " cloud column: " << point_cloud_name <<" block table: " << block_table_name << std::endl;
    // OCI_SUCCESS_WITH_INFO error, which according to google relates to 
    // a warning related to expired or expiring passwords needs to be 
    // handled in the oracle wrapper.
    
    // Create the index before connecting to Oracle.  That way we don't heartbeat
    // the server while we're cruising through the file(s).
    
    // OS X RAMDISK configuration
    // http://www.macosxhints.com/article.php?story=20090222092827145
    
    // Obj_id is serial for each row in the block table
    // blk_id is the index leaf node id (this is currently being written incorrectly)
    OWConnection* con = new OWConnection(username.c_str(),password.c_str(),instance.c_str());
    if (con->Succeeded()) {
        std::cout <<"Oracle connection succeded" << std::endl;
    } else {
        std::cout <<"Oracle connection failed" << std::endl; exit(1);
    }

    con->StartTransaction();

    std::istream* istrm;
    try {
            istrm = OpenInput(input, false);
    } catch (std::exception const& e)
    {
        std::cout << e.what() << std::endl;
        std::cout << "exiting..." << std::endl;
        exit(-1);
    }

    
    
    if (bDropTable) {
        std::cout << "dropping existing tables..." << std::endl;

        DeleteTable(con, table_name.c_str(), base_table_name.c_str(), point_cloud_name.c_str());
    }
    
    if (!pre_sql.empty()) {
        std::cout << "running pre-sql ..." << std::endl;
        ostringstream oss;
        oss << pre_sql;
        OWStatement* statement = 0;
        statement = Run(con, oss);
        if (statement != 0) {
            delete statement; 
        }
        else {
            std::cout << "pre-sql execution failed.." << std::endl;
            return false;
        }
        oss.str("");        
    }
    if (!BlockTableExists(con, table_name.c_str()))
        CreateBlockTable(con, table_name.c_str());
    else {
        bUseExistingBlockTable = true;
        std::cout << "Using existing block table ... " << std::endl;

    }

    LASReader* reader = new LASReader(*istrm);
    LASQuery* query = 0;
    if (!KDTreeIndexExists(input)) {

        LASIndexDataStream* idxstrm = new LASIndexDataStream(reader, idx_dimension);

        LASIndex* idx = new LASIndex(input);
        idx->SetType(LASIndex::eExternalIndex);
        idx->SetLeafCapacity(nCapacity);
        idx->SetFillFactor(dFillFactor);
        idx->Initialize(*idxstrm);
        query = new LASQuery;
        idx->Query(*query);
        if (idx != 0) delete idx;
        if (reader != 0) delete reader;
        if (istrm != 0 ) delete istrm;
            
    } else {
        std::cout << "Using kdtree ... " << std::endl;
        std::ostringstream os;
        os << input << ".kdx" ;
        
        std::istream* kdx = OpenInput(os.str(), false);
        query = new LASQuery(*kdx);
    }

    std::list<LASQueryResult>& results = query->GetResults();
    
    std::list<LASQueryResult>::const_iterator i;
    
    std::istream* istrm2;
    istrm2 = OpenInput(input, false);
    LASReader* reader2 = new LASReader(*istrm2);
    

    long pc_id = CreatePCEntry(  con, 
                    query, 
                    table_name.c_str(),
                    base_table_name.c_str(),
                    point_cloud_name.c_str(),
                    aux_columns.c_str(),
                    aux_values.c_str(),
                    3, // we're assuming 3d for now
                    srid,
                    nCapacity,
                    precision,
                    bUseSolidGeometry,
                    bUse3d,
                    bSetExtents,
                    xmin, xmax, ymin, ymax, zmin, zmax);
                    
    
    std::cout << "Writing " << results.size() << " blocks ..." << std::endl;


    if (!pre_block_sql.empty()) {
        std::cout << "running pre-block-sql ..." << std::endl;

        ostringstream oss;
        oss << pre_block_sql;
        OWStatement* statement = 0;
        statement = Run(con, oss);
        if (statement != 0) {
            delete statement; 
        }
        else {
            std::cout << " pre-block-sql execution failed.." << std::endl;
            return false;
        }
        oss.str("");
        con->Commit();     
    }
    
    long nDimensions = 3;
    InsertBlocks(con, results, nCommitInterval, srid, reader2, table_name.c_str(), precision, pc_id, bUseSolidGeometry, bUse3d, nDimensions);
 
    
    if (!bUseExistingBlockTable) {
        std::cout << "Creating new block table user_sdo_geom_metadata entries and index ..." << std::endl;
        CreateSDOEntry( con, 
                        table_name.c_str(), 
                        query, 
                        srid , 
                        precision, 
                        bUseSolidGeometry,
                        bUse3d,
                        bSetExtents,
                        xmin,
                        xmax,
                        ymin,
                        ymax,
                        zmin,
                        zmax);
                        
        CreateBlockIndex(   con, 
                            table_name.c_str(), 
                            srid, 
                            bUseSolidGeometry, 
                            bUse3d);
    }




    if (!post_sql.empty()) {
        std::cout << "running post-sql ..." << std::endl;

        ostringstream oss;
        oss << post_sql;
        OWStatement* statement = 0;
        statement = Run(con, oss);
        if (statement != 0) {
            delete statement; 
        }
        else {
            std::cout << "post-sql execution failed.." << std::endl;
            return false;
        }
        oss.str("");        
    }

    con->Commit();
}
// 
// select t.x, t.y, t.id from (
// select 
// 
//  sdo_pc_pkg.to_geometry(
//                     a.points,   -- point LOB
//                     a.num_points, -- # of points in the LOB
//                     2,  -- total dimensionality
//                     8265 -- SRID
//                     ) shape from line_27006_reproj a
//                     --)
//      )c ,  TABLE(SDO_UTIL.GETVERTICES(c.shape)) t
// select sdo_pc_pkg.to_geometry(
//                     a.points,   -- point LOB
//                     a.num_points, -- # of points in the LOB
//                     2,  -- total dimensionality
//                     8265 -- SRID
//                     ) shape from line_27006_reproj a
//                     where rownum < 10
                    
