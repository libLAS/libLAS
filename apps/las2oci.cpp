#include "las2oci.hpp"
















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
                     liblas::Bounds<double> const& extent)
{
    
    statement->AddElement(sdo_ordinates, extent.min(0));
    statement->AddElement(sdo_ordinates, extent.min(1));
    if (extent.dimension() > 2)
        statement->AddElement(sdo_ordinates, extent.min(2));
    
    statement->AddElement(sdo_ordinates, extent.max(0));
    statement->AddElement(sdo_ordinates, extent.max(1));
    if (extent.dimension() > 2)
        statement->AddElement(sdo_ordinates, extent.max(2));
        

}


bool FillBlock( OWStatement* statement,
                IndexResult& result, 
                liblas::Reader* reader,
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


    // list<SpatialIndex::id_type> const& ids = result.GetIDs();
    IDVector const& ids = result.GetIDs();
    
    b->pc_ids[index] = pc_id;
    b->srids[index] = (long)srid;
    b->block_ids[index] = result.GetID();
    b->num_points[index] = (long)ids.size();
    
    std::vector<uint8_t>* blob = new std::vector<uint8_t>;
    result.GetData(reader, *blob);

    
    b->blobs[index] = blob;
    // // FIXME: null srids not supported 
    b->srids[index] = srid;
    b->gtypes[index] = gtype;
    // 
    OCIArray* sdo_elem_info=0;
    statement->GetConnection()->CreateType(&sdo_elem_info, statement->GetConnection()->GetElemInfoType());
    SetElements(statement, sdo_elem_info, bUseSolidGeometry);
    // 
    b->element_arrays[index] = sdo_elem_info;
    
    OCIArray* sdo_ordinates=0;
    statement->GetConnection()->CreateType(&sdo_ordinates, statement->GetConnection()->GetOrdinateType());
    // 
    // 
    // 
    SetOrdinates(statement, sdo_ordinates, result.GetBounds());
    
    b->coordinate_arrays[index] = sdo_ordinates;
    

    return true;
}


