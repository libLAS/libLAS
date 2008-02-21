/***************************************************************************
 * $Id$
 * $Date: 2008-02-09 19:39:48 +0100 (Sat, 09 Feb 2008) $
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS information reporting and setting
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

/* TODO: Remove before releasing */
#if defined(_MSC_VER) && defined(USE_VLD)
#include <vld.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <liblas.h>

void usage()
{
    fprintf(stderr,"----------------------------------------------------------\n");
    fprintf(stderr,"    lasinfo usage:\n");
    fprintf(stderr,"----------------------------------------------------------\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"Printing simple header info:\n");
    fprintf(stderr,"  lasinfo lidar.las\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"Checking point information:\n");
    fprintf(stderr,"  lasinfo --check lidar.las\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"Reading from stdin:\n");
    fprintf(stderr,"  lasinfo --stdin -c < lidar.las\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"Repairing header info to reflect point data:\n");
    fprintf(stderr,"  lasinfo -i lidar.las --repair\n");
    fprintf(stderr,"  lasinfo -i lidar.las --repair_bounding_box\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"Altering header inf:\n");
    fprintf(stderr, "  lasinfo -i lidar.las --system_identifier \"hello world!\" "
                    "--generating_software \"this is a test (-:\"  --file_creation 8 2007\n");
    fprintf(stderr,"\n");

    fprintf(stderr, "For more information, see the full documentation for lasinfo at:\n"
                    " http://liblas.org/browser/trunk/doc/lasinfo.txt\n");
    fprintf(stderr,"----------------------------------------------------------\n");

    exit(1);
}

void print_error(char* message) {

    if (LASError_GetErrorCount()) {
        fprintf(stdout, 
            "%s: %s (%d) from method %s\n",
            message,
            LASError_GetLastErrorMsg(),
            LASError_GetLastErrorNum(),
            LASError_GetLastErrorMethod()
        ); 
    } else {
        fprintf(stdout, 
            "You have encountered an error. '%s'\n",
            message
        );         
    }

}

typedef struct  {

    double t;
    double x, y, z;
    uint16_t intensity;
    uint8_t cls;
    uint8_t scan_angle;
    uint8_t user_data;
    uint16_t retnum;
    uint16_t numret;
    uint16_t scandir;
    uint16_t fedge;
    long rgpsum;    
    int number_of_point_records;
    int number_of_points_by_return[8];
    int number_of_returns_of_given_pulse[8];
    int classification[32];
    int classification_synthetic;
    int classification_keypoint;
    int classification_withheld;
    LASPointH pmax;
    LASPointH pmin;
} PointSummary;

PointSummary* SummarizePoints(LASReaderH reader) {
    
    PointSummary* summary;
    LASPointH p;
    uint8_t cls = 0;
    int i = 0;

    summary = (PointSummary*) malloc(sizeof(PointSummary));

    summary->number_of_point_records = 0;
    for (i=0; i<8;i++)
        summary->number_of_points_by_return[i] = 0;
    for (i=0; i<8;i++)
        summary->number_of_returns_of_given_pulse[i] = 0;
    for (i=0; i<32;i++)
        summary->classification[i] = 0;
    summary->classification_synthetic = 0;
    summary->classification_keypoint = 0;
    summary->classification_withheld = 0;
        


    p  = LASReader_GetNextPoint(reader);
    
    if (!p) {
        print_error("Not able to fetch a point.  LASReaderH is invalid");
        exit(-1);
    }

    summary->pmin = LASPoint_Copy(p);
    summary->pmax = LASPoint_Copy(p);

    while (p)
    {
        i++;

        
        summary->x = LASPoint_GetX(p);
        LASPoint_SetX(summary->pmin, MIN(summary->x, LASPoint_GetX(summary->pmin)));
        LASPoint_SetX(summary->pmax, MAX(summary->x, LASPoint_GetX(summary->pmax)));

        summary->y = LASPoint_GetY(p);
        LASPoint_SetY(summary->pmin, MIN(summary->y, LASPoint_GetY(summary->pmin)));
        LASPoint_SetY(summary->pmax, MAX(summary->y, LASPoint_GetY(summary->pmax)));

        summary->z = LASPoint_GetZ(p);
        LASPoint_SetZ(summary->pmin, MIN(summary->z, LASPoint_GetZ(summary->pmin)));
        LASPoint_SetZ(summary->pmax, MAX(summary->z, LASPoint_GetZ(summary->pmax)));

        summary->intensity = LASPoint_GetIntensity(p);
        LASPoint_SetIntensity(summary->pmin, MIN(summary->intensity, LASPoint_GetIntensity(summary->pmin)));
        LASPoint_SetIntensity(summary->pmax, MAX(summary->intensity, LASPoint_GetIntensity(summary->pmax)));

        summary->t = LASPoint_GetTime(p);
        LASPoint_SetTime(summary->pmin, MIN(summary->t, LASPoint_GetTime(summary->pmin)));
        LASPoint_SetTime(summary->pmax, MAX(summary->t, LASPoint_GetTime(summary->pmax)));

        summary->retnum = LASPoint_GetReturnNumber(p);
        LASPoint_SetReturnNumber(summary->pmin, MIN(summary->retnum, LASPoint_GetReturnNumber(summary->pmin)));
        LASPoint_SetReturnNumber(summary->pmax, MAX(summary->retnum, LASPoint_GetReturnNumber(summary->pmax)));                    

        summary->numret = LASPoint_GetNumberOfReturns(p);
        LASPoint_SetNumberOfReturns(summary->pmin, MIN(summary->numret, LASPoint_GetNumberOfReturns(summary->pmin)));
        LASPoint_SetNumberOfReturns(summary->pmax, MAX(summary->numret, LASPoint_GetNumberOfReturns(summary->pmax)));  
        
        summary->scandir = LASPoint_GetScanDirection(p);
        LASPoint_SetScanDirection(summary->pmin, MIN(summary->scandir, LASPoint_GetScanDirection(summary->pmin)));
        LASPoint_SetScanDirection(summary->pmax, MAX(summary->scandir, LASPoint_GetScanDirection(summary->pmax)));  
        
        summary->fedge = LASPoint_GetFlightLineEdge(p);
        LASPoint_SetFlightLineEdge(summary->pmin, MIN(summary->fedge, LASPoint_GetFlightLineEdge(summary->pmin)));
        LASPoint_SetFlightLineEdge(summary->pmax, MAX(summary->fedge, LASPoint_GetFlightLineEdge(summary->pmax)));  

        summary->scan_angle = LASPoint_GetScanAngleRank(p);
        LASPoint_SetScanAngleRank(summary->pmin, MIN(summary->scan_angle, LASPoint_GetScanAngleRank(summary->pmin)));
        LASPoint_SetScanAngleRank(summary->pmax, MAX(summary->scan_angle, LASPoint_GetScanAngleRank(summary->pmax)));  

        summary->user_data = LASPoint_GetUserData(p);
        LASPoint_SetUserData(summary->pmin, MIN(summary->user_data, LASPoint_GetUserData(summary->pmin)));
        LASPoint_SetUserData(summary->pmax, MAX(summary->user_data, LASPoint_GetUserData(summary->pmax)));  
                                
        summary->number_of_point_records = i;
        summary->number_of_points_by_return[LASPoint_GetReturnNumber(p)-1]++;
        summary->number_of_returns_of_given_pulse[LASPoint_GetNumberOfReturns(p)]++;
        
        cls = LASPoint_GetClassification(p);
        summary->classification[(cls & 31)]++;            
        if (cls & 32) summary->classification_synthetic++;          
        if (cls & 64) summary->classification_keypoint++; 
        if (cls & 128) summary->classification_withheld++;
          
          /*
          // if (lasreader->point.point_source_ID < point_min.point_source_ID) point_min.point_source_ID = lasreader->point.point_source_ID;
          // else if (lasreader->point.point_source_ID > point_max.point_source_ID) point_max.point_source_ID = lasreader->point.point_source_ID;

          */
        p  = LASReader_GetNextPoint(reader);
    }
    
    return summary; 
}
static const char * LASPointClassification [] = {
  "Created, never classified",
  "Unclassified",
  "Ground",
  "Low Vegetation",
  "Medium Vegetation",
  "High Vegetation",
  "Building",
  "Low Point (noise)",
  "Model Key-point (mass point)",
  "Water",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Overlap Points",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition",
  "Reserved for ASPRS Definition"
};

void print_point_summary(PointSummary* summary, LASHeaderH header) {

    long rgpsum = 0;
    int i = 0;

    if (!summary) {print_error("Point Summary does not exist!"); exit(-1);}

    fprintf(stderr, "\n---------------------------------------------------------\n");
    fprintf(stderr, "  Point Inspection Summary\n");
    fprintf(stderr, "---------------------------------------------------------\n");
    
    fprintf(stderr, " Header Point Count: %d\n",
                    LASHeader_GetPointRecordsCount(header));
                    
    fprintf(stderr, " Actual Point Count: %d\n", 
                    summary->number_of_point_records);

    fprintf(stderr, "\n---------------------------------------------------------\n");
    fprintf(stderr, "  Minimum and Maximum Attributes (min,max)\n");
    fprintf(stderr, "---------------------------------------------------------\n");
    
    fprintf(stderr, " Min X,Y,Z: \t\t%.6f,%.6f,%.6f\n", 
                    LASPoint_GetX(summary->pmin),
                    LASPoint_GetY(summary->pmin),
                    LASPoint_GetZ(summary->pmin)
                    );
    fprintf(stderr, " Max X,Y,Z: \t\t%.6f,%.6f,%.6f\n", 
                    LASPoint_GetX(summary->pmax),
                    LASPoint_GetY(summary->pmax),
                    LASPoint_GetZ(summary->pmax)
                    );
    fprintf(stderr, " Bounding Box:\t\t%.2f,%.2f,%.2f,%.2f\n",
                    LASPoint_GetX(summary->pmin),
                    LASPoint_GetY(summary->pmin),
                    LASPoint_GetX(summary->pmax),
                    LASPoint_GetY(summary->pmax)
                    );
    fprintf(stderr, " Time:\t\t\t%.6f,%.6f\n", 
                    LASPoint_GetTime(summary->pmin),
                    LASPoint_GetTime(summary->pmax)
                    );
    fprintf(stderr, " Return Number:\t\t%d,%d\n", 
                    LASPoint_GetReturnNumber(summary->pmin),
                    LASPoint_GetReturnNumber(summary->pmax)
                    );
    fprintf(stderr, " Return Count:\t\t%d,%d\n", 
                    LASPoint_GetNumberOfReturns(summary->pmin),
                    LASPoint_GetNumberOfReturns(summary->pmax)
                    );
    fprintf(stderr, " Flightline Edge:\t%d,%d\n", 
                    LASPoint_GetFlightLineEdge(summary->pmin),
                    LASPoint_GetFlightLineEdge(summary->pmax)
                    );
    fprintf(stderr, " Intensity:\t\t%d,%d\n",
                    LASPoint_GetIntensity(summary->pmin),
                    LASPoint_GetIntensity(summary->pmax)
                    );
    fprintf(stderr, " Scan Direction Flag:\t%d,%d\n",
                    LASPoint_GetScanDirection(summary->pmin),
                    LASPoint_GetScanDirection(summary->pmax)
                    );
    fprintf(stderr, " Classification:\t%d,%d\n",
                    LASPoint_GetClassification(summary->pmin),
                    LASPoint_GetClassification(summary->pmax)
                    );
                    
    fprintf(stderr, "\n---------------------------------------------------------\n");
    fprintf(stderr, "  Number of Returns by Pulse\n");
    fprintf(stderr, "---------------------------------------------------------\n");
    
    for (i = 1; i < 8; i++) {
        rgpsum = rgpsum + summary->number_of_returns_of_given_pulse[i];
        fprintf(stderr, "\t(%d) %d", i,summary->number_of_returns_of_given_pulse[i]);
    }
    fprintf(stderr, "\n Actual Point Count: %ld\n", rgpsum); 


    for (i = 0; i < 5; i++) {
        if (LASHeader_GetPointRecordsByReturnCount(header, i) != summary->number_of_points_by_return[i]) 
        {
            fprintf(stderr, " actual number of points by return is different (actual, header):"); 
            for (i = 0; i < 5; i++) {
                fprintf(stderr, " (%d,%d)", 
                        summary->number_of_points_by_return[i],
                        LASHeader_GetPointRecordsByReturnCount(header, i)
                        );
            } 
            fprintf(stderr, "\n");
        }
    }

    fprintf(stderr, "\n---------------------------------------------------------\n");
    fprintf(stderr, "  Point Classifications\n");
    fprintf(stderr, "---------------------------------------------------------\n");

    for (i = 0; i < 32; i++) {
        if (summary->classification[i]) {
            fprintf(stderr, "\t%8d %s (%d)\n", 
                            summary->classification[i], 
                            LASPointClassification[i], 
                            i);
        }
    }

    if (summary->classification_synthetic || summary->classification_keypoint ||  summary->classification_withheld) {
        fprintf(stderr, "\n---------------------------------------------------------\n");
        fprintf(stderr, "  Point Classification Histogram\n");
        fprintf(stderr, "---------------------------------------------------------\n");

        if (summary->classification_synthetic) fprintf(stderr, " +-> flagged as synthetic: %d\n", summary->classification_synthetic);
        if (summary->classification_keypoint) fprintf(stderr,  " +-> flagged as keypoints: %d\n", summary->classification_keypoint);
        if (summary->classification_withheld) fprintf(stderr,  " +-> flagged as withheld:  %d\n", summary->classification_withheld);
    }         
}

void print_header(LASHeaderH header, const char* file_name) {

    char *pszSignature = NULL;
    char *pszProjectId = NULL;
    char *pszSystemId = NULL;
    char *pszSoftwareId = NULL;

    pszSignature = LASHeader_GetFileSignature(header);
    pszProjectId = LASHeader_GetProjectId(header);
    pszSystemId = LASHeader_GetSystemId(header);
    pszSoftwareId = LASHeader_GetSoftwareId(header);

    fprintf(stderr, "\n---------------------------------------------------------\n");
    fprintf(stderr, "  Header Summary\n");
    fprintf(stderr, "---------------------------------------------------------\n");


    fprintf(stdout, "  File Name: %s\n", file_name);
    
    if (strcmp(pszSignature,"LASF") !=0) {
        print_error("File signature is not 'LASF'... aborting");
        exit(-1);
    }

    fprintf(stdout, "  file source ID:            %ld\n", 
                    LASHeader_GetFileSourceId(header) ) ;

    fprintf(stdout, "  reserved:                  %d\n", 
                    LASHeader_GetReserved(header) );

    fprintf(stdout, "  project ID GUID:           '%s'\n", 
                    pszProjectId);

    fprintf(stdout, "  version major.minor:       %d.%d\n", 
                    LASHeader_GetVersionMajor(header), 
                    LASHeader_GetVersionMinor(header));

    fprintf(stdout, "  system_identifier:         '%s'\n", 
                    pszSystemId);

    fprintf(stdout, "  generating_software:       '%s'\n", 
                    pszSoftwareId);

    fprintf(stdout, "  file creation day/year:    %d/%d\n", 
                    LASHeader_GetCreationDOY(header), 
                    LASHeader_GetCreationYear(header));

    fprintf(stdout, "  header size                %d\n", 
                    LASHeader_GetHeaderSize(header));

    fprintf(stdout, "  offset to point data       %d\n", 
                    LASHeader_GetDataOffset(header));

    fprintf(stdout, "  number var. length records %d\n", 
                    LASHeader_GetRecordsCount(header));

    fprintf(stdout, "  point data format          %d\n", 
                    LASHeader_GetDataFormatId(header));

    fprintf(stdout, "  point data record length   %d\n", 
                    LASHeader_GetDataRecordLength(header));

    fprintf(stdout, "  number of point records    %d\n", 
                    LASHeader_GetPointRecordsCount(header));

    fprintf(stdout, "  number of points by return %d %d %d %d %d\n", 
                    LASHeader_GetPointRecordsByReturnCount(header, 0), 
                    LASHeader_GetPointRecordsByReturnCount(header, 1), 
                    LASHeader_GetPointRecordsByReturnCount(header, 2), 
                    LASHeader_GetPointRecordsByReturnCount(header, 3), 
                    LASHeader_GetPointRecordsByReturnCount(header, 4));

    fprintf(stdout, "  scale factor x y z         %.6f %.6f %.6f\n", 
                    LASHeader_GetScaleX(header), 
                    LASHeader_GetScaleY(header),
                    LASHeader_GetScaleZ(header));

    fprintf(stdout, "  offset x y z               %.6f %.6f %.6f\n", 
                    LASHeader_GetOffsetX(header), 
                    LASHeader_GetOffsetY(header), 
                    LASHeader_GetOffsetZ(header));

    fprintf(stdout, "  min x y z                  %.6f %.6f %.6f\n",
                    LASHeader_GetMinX(header), 
                    LASHeader_GetMinY(header), 
                    LASHeader_GetMinZ(header));

    fprintf(stdout, "  max x y z                  %.6f %.6f %.6f\n", 
                    LASHeader_GetMaxX(header), 
                    LASHeader_GetMaxY(header), 
                    LASHeader_GetMaxZ(header));

    free(pszSignature);
    free(pszProjectId);
    free(pszSystemId);
    free(pszSoftwareId);
    
    
}

int main(int argc, char *argv[])
{
    int i;
    char* file_name = NULL;
    
    LASReaderH reader = NULL;
    LASHeaderH header = NULL;
    LASWriterH writer = NULL;
    
    int check_points = FALSE;
    int repair_header = FALSE;
    int change_header = FALSE;
    int repair_bounding_box = FALSE;
    int use_stdin = FALSE;
    int update_return_counts = FALSE;


    char *system_identifier = NULL;
    char *generating_software = NULL;
    uint8_t file_creation_day = 0;
    uint8_t file_creation_year = 0;
    
    int err=0;

    PointSummary* summary = NULL;

    for (i = 1; i < argc; i++)
    {
        if (    strcmp(argv[i],"-h") == 0 ||
                strcmp(argv[i],"--help") == 0
            )
        {
            usage();
        }
        else if (   strcmp(argv[i],"--input") == 0  ||
                    strcmp(argv[i],"--in") == 0     ||
                    strcmp(argv[i],"-i") == 0       ||
                    strcmp(argv[i],"-in") == 0
                )
        {
            i++;
            file_name = argv[i];
        }
        else if (   strcmp(argv[i], "--points") == 0        || 
                    strcmp(argv[i], "--check") == 0         || 
                    strcmp(argv[i], "--check_points") == 0  ||
                    strcmp(argv[i], "-c") == 0          || 
                    strcmp(argv[i], "-points") == 0         || 
                    strcmp(argv[i], "-check") == 0          || 
                    strcmp(argv[i], "-check_points") == 0  
                )
        {
            check_points = TRUE;
        }
        else if (   strcmp(argv[i], "--nocheck") == 0 ||
                    strcmp(argv[i], "-nocheck") == 0 
                )
        {
            check_points = FALSE;
        }
        else if (   strcmp(argv[i], "--stdin") == 0 ||
                    strcmp(argv[i], "-ilas") == 0
                ) 
        {
            use_stdin = TRUE;
        }

        else if (   strcmp(argv[i], "--repair") == 0        ||
                    strcmp(argv[i], "-r") == 0  ||
                    strcmp(argv[i], "-repair_header") == 0  ||
                    strcmp(argv[i], "-repair") == 0
                ) 
        {
            repair_header = TRUE;
            check_points = TRUE;
        }

        else if (   strcmp(argv[i], "--repair_bb") == 0             ||
                    strcmp(argv[i], "--repair_bounding_box") == 0   ||
                    strcmp(argv[i], "--repair_boundingbox") == 0    ||
                    strcmp(argv[i], "-repair_bb") == 0              ||
                    strcmp(argv[i], "-repair_bounding_box") == 0    ||
                    strcmp(argv[i], "-repair_boundingbox") == 0     ||
                    strcmp(argv[i], "-repair") == 0     ||
                    strcmp(argv[i], "-rb") == 0

                ) 
        {
            repair_bounding_box = TRUE;
            check_points = TRUE;
        }

        else if (   strcmp(argv[i],"--system_identifier") == 0   ||
                    strcmp(argv[i],"-system_identifier") == 0   ||
                    strcmp(argv[i],"-s") == 0   ||
                    strcmp(argv[i],"-sys_id") == 0)
        {
            i++;
            system_identifier = (char*) malloc(31 * sizeof(char));
            strcpy(system_identifier, argv[i]);
            change_header = TRUE;
        }

        else if (   strcmp(argv[i],"--generating_software") == 0   ||
                    strcmp(argv[i],"-generating_software") == 0   ||
                    strcmp(argv[i],"-g") == 0   ||
                    strcmp(argv[i],"-gen_soft") == 0)
        {
            i++;
            generating_software = (char*) malloc(31*sizeof(char));
            strcpy(generating_software, argv[i]);
            change_header = TRUE;
        }

        else if (   strcmp(argv[i],"--file_creation") == 0   ||
                    strcmp(argv[i],"-file_creation") == 0)
        {
            i++;
            file_creation_day = (unsigned short)atoi(argv[i]);
            i++;
            file_creation_year = (unsigned short)atoi(argv[i]);
            change_header = TRUE;
        }
                      
        else if (i == argc - 1 && file_name == NULL)
        {
            file_name = argv[i];
        } 
        else
        {
            usage();
            exit(-1);
        }
    }
      
    if (use_stdin) {
        file_name = "stdin";
    }
    
    if (!file_name) {
        print_error("No filename was provided to be opened");
        usage();
        exit(-1);
    }
    
    reader = LASReader_Create(file_name);
    if (!reader) { 
        print_error("Could not open file ");
        exit(-1);
    } 
      
    header = LASReader_GetHeader(reader);
    if (!header) { 
        print_error("Could not get LASHeader ");
        exit(-1);
    } 

    
    
    print_header(header, file_name);
    
    if (change_header) {
        if (system_identifier) {
            err = LASHeader_SetSystemId (header, system_identifier);
            if (err) print_error("Could not set SystemId");
        }
        if (generating_software) {
            err = LASHeader_SetSoftwareId(header, generating_software);
            if (err) print_error("Could not set SoftwareId");
        }
        if ( file_creation_day || file_creation_year) {
            err = LASHeader_SetCreationDOY(header, file_creation_day);
            if (err) print_error("Could not set file creation day");
            err = LASHeader_SetCreationYear(header, file_creation_year);
            if (err) print_error("Could not set file creation year");
        }

        /* We need to wipe out the reader and make a writer. */
        if (reader) {
            LASReader_Destroy(reader);
            reader = NULL;
        }
        
        writer = LASWriter_Create(file_name, header);
        if (!writer) {
            print_error("Problem creating LASWriterH object");
	        LASHeader_Destroy(header);
            header = NULL;
            exit(-1);
        }

        if (writer) LASWriter_Destroy(writer);
        writer = NULL;
        if (header) LASHeader_Destroy(header);
        header = NULL;
        exit(0);
    }
    
    if (check_points)
    {
        if (!reader) {
            reader = LASReader_Create(file_name);
            if (!reader) { 
                print_error("Could not open file ");
                exit(-1);
            } 
        }
  
        if (! header) {
            header = LASReader_GetHeader(reader);
            if (!header) { 
                print_error("Could not get LASHeader ");
                exit(-1);
            } 
        } 
        
        if (!summary)
            summary = SummarizePoints(reader);
        print_point_summary(summary, header);
        
        if (repair_header) {
            /* We need to wipe out the reader and make a writer. */
            if (reader) {
                LASReader_Destroy(reader);
                reader = NULL;
            }
            
            if (use_stdin) {
                print_error("Cannot update header information on piped input!");
                exit(-1);
            }

  
            if (! header) {
                header = LASReader_GetHeader(reader);
                if (!header) { 
                    print_error("Could not get LASHeader ");
                    exit(-1);
            } 
            
            if (repair_bounding_box) {
                err = LASHeader_SetMin( header, 
                                        LASPoint_GetX(summary->pmin), 
                                        LASPoint_GetY(summary->pmin), 
                                        LASPoint_GetZ(summary->pmin)
                                      );
                if (err) {
                    print_error("Could not set minimum for header ");
                    exit(-1);
                }
                err = LASHeader_SetMax( header, 
                                        LASPoint_GetX(summary->pmax), 
                                        LASPoint_GetY(summary->pmax), 
                                        LASPoint_GetZ(summary->pmax)
                                      );
                if (err) {
                    print_error("Could not set minimum for header ");
                    exit(-1);
                }

            }

            for (i = 0; i < 5; i++) {
                if (LASHeader_GetPointRecordsByReturnCount(header, i) != summary->number_of_points_by_return[i]) 
                {
                    update_return_counts = TRUE;
                }
            }
            
            if (update_return_counts) {
                printf("supposed to update return counts\n");
                for (i = 0; i < 5; i++) {
                    LASHeader_SetPointRecordsByReturnCount(header, i, summary->number_of_points_by_return[i]);
                    printf("update_return_counts...\n");
                }                
            }
                
            writer = LASWriter_Create(file_name, header);
            if (!writer) {
                print_error("Problem creating LASWriterH object");
    	        LASHeader_Destroy(header);
                header = NULL;
                exit(-1);
            }
            LASWriter_Destroy(writer);
            writer = NULL;
            LASHeader_Destroy(header);
            header = NULL;            
        }             
            
        }
        
        LASPoint_Destroy(summary->pmin);
        LASPoint_Destroy(summary->pmax);
        free(summary);
        
    }   
    
     if (reader) LASReader_Destroy(reader);
     if (header) LASHeader_Destroy(header);
     

/*
    //     if (repair_header || repair_bounding_box || change_header)
    //     {
    //       if (file_name)
    //       {
    //         if (strstr(file_name, ".gz"))
    //         {
    //           fprintf(stderr, "ERROR: cannot change header of gzipped input files\n");
    //           repair_header = repair_bounding_box = change_header = false;
    //         }
    //         else
    //         {
    //           file = fopen(file_name, "rb+");
    //           if (file == 0)
    //           {
    //             fprintf (stderr, "ERROR: could reopen file '%s' for changing header\n", file_name);
    //             repair_header = repair_bounding_box = change_header = false;
    //           }
    //         }
    //       }
    //       else if (ilas)
    //       {
    //         fprintf(stderr, "ERROR: cannot change header of piped input\n");
    //         repair_header = repair_bounding_box = change_header = false;
    //       }
    //       else
    //       {
    //         fprintf (stderr, "ERROR: no input specified\n");
    //         usage();
    //       }
    //     }
    // 
    //     fprintf(stderr, "  x %d %d\n",point_min.x, point_max.x);
    //     fprintf(stderr, "  y %d %d\n",point_min.y, point_max.y);
    //     fprintf(stderr, "  z %d %d\n",point_min.z, point_max.z);
    //     fprintf(stderr, "  intensity %d %d\n",point_min.intensity, point_max.intensity);
    //     fprintf(stderr, "  edge_of_flight_line %d %d\n",point_min.edge_of_flight_line, point_max.edge_of_flight_line);
    //     fprintf(stderr, "  scan_direction_flag %d %d\n",point_min.scan_direction_flag, point_max.scan_direction_flag);
    //     fprintf(stderr, "  number_of_returns_of_given_pulse %d %d\n",point_min.number_of_returns_of_given_pulse, point_max.number_of_returns_of_given_pulse);
    //     fprintf(stderr, "  return_number %d %d\n",point_min.return_number, point_max.return_number);
    //     fprintf(stderr, "  classification %d %d\n",point_min.classification, point_max.classification);
    //     fprintf(stderr, "  scan_angle_rank %d %d\n",point_min.scan_angle_rank, point_max.scan_angle_rank);
    //     fprintf(stderr, "  user_data %d %d\n",point_min.user_data, point_max.user_data);
    //     fprintf(stderr, "  point_source_ID %d %d\n",point_min.point_source_ID, point_max.point_source_ID);
    //     fprintf(stderr, "  gps_time %f %f\n",gps_time_min, gps_time_max);
    //     
    //     if (change_header)
    //     {
    //       if (system_identifier)
    //       {
    //         fseek(file, 26, SEEK_SET);
    //         fwrite(system_identifier, sizeof(char), 32, file);
    //       }
    //       if (generating_software)
    //       {
    //         fseek(file, 58, SEEK_SET);
    //         fwrite(generating_software, sizeof(char), 32, file);
    //       }
    //       if (file_creation_day || file_creation_year)
    //       {
    //         fseek(file, 90, SEEK_SET);
    //         fwrite(&file_creation_day, sizeof(unsigned short), 1, file);
    //         fwrite(&file_creation_year, sizeof(unsigned short), 1, file);
    //       }
    //     }
    // 
    //     if (number_of_point_records != header->number_of_point_records)
    //     {
    //       fprintf(stderr, "real number of points (%d) is different from header number of points (%d) %s\n", number_of_point_records, header->number_of_point_records, repair_header ? "(repaired)" : "");
    //       if (repair_header)
    //       {
    //         fseek(file, 107, SEEK_SET);
    //         fwrite(&number_of_point_records, sizeof(unsigned int), 5, file);
    //       }
    //     }
    // 
    //     bool report = false;
    //     for (i = 1; i < 6; i++) if (header->number_of_points_by_return[i-1] != number_of_points_by_return[i]) report = true;
    //     if (report)
    //     {
    //       fprintf(stderr, "actual number of points by return is different:"); 
    //       for (i = 1; i < 6; i++) fprintf(stderr, " %d", number_of_points_by_return[i]); 
    //       fprintf(stderr, " %s\n", repair_header ? "(repaired)" : "");
    //       if (repair_header)
    //       {
    //         fseek(file, 111, SEEK_SET);
    //         fwrite(&(number_of_points_by_return[1]), sizeof(unsigned int), 5, file);
    //       }
    //     }
    // 
    //     if (number_of_points_by_return[0]) fprintf(stderr, "WARNING: there are %d points with return number 0\n", number_of_points_by_return[0]); 
    //     if (number_of_points_by_return[6]) fprintf(stderr, "WARNING: there are %d points with return number 6\n", number_of_points_by_return[6]); 
    //     if (number_of_points_by_return[7]) fprintf(stderr, "WARNING: there are %d points with return number 7\n", number_of_points_by_return[7]); 
    // 
    //     report = false;
    //     for (i = 1; i < 8; i++) if (number_of_returns_of_given_pulse[i]) report = true;
    //     if (report)
    //     {
    //       fprintf(stderr, "overview over number of returns of given pulse:"); 
    //       for (i = 1; i < 8; i++) fprintf(stderr, " %d", number_of_returns_of_given_pulse[i]);
    //       fprintf(stderr, "\n"); 
    //     }
    // 
    //     if (number_of_returns_of_given_pulse[0]) fprintf(stderr, "WARNING: there are %d points with a number of returns of given pulse of 0\n", number_of_returns_of_given_pulse[0]); 
    // 
    //     report = false;
    //     for (i = 0; i < 32; i++) if (classification[i]) report = true;
    //     if (classification_synthetic || classification_keypoint ||  classification_withheld) report = true;
    // 
    //     if (report)
    //     {
    //       fprintf(stderr, "histogram of classification of points:\n"); 
    //       for (i = 0; i < 32; i++) if (classification[i]) fprintf(stderr, " %8d %s (%d)\n", classification[i], LASpointClassification[i], i);
    //       if (classification_synthetic) fprintf(stderr, " +-> flagged as synthetic: %d\n", classification_synthetic);
    //       if (classification_keypoint) fprintf(stderr,  " +-> flagged as keypoints: %d\n", classification_keypoint);
    //       if (classification_withheld) fprintf(stderr,  " +-> flagged as withheld:  %d\n", classification_withheld);
    //     }
    // 
    //     if (repair_bounding_box)
    //     {
    //       fprintf(stderr, "repairing bounding box\n");
    //       fseek(file, 179, SEEK_SET);
    //       fwrite(&(max[0]), sizeof(double), 1, file);
    //       fwrite(&(min[0]), sizeof(double), 1, file);
    //       fwrite(&(max[1]), sizeof(double), 1, file);
    //       fwrite(&(min[1]), sizeof(double), 1, file);
    //       fwrite(&(max[2]), sizeof(double), 1, file);
    //       fwrite(&(min[2]), sizeof(double), 1, file);
    //     }
    //     else
    //     {
    //       if (max[0] > header->max_x)
    //       {
    //         fprintf(stderr, "real max x larger than header max x by %lf %s\n", max[0] - header->max_x);
    //         if (repair_header)
    //         {
    //           fseek(file, 179, SEEK_SET);
    //           fwrite(&(max[0]), sizeof(double), 1, file);
    //         }
    //       }
    //       if (min[0] < header->min_x)
    //       {
    //         fprintf(stderr, "real min x smaller than header min x by %lf %s\n", header->min_x - min[0], repair_header ? "(repaired)" : "");
    //         if (repair_header)
    //         {
    //           fseek(file, 187, SEEK_SET);
    //           fwrite(&(min[0]), sizeof(double), 1, file);
    //         }
    //       }
    //       if (max[1] > header->max_y)
    //       {
    //         fprintf(stderr, "real max y larger than header max y by %lf %s\n", max[1] - header->max_y, repair_header ? "(repaired)" : "");
    //         if (repair_header)
    //         {
    //           fseek(file, 195, SEEK_SET);
    //           fwrite(&(max[1]), sizeof(double), 1, file);
    //         }
    //       }
    //       if (min[1] < header->min_y)
    //       {
    //         fprintf(stderr, "real min y smaller than header min y by %lf %s\n", header->min_y - min[1], repair_header ? "(repaired)" : "");
    //         if (repair_header)
    //         {
    //           fseek(file, 203, SEEK_SET);
    //           fwrite(&(min[1]), sizeof(double), 1, file);
    //         }
    //       }
    //       if (max[2] > header->max_z)
    //       {
    //         fprintf(stderr, "real max z larger than header max z by %lf %s\n", max[2] - header->max_z, repair_header ? "(repaired)" : "");
    //         if (repair_header)
    //         {
    //           fseek(file, 211, SEEK_SET);
    //           fwrite(&(max[2]), sizeof(double), 1, file);
    //         }
    //       }
    //       if (min[2] < header->min_z)
    //       {
    //         fprintf(stderr, "real min z smaller than header min z by %lf %s\n", header->min_z - min[2], repair_header ? "(repaired)" : "");
    //         if (repair_header)
    //         {
    //           fseek(file, 219, SEEK_SET);
    //           fwrite(&(min[2]), sizeof(double), 1, file);
    //         }
    //       }
    //     }
    //   }
    // 
    //   if (repair_header || change_header)
    //   {
    //     fclose(file);
    //   }
*/
  return 0;
}
