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
        if (LASError_GetLastErrorNum()) 
            print_error("Not able to fetch a point.  LASReaderH is invalid");
        else
            print_error("File does not contain any points to read.");
        exit(1);
    }

    summary->pmin = LASPoint_Copy(p);
    summary->pmax = LASPoint_Copy(p);

    /* If we got this far we have one point */
    i = 1;
    while (p)
    {
    
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

        if (LASPoint_GetReturnNumber(p))
            summary->number_of_points_by_return[LASPoint_GetReturnNumber(p)-1]++;
        else
            summary->number_of_points_by_return[LASPoint_GetReturnNumber(p)]++;        

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

        if (LASError_GetLastErrorNum()) {
            print_error("Not able to fetch point");
            exit(1);
        }
        
        i++;
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

    if (!summary) {print_error("Point Summary does not exist!"); exit(1);}

    fprintf(stderr, "\n---------------------------------------------------------\n");
    fprintf(stderr, "  Point Inspection Summary\n");
    fprintf(stderr, "---------------------------------------------------------\n");
    
    fprintf(stderr, "  Header Point Count: %d\n",
                    LASHeader_GetPointRecordsCount(header));
                    
    fprintf(stderr, "  Actual Point Count: %d\n", 
                    summary->number_of_point_records);

    fprintf(stderr, "\n  Minimum and Maximum Attributes (min,max)\n");
    fprintf(stderr, "---------------------------------------------------------\n");
    
    fprintf(stderr, "  Min X,Y,Z: \t\t%.6f,%.6f,%.6f\n", 
                    LASPoint_GetX(summary->pmin),
                    LASPoint_GetY(summary->pmin),
                    LASPoint_GetZ(summary->pmin)
                    );
    fprintf(stderr, "  Max X,Y,Z: \t\t%.6f,%.6f,%.6f\n", 
                    LASPoint_GetX(summary->pmax),
                    LASPoint_GetY(summary->pmax),
                    LASPoint_GetZ(summary->pmax)
                    );
    fprintf(stderr, "  Bounding Box:\t\t%.2f,%.2f,%.2f,%.2f\n",
                    LASPoint_GetX(summary->pmin),
                    LASPoint_GetY(summary->pmin),
                    LASPoint_GetX(summary->pmax),
                    LASPoint_GetY(summary->pmax)
                    );
    fprintf(stderr, "  Time:\t\t\t%.6f,%.6f\n", 
                    LASPoint_GetTime(summary->pmin),
                    LASPoint_GetTime(summary->pmax)
                    );
    fprintf(stderr, "  Return Number:\t%d,%d\n", 
                    LASPoint_GetReturnNumber(summary->pmin),
                    LASPoint_GetReturnNumber(summary->pmax)
                    );
    fprintf(stderr, "  Return Count:\t\t%d,%d\n", 
                    LASPoint_GetNumberOfReturns(summary->pmin),
                    LASPoint_GetNumberOfReturns(summary->pmax)
                    );
    fprintf(stderr, "  Flightline Edge:\t%d,%d\n", 
                    LASPoint_GetFlightLineEdge(summary->pmin),
                    LASPoint_GetFlightLineEdge(summary->pmax)
                    );
    fprintf(stderr, "  Intensity:\t\t%d,%d\n",
                    LASPoint_GetIntensity(summary->pmin),
                    LASPoint_GetIntensity(summary->pmax)
                    );
    fprintf(stderr, "  Scan Direction Flag:\t%d,%d\n",
                    LASPoint_GetScanDirection(summary->pmin),
                    LASPoint_GetScanDirection(summary->pmax)
                    );
    fprintf(stderr, "  Classification:\t%d,%d\n",
                    LASPoint_GetClassification(summary->pmin),
                    LASPoint_GetClassification(summary->pmax)
                    );
                    
    fprintf(stderr, "\n  Number of Returns by Pulse\n");
    fprintf(stderr, "---------------------------------------------------------\n");
    
    for (i = 1; i < 8; i++) {
        rgpsum = rgpsum + summary->number_of_returns_of_given_pulse[i];
        fprintf(stderr, "\t(%d) %d", i,summary->number_of_returns_of_given_pulse[i]);
    }
    fprintf(stderr, "\n Total Pulses: %ld\n", rgpsum); 


    for (i = 0; i < 5; i++) {
        if (LASHeader_GetPointRecordsByReturnCount(header, i) != summary->number_of_points_by_return[i]) 
        {
            fprintf(stderr, " \n Actual number of points by return \n is different from header (actual, header):\n"); 
            for (i = 0; i < 5; i++) {
                fprintf(stderr, "\t(%d,%d)", 
                        summary->number_of_points_by_return[i],
                        LASHeader_GetPointRecordsByReturnCount(header, i)
                        );
            } 
            fprintf(stderr, "\n");
        }
    }

    fprintf(stderr, "\n  Point Classifications\n");
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
        fprintf(stderr, "\n  Point Classification Histogram\n");
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
        exit(1);
    }
    fprintf(stdout, "  Version:                    %d.%d\n", 
                    LASHeader_GetVersionMajor(header), 
                    LASHeader_GetVersionMinor(header));

    fprintf(stdout, "  Source ID:                  %d\n", 
                    LASHeader_GetFileSourceId(header) ) ;

    fprintf(stdout, "  Reserved:                   %d\n", 
                    LASHeader_GetReserved(header) );

    fprintf(stdout, "  Project ID/GUID:           '%s'\n", 
                    pszProjectId);

    fprintf(stdout, "  System Identifier:         '%s'\n", 
                    pszSystemId);

    fprintf(stdout, "  Generating Software:       '%s'\n", 
                    pszSoftwareId);

    fprintf(stdout, "  File Creation Day/Year:    %d/%d\n", 
                    LASHeader_GetCreationDOY(header), 
                    LASHeader_GetCreationYear(header));

    fprintf(stdout, "  Header Size                %d\n", 
                    LASHeader_GetHeaderSize(header));

    fprintf(stdout, "  Offset to Point Data       %d\n", 
                    LASHeader_GetDataOffset(header));

    fprintf(stdout, "  Number Var. Length Records %d\n", 
                    LASHeader_GetRecordsCount(header));

    fprintf(stdout, "  Point Data Format          %d\n", 
                    LASHeader_GetDataFormatId(header));

    fprintf(stdout, "  Point Data Record Length   %d\n", 
                    LASHeader_GetDataRecordLength(header));

    fprintf(stdout, "  Number of Point Records    %d\n", 
                    LASHeader_GetPointRecordsCount(header));

    fprintf(stdout, "  Number of Points by Return %d %d %d %d %d\n", 
                    LASHeader_GetPointRecordsByReturnCount(header, 0), 
                    LASHeader_GetPointRecordsByReturnCount(header, 1), 
                    LASHeader_GetPointRecordsByReturnCount(header, 2), 
                    LASHeader_GetPointRecordsByReturnCount(header, 3), 
                    LASHeader_GetPointRecordsByReturnCount(header, 4));

    fprintf(stdout, "  Scale Factor X Y Z         %.6f %.6f %.6f\n", 
                    LASHeader_GetScaleX(header), 
                    LASHeader_GetScaleY(header),
                    LASHeader_GetScaleZ(header));

    fprintf(stdout, "  Offset X Y Z               %.6f %.6f %.6f\n", 
                    LASHeader_GetOffsetX(header), 
                    LASHeader_GetOffsetY(header), 
                    LASHeader_GetOffsetZ(header));

    fprintf(stdout, "  Min X Y Z                  %.6f %.6f %.6f\n",
                    LASHeader_GetMinX(header), 
                    LASHeader_GetMinY(header), 
                    LASHeader_GetMinZ(header));

    fprintf(stdout, "  Max X Y Z                  %.6f %.6f %.6f\n", 
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
    
    int err = 0;

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
            exit(1);
        }
    }
      
    if (use_stdin) {
        file_name = "stdin";
    }
    
    if (!file_name) {
        print_error("No filename was provided to be opened");
        usage();
        exit(1);
    }
    
    reader = LASReader_Create(file_name);
    if (!reader) { 
        print_error("Could not open file ");
        exit(1);
    } 
      
    header = LASReader_GetHeader(reader);
    if (!header) { 
        print_error("Could not get LASHeader ");
        exit(1);
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
        
        writer = LASWriter_Create(file_name, header, LAS_MODE_APPEND);
        if (!writer) {
            print_error("Problem creating LASWriterH object");
	        LASHeader_Destroy(header);
            header = NULL;
            exit(1);
        }

        if (writer) LASWriter_Destroy(writer);
        writer = NULL;
        if (header) LASHeader_Destroy(header);
        header = NULL;
    }
    
    if (check_points)
    {
        if (!reader) {
            reader = LASReader_Create(file_name);
            if (!reader) { 
                print_error("Could not open file ");
                exit(1);
            } 
        }
  
        if (! header) {
            header = LASReader_GetHeader(reader);
            if (!header) { 
                print_error("Could not get LASHeader ");
                exit(1);
            } 
        } 
        
        if (!summary)
            summary = SummarizePoints(reader);
        print_point_summary(summary, header);
        
        if (repair_header) {
            fprintf(stderr, "\n---------------------------------------------------------\n");
            fprintf(stderr, "  Repair Summary\n");
            fprintf(stderr, "---------------------------------------------------------\n");

            
            if (use_stdin) {
                print_error("Cannot update header information on piped input!");
                exit(1);
            }

  
            if (! header) {
                header = LASReader_GetHeader(reader);
                if (!header) { 
                    print_error("Could not get LASHeader ");
                    exit(1);
                }
            } 
        
            if (! repair_bounding_box) {
                if ( LASHeader_GetMinX(header) != LASPoint_GetX(summary->pmin) )
                    repair_bounding_box = TRUE;
                if ( LASHeader_GetMinY(header) != LASPoint_GetY(summary->pmin) )
                    repair_bounding_box = TRUE;
                if ( LASHeader_GetMinZ(header) != LASPoint_GetZ(summary->pmin) )
                    repair_bounding_box = TRUE;

                if ( LASHeader_GetMaxX(header) != LASPoint_GetX(summary->pmax) )
                    repair_bounding_box = TRUE;
                if ( LASHeader_GetMaxY(header) != LASPoint_GetY(summary->pmax) )
                    repair_bounding_box = TRUE;
                if ( LASHeader_GetMaxZ(header) != LASPoint_GetZ(summary->pmax) )
                    repair_bounding_box = TRUE;
            }
            
            if (repair_bounding_box) {
                fprintf(stderr, "  Reparing Bounding Box...\n");
                err = LASHeader_SetMin( header, 
                                        LASPoint_GetX(summary->pmin), 
                                        LASPoint_GetY(summary->pmin), 
                                        LASPoint_GetZ(summary->pmin)
                                      );
                if (err) {
                    print_error("Could not set minimum for header ");
                    exit(1);
                }
                err = LASHeader_SetMax( header, 
                                        LASPoint_GetX(summary->pmax), 
                                        LASPoint_GetY(summary->pmax), 
                                        LASPoint_GetZ(summary->pmax)
                                      );
                if (err) {
                    print_error("Could not set minimum for header ");
                    exit(1);
                }

            }


            for (i = 0; i < 5; i++) {

                if (LASHeader_GetPointRecordsByReturnCount(header, i) != summary->number_of_points_by_return[i]) 
                {
                    update_return_counts = TRUE;
                    break;
                }
            }
            
            if (update_return_counts) {
                fprintf(stderr, "  Reparing Point Count by Return...\n");
                for (i = 0; i < 5; i++) {
                    LASHeader_SetPointRecordsByReturnCount(header, i, summary->number_of_points_by_return[i]);
                }                
            }
            



            if (reader) {
                LASReader_Destroy(reader);
                reader = NULL;
            }
                
            writer = LASWriter_Create(file_name, header, LAS_MODE_APPEND);
            if (!writer) {
                print_error("Problem creating LASWriterH object for append");
    	        LASHeader_Destroy(header);
                header = NULL;
                exit(1);
            }
            LASWriter_Destroy(writer);
            writer = NULL;
            LASHeader_Destroy(header);
            header = NULL;            
            
        
        LASPoint_Destroy(summary->pmin);
        LASPoint_Destroy(summary->pmax);
        free(summary);
    }
}   
    
     if (reader) LASReader_Destroy(reader);
     if (header) LASHeader_Destroy(header);
     

  return 0;
}