blocks* CreateBlock(int size)
{
    blocks* b = (blocks*) malloc( sizeof(blocks));
    
    b->pc_ids = (long*) malloc( size * sizeof(long));
    b->block_ids = (long*) malloc ( size * sizeof(long));
    b->num_points = (long*) malloc ( size * sizeof(long));
    b->blobs = (std::vector<uint8_t>**) malloc ( size * sizeof(std::vector<uint8_t>*));
    b->locators =(OCILobLocator**) malloc( sizeof(OCILobLocator*) * size );

    b->srids = (long*) malloc ( size * sizeof(long));
    b->gtypes = (long*) malloc ( size * sizeof(long));

    b->element_arrays = (OCIArray**) malloc ( size * sizeof(OCIArray*));

    b->coordinate_arrays = (OCIArray**) malloc ( size * sizeof(OCIArray*));

    b->size = size;
    
    return b;
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

bool InsertBlock(OWConnection* connection, 
                IndexResult& result, 
                blocks* block,
                long block_index,
                int srid, 
                liblas::Reader* reader, 
                const char* tableName, 
                long precision,
                long pc_id,
                bool bUseSolidGeometry,
                bool bUse3d)
{
    ostringstream oss;

    IDVector const& ids = result.GetIDs();
    // const SpatialIndex::Region* b = result.GetBounds();
    uint32_t num_points = ids.size();


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

    std::vector<uint8_t> data;
    result.GetData(reader, data);
        
    // std::vector<liblas::uint8_t> data;
    // bool gotdata = GetResultData(result, reader, data, 3);
    // if (! gotdata) throw std::runtime_error("unable to fetch point data byte array");

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
    
     // x0, x1, y0, y1, z0, z1, bUse3d
    SetOrdinates(statement, sdo_ordinates, result.GetBounds());
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
                KDXIndexSummary* summary,
                liblas::Reader* reader2, 
                const std::string& table_name, 
                long nCommitInterval, 
                int srid, 
                long precision,
                long pc_id,
                bool bUseSolidGeometry,
                bool bUse3d,
                long nDimensions
                )
{
    ResultsVector::iterator i;

    long commit_interval = 1000;
    blocks* b = CreateBlock(nCommitInterval);

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
    ResultsVector& results = summary->GetResults();
    
    long total_blocks = results.size();
    long blocks_written = 0;
    long blocks_left= 0;
    long to_fill = 0;
    
    
    // for (j = 0; j < total_blocks; j+=commit_interval) {
    //      blocks_left = total_blocks - blocks_written;
    //      if (blocks_left < commit_interval) {
    //          // only fill to this level
    //          to_fill = blocks_left;
    //      } else {
    //          to_fill = commit_interval;
    //      }
    //      
    //      
    //      for (int t = 0; t< to_fill; t++) {
    //          FillBlock(statement,
    //                          results[t], 
    //                          reader2,
    //                          b,
    //                          t,
    //                          srid, 
    //                           pc_id,
    //                           GetGType(bUse3d, bUseSolidGeometry),
    //                           bUseSolidGeometry,
    //                           bUse3d,
    //                           nDimensions
    //                           );
    // 
    // 
    //          t++;
    //      
    //      
    //      }
    //      
    //      
    //      
    //      blocks_written = blocks_written + to_fill;
    //  }


    for (i=results.begin(); i!=results.end(); i++)
    {        
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
        j++;
    }
    return inserted;
}


// bool ArrayInsert( OWConnection* connection,
//         const char* insertStatement,
//         int* panObjId,
//         int* panBlockId,
//         int* panNumPoints,
//         double** ppfdBuffer,
//         int nArrayCols,
//         int nRowsToInsert)
// {
//     OWStatement* statement = connection->CreateStatement( insertStatement );
// 
//     statement->Bind( panObjId );
//     statement->Bind( panBlockId );
//     statement->Bind( panNumPoints );
//     statement->BindArray( ppfdBuffer, nArrayCols );
// 
//     if (statement->Execute(nRowsToInsert) == false) {
//         delete statement;
//         return false;
//     }
// 
//     delete statement;
//     return true;
// }

// bool InsertBlocks(OWConnection* connection,
//                   IndexResult& result,
//                   int srid,
//                   liblas::Reader* reader,
//                   const char* tableName,
//                   long precision,
//                   long pc_id,
//                   bool bUseSolidGeometry,
//                   bool bUse3d,
//                   int max_points_per_row)
// {
//     ostringstream oss;
//     IDVector const& ids = result.GetIDs();
//     liblas::Bounds b = result.GetBounds();
//     uint32_t num_points =ids.size();
//     ostringstream oss_geom;
// 
//     ostringstream s_srid;
//     ostringstream s_gtype;
//     ostringstream s_eleminfo;
//     bool bGeographic = false;
// 
//     if (srid == 0) {
//         s_srid << "NULL";
//         // bUse3d = true;
//         // bUseSolidGeometry = true;
//         }
//     else if (srid == 4326) {
//         // bUse3d = true;
//         // bUseSolidGeometry = true;
//         bGeographic = true;
//         s_srid << srid;
//         // s_srid << "NULL";
//     }
//     else {
//         s_srid << srid;
//         // bUse3d = false;
//         // If the user set an srid and set it to solid, we're still 3d
//         // if (bUseSolidGeometry == true)
//         //     bUse3d = true;
//     }
// 
//     if (bUse3d) {
//         if (bUseSolidGeometry == true) {
//             s_gtype << "3008";
//             s_eleminfo << "(1,1007,3)";
// 
//         } else {
//             s_gtype << "3003";
//             s_eleminfo  << "(1,1003,3)";
// 
//         }
//     } else {
//         if (bUseSolidGeometry == true) {
//             s_gtype << "2008";
//             s_eleminfo << "(1,1007,3)";
// 
//         } else {
//             s_gtype << "2003";
//             s_eleminfo  << "(1,1003,3)";
// 
//         }
//     }
// 
//     double x0, x1, y0, y1, z0, z1;
//     double tolerance = 0.05;
// 
// 
//     x0 = b.min(0);
//     x1 = b.max(0);
//     y0 = b.min(1);
//     y1 = b.max(1);
// 
//     if (bUse3d) {
// 
//         z0 = b.min(2);
//         z1 = b.max(2);
// 
//     } else if (bGeographic) {
//         x0 = -180.0;
//         x1 = 180.0;
//         y0 = -90.0;
//         y1 = 90.0;
//         z0 = 0.0;
//         z1 = 20000.0;
//         tolerance = 0.000000005;
//     } else {
//         z0 = 0.0;
//         z1 = 20000.0;
//     }
// 
// 
//     // std::cout << "use 3d?: " << bUse3d << " srid: " << s_srid.str() << std::endl;
//     oss_geom.setf(std::ios_base::fixed, std::ios_base::floatfield);
//     oss_geom.precision(precision);
// 
//     oss_geom << "mdsys.sdo_geometry(" << s_gtype.str() << ", " << s_srid.str() << ", null, "
//         "mdsys.sdo_elem_info_array" << s_eleminfo.str() << ", "
//         "mdsys.sdo_ordinate_array( ";
// 
//     oss_geom << x0 << ", " << y0 << ", ";
// 
//     if (bUse3d) {
//         oss_geom << z0 << ", ";
//     }
// 
//     oss_geom << x1 << ", " << y1 << "";
// 
//     if (bUse3d) {
//         oss_geom << ", ";
//         oss_geom << z1;
//     }
// 
//     oss_geom << "))";
// 
//     oss << "INSERT INTO " << tableName << " (" <<
//         "OBJ_ID, "
//         "BLK_ID, "
//         "BLK_EXTENT, "
//         "BLK_DOMAIN, "
//         "PCBLK_MIN_RES, "
//         "PCBLK_MAX_RES, "
//         "NUM_POINTS, "
//         "NUM_UNSORTED_POINTS, "
//         "PT_SORT_DIM, "
//         "POINTS) "
//         "VALUES (:1, :2, " << oss_geom.str() << ", NULL, 1, 1, :3, 0, 1, :4)";
// 
//     std::vector<uint8_t> data;
// 
//     result.GetData(reader, data);
// 
// 
//     /*
//      *  Assuming 3 Dimensions data
//      */
// 
//     int nDims = 3;
// 
//     /*
//      *  Allocate fixed width buffer dimensions
//      */
// 
//     double** ppfdBuffer = NULL;
// 
//     int nPoints = num_points;
//     int nFizedRows = nPoints / max_points_per_row;
//     int nFixedCols = min(nPoints, max_points_per_row) * nDims;
// 
//     ppfdBuffer = (double**) malloc( sizeof(double*) * nFizedRows );
// 
//     if( ppfdBuffer == NULL )
//         throw std::runtime_error("unable to allocate memory");
// 
//     for( int i = 0; i < nFizedRows; i++ )
//     {
//         ppfdBuffer[i] = (double*) malloc( sizeof(double) * nFixedCols );
//         
//         if( ppfdBuffer[i] == NULL )
//             throw std::runtime_error("unable to allocate memory");
//     }
// 
//     /*
//      *  Allocate buffer for extra row
//      *
//      *  That row could be the last row, or the only row. In both
//      *  case it should be also smaller than MAX_POINTS_PER_ROW.
//      */
// 
//     int nExtraPoints = num_points - (nFizedRows * max_points_per_row);
//     int nExtraCols = nExtraPoints * nDims;
// 
//     double* ppfdExtra[1];
//     
//     if( nExtraPoints > 0 )
//     {
//         ppfdExtra[0] = (double*) malloc( sizeof(double) * nExtraCols );
//     }
// 
//     /*
//      *  Load points data swapping words as nedded
//      */
// 
//     int index = 0;
// 
//     for( int i = 0; i < nFizedRows; i++)
//     {
//         for( int j = 0; j < nFixedCols; j++)
//         {
//             ppfdBuffer[i][j] = data.at(index++);
//         }
// 
//         GDALSwapWords( ppfdBuffer[i], sizeof(double), nFixedCols, sizeof(double) );
//     }
// 
//     if( nExtraPoints > 0 )
//     {
//         for( int i = 0; i < nExtraCols; i++)
//         {
//             ppfdExtra[0][i] = data.at(index++);
//         }
//         
//         GDALSwapWords( ppfdExtra[0], sizeof(double), nExtraCols, sizeof(double) );
//     }
// 
//     /*
//      *  Create array for each other column
//      */
// 
//     int  nRows = max(nFizedRows,1); /* it could be just one "extra" row */
// 
//     int* panObjId = (int*) malloc( sizeof(long) * nRows );
//     int* panBlockId = (int*) malloc( sizeof(long) * nRows );
//     int* panNumPoints = (int*) malloc( sizeof(long) * nRows );
// 
//     for( int i = 0; i < nRows; i++ )
//     {
//         panObjId[i] = pc_id;
//         panBlockId[i] = result.GetID();
//         panNumPoints[i] = max_points_per_row;
//     }
// 
//     /*
//      *  Insert nFizedRows at once
//      */
// 
//     if( nFizedRows > 0 )
//     {
//         if( ArrayInsert( connection,
//                 oss.str().c_str(),
//                 panObjId,
//                 panBlockId,
//                 panNumPoints,
//                 ppfdBuffer,
//                 nFixedCols,
//                 nFizedRows ) == false )
//         {
//             throw std::runtime_error("error writing blocks.");
//         }
//     }
// 
//     /*
//      *  Insert one extra row
//      */
// 
//     if( nExtraPoints > 0 )
//     {
//         panNumPoints[0] = nExtraPoints;
// 
//         if( ArrayInsert( connection,
//                 oss.str().c_str(),
//                 panObjId,
//                 panBlockId,
//                 panNumPoints,
//                 ppfdExtra,
//                 nExtraCols, 1 ) == false )
//         {
//             throw std::runtime_error("error writing single block.");
//         }
//     }
// 
//     connection->Commit();
// 
//     free( panObjId );
//     free( panBlockId );
//     free( panNumPoints );
//     for( int i = 0; i < nFizedRows; i++ )
//     {
//         free( ppfdBuffer[i] );
//     }
//     free( ppfdBuffer );
//     if( ppfdExtra[0] != NULL)
//     {
//         free( ppfdExtra[0] );
//     }
//     return true;
// }

bool CreateSDOEntry(    OWConnection* connection, 
                        const char* tableName, 
                        KDXIndexSummary* query, 
                        long srid, 
                        long precision,
                        bool bUseSolidGeometry,
                        bool bUse3d,
                        bool bSetExtents,
                        liblas::Bounds<double> const& bounds)
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
    liblas::Bounds<double> e = *query->bounds.get();

    if (IsGeographic(connection, srid)) {
        e.min(0,-180.0); e.max(0,180.0);
        e.min(1,-90.0); e.max(1,90.0);
        e.min(2,0.0); e.max(2,20000.0);

        tolerance = 0.000000005;
    }


    if (bSetExtents){
        e = bounds;
        // e->x0 = bounds.min(0); e->x1 = bounds.max(0);
        // e->y0 = bounds.min(1); e->y1 = bounds.max(1);
        // e->z0 = bounds.min(2); e->z1 = bounds.max(2);
    }     

     
    oss <<  "INSERT INTO user_sdo_geom_metadata VALUES ('" << tableName <<
        "','blk_extent', MDSYS.SDO_DIM_ARRAY(";
    
    oss << "MDSYS.SDO_DIM_ELEMENT('X', " << e.min(0) << "," << e.max(0) <<"," << tolerance << "),"
           "MDSYS.SDO_DIM_ELEMENT('Y', " << e.min(1) << "," << e.max(1) <<"," << tolerance << ")";
           
    if (bUse3d) {
        oss << ",";
        oss <<"MDSYS.SDO_DIM_ELEMENT('Z', "<< e.min(2) << "," << e.max(2) << "," << tolerance << ")";
    }
    oss << ")," << s_srid.str() << ")";
    
    statement = RunSQL(connection, oss);
    if (statement != 0) delete statement; else return false;
    oss.str("");
    
    
    return true;
        
}





long CreatePCEntry( OWConnection* connection, 
                    KDXIndexSummary* query, 
                    std::string blkTableName, 
                    std::string pcTableName, 
                    std::string cloudColumnName,
                    std::string aux_columns,
                    std::string aux_values,
                    int nDimension, 
                    int srid,
                    int blk_capacity,
                    long precision,
                    bool bUseSolidGeometry,
                    bool bUse3d)
{
    ostringstream oss;

    OWStatement* statement = 0;

    oss.setf(std::ios_base::fixed, std::ios_base::floatfield);
    oss.precision(precision);
    
    std::string blkTableName_u = to_upper(blkTableName);    
    std::string pcTableName_u = to_upper(pcTableName);
    std::string cloudColumnName_u = to_upper(cloudColumnName);
    std::string aux_columns_u = to_upper(aux_columns);

    ostringstream columns;
    ostringstream values;
    
    if (!aux_columns_u.empty() ) {
        columns << cloudColumnName_u << "," << aux_columns_u;
    
        values << "pc," << aux_values;
    } else {
        columns << cloudColumnName_u;
        values << "pc";
    }


    ostringstream s_srid;
    ostringstream s_gtype;
    ostringstream s_eleminfo;
    ostringstream s_geom;


    IsGeographic(connection, srid);

    if (srid == 0)
    {
        s_srid << "NULL";
    }
    else
    {
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
    

    // extent* e = GetExtent(  *(query->bounds.get()), bUse3d );
    liblas::Bounds<double> e =  *(query->bounds.get());

    s_geom << "           mdsys.sdo_geometry("<<s_gtype.str() <<", "<<s_srid.str()<<", null,\n"
"              mdsys.sdo_elem_info_array"<< s_eleminfo.str() <<",\n"
"              mdsys.sdo_ordinate_array(\n";

    s_geom << e.min(0) << "," << e.min(1) << ",";

    if (bUse3d) {
        s_geom << e.min(2) << ",";
    }
    
    s_geom << e.max(0) << "," << e.max(1);

    if (bUse3d) {
        s_geom << "," << e.max(2);
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
"  insert into " << pcTableName_u << " ( ID, "<< columns.str() <<
        ") values ( pc.pc_id, " << values.str() << ");\n"

"  "
"end;\n";


    int pc_id = 0;
    long output = 0;
    statement = connection->CreateStatement(oss.str().c_str());
    statement->Bind(&pc_id);
    if (statement->Execute() == false) {

        std::cout << "statement execution failed "  << CPLGetLastErrorMsg() << std::endl;
        delete statement;
        return 0;
    }
    output = pc_id;

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
    
    uint32_t nCapacity = 10000;

    int srid = 0;
    long precision = 8;
    
    liblas::Bounds<double> global_extent;
    
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
        else if (   strcmp(argv[i],"--srid") == 0  ||
                    strcmp(argv[i],"-srid") == 0   ||
                    strcmp(argv[i],"-s") == 0 
                )
        {
            i++;
            srid=atoi(argv[i]);
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
            global_extent.min(0, atof(argv[i]));
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--xmax") == 0  ||
                    strcmp(argv[i],"-xmax") == 0
                )
        {
            i++;
            global_extent.max(0, atof(argv[i]));
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--ymin") == 0  ||
                    strcmp(argv[i],"-ymin") == 0
                )
        {
            i++;
            global_extent.min(1, atof(argv[i]));
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--ymax") == 0  ||
                    strcmp(argv[i],"-ymax") == 0
                )
        {
            i++;
            global_extent.max(1, atof(argv[i]));
            bSetExtents = true;
        }
        else if (   strcmp(argv[i],"--zmin") == 0  ||
                    strcmp(argv[i],"-zmin") == 0
                )
        {
            i++;
            global_extent.min(2, atof(argv[i]));
            bSetExtents = true;
        }

        else if (   strcmp(argv[i],"--zmax") == 0  ||
                    strcmp(argv[i],"-zmax") == 0
                )
        {
            i++;
            global_extent.max(2, atof(argv[i]));
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

    if (block_table_name.size() == 0) {
        // change filename foo.las -> foo for an appropriate
        // block tablename for oracle... must be less than 30 characters
        // and no extraneous characters.
        string::size_type dot_pos = input.find_first_of(".");
        block_table_name = input.substr(0,dot_pos);
    } else {
        block_table_name = block_table_name;
    }
    string::size_type slash_pos = connection.find("/",0);
    username = connection.substr(0,slash_pos);
    string::size_type at_pos = connection.find("@",slash_pos);
//    std::cout <<"slash_pos: " << slash_pos << " at_pos: " << at_pos<<std::endl;
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

    // std::istream* istrm;
    // try {
    //         istrm = OpenInput(input, false);
    // } catch (std::exception const& e)
    // {
    //     std::cout << e.what() << std::endl;
    //     std::cout << "exiting..." << std::endl;
    //     exit(-1);
    // }

    
    con->StartTransaction();

    if (bDropTable) {
        std::cout << "dropping existing tables..." << std::endl;

        WipeBlockTable(con, block_table_name, base_table_name, point_cloud_name);
    }
    
    if (!pre_sql.empty()) {
        std::cout << "running pre-sql ..." << std::endl;
        ostringstream oss;
        oss << pre_sql;
        OWStatement* statement = 0;
        statement = RunSQL(con, oss);
        if (statement != 0) {
            delete statement; 
        }
        else {
            std::cout << "pre-sql execution failed.." << std::endl;
            return false;
        }
        oss.str("");        
    }
    if (!BlockTableExists(con, block_table_name.c_str()))
        CreateBlockTable(con, block_table_name);
    else {
        bUseExistingBlockTable = true;
        std::cout << "Using existing block table ... " << std::endl;

    }

    KDXIndexSummary* query = 0;
    if (!KDTreeIndexExists(input)) {
        std::cout << "KDTree .kdx file does not exist for file, unable to proceed" << std::endl;
        exit(1);  
    } else {
        std::cout << "Using kdtree ... " << std::endl;
        std::ostringstream os;
        os << input << ".kdx" ;
        
        std::istream* kdx = OpenInput(os.str(), false);
        query = new KDXIndexSummary(*kdx);
    }

    ResultsVector& results = query->GetResults();
    
    ResultsVector::iterator i;
    
    std::istream* istrm2;
    istrm2 = OpenInput(input, false);
    liblas::Reader* reader2 = new liblas::Reader(*istrm2);
    

    long pc_id = CreatePCEntry(  con, 
                    query, 
                    block_table_name,
                    base_table_name,
                    point_cloud_name,
                    aux_columns,
                    aux_values,
                    3, // we're assuming 3d for now
                    srid,
                    nCapacity,
                    precision,
                    bUseSolidGeometry,
                    bUse3d);
                    
    
    std::cout << "Writing " << results.size() << " blocks ..." << std::endl;


    if (!pre_block_sql.empty()) {
        std::cout << "running pre-block-sql ..." << std::endl;

        ostringstream oss;
        oss << pre_block_sql;
        OWStatement* statement = 0;
        statement = RunSQL(con, oss);
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
                // 
                // OWConnection* con, 
                // KDXIndexSummary* summary,
                // liblas::Reader* reader2, 
                // const std::string& table_name, 
                // long nCommitInterval, 
                // int srid, 
                // long precision,
                // long pc_id,
                // bool bUseSolidGeometry,
                // bool bUse3d,

                // long nDimensions    

    InsertBlocks(con,
                 query,
                 reader2,
                 block_table_name,
                 nCommitInterval,
                 srid,
                 precision,
                 pc_id,
                 bUseSolidGeometry,
                 bUse3d,
                 nCapacity);
 
    
    if (!bUseExistingBlockTable) {
        std::cout << "Creating new block table user_sdo_geom_metadata entries and index ..." << std::endl;
        CreateSDOEntry( con, 
                        block_table_name.c_str(), 
                        query, 
                        srid , 
                        precision, 
                        bUseSolidGeometry,
                        bUse3d,
                        bSetExtents,
                        global_extent);
                        
        CreateBlockIndex(   con, 
                            block_table_name.c_str(), 
                            srid,  
                            bUse3d);
    }




    if (!post_sql.empty()) {
        std::cout << "running post-sql ..." << std::endl;

        ostringstream oss;
        oss << post_sql;
        OWStatement* statement = 0;
        statement = RunSQL(con, oss);
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
                    
