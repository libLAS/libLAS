#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <liblas.h>




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


LASPointSummary* SummarizePoints(LASReaderH reader) {
    
    LASPointSummary* summary;
    LASPointH p;
    uint8_t cls = 0;
    int i = 0;

    summary = (LASPointSummary*) malloc(sizeof(LASPointSummary));

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
            LASError_Print("Not able to fetch a point.  LASReaderH is invalid");
        else
            LASError_Print("File does not contain any points to read.");
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
        LASPoint_SetClassification(summary->pmin, MIN(cls, LASPoint_GetClassification(summary->pmin)));
        LASPoint_SetClassification(summary->pmax, MAX(cls, LASPoint_GetClassification(summary->pmax)));  

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
            LASError_Print("Not able to fetch point");
            exit(1);
        }
        
        i++;
    }
    
    return summary; 
}

void print_point_summary(LASPointSummary* summary, LASHeaderH header) {

    long rgpsum = 0;
    long pbretsum = 0;
    
    int i = 0;

    if (!summary) {LASError_Print("Point Summary does not exist!"); exit(1);}

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

    fprintf(stderr, "\n  Number of Points by Return\n");
    fprintf(stderr, "---------------------------------------------------------\n");

    for (i = 0; i < 5; i++) {
        pbretsum = pbretsum + summary->number_of_points_by_return[i];
        fprintf(stderr, "\t(%d) %d", i,summary->number_of_points_by_return[i]);
    }
    fprintf(stderr, "\n Total Points: %ld\n", pbretsum); 
                    
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


    fprintf(stderr, "  File Name: %s\n", file_name);
    
    if (strcmp(pszSignature,"LASF") !=0) {
        LASError_Print("File signature is not 'LASF'... aborting");
        exit(1);
    }
    fprintf(stderr, "  Version:                    %d.%d\n", 
                    LASHeader_GetVersionMajor(header), 
                    LASHeader_GetVersionMinor(header));

    fprintf(stderr, "  Source ID:                  %d\n", 
                    LASHeader_GetFileSourceId(header) ) ;

    fprintf(stderr, "  Reserved:                   %d\n", 
                    LASHeader_GetReserved(header) );

    fprintf(stderr, "  Project ID/GUID:           '%s'\n", 
                    pszProjectId);

    fprintf(stderr, "  System Identifier:         '%s'\n", 
                    pszSystemId);

    fprintf(stderr, "  Generating Software:       '%s'\n", 
                    pszSoftwareId);

    fprintf(stderr, "  File Creation Day/Year:    %d/%d\n", 
                    LASHeader_GetCreationDOY(header), 
                    LASHeader_GetCreationYear(header));

    fprintf(stderr, "  Header Size                %d\n", 
                    LASHeader_GetHeaderSize(header));

    fprintf(stderr, "  Offset to Point Data       %d\n", 
                    LASHeader_GetDataOffset(header));

    fprintf(stderr, "  Number Var. Length Records %d\n", 
                    LASHeader_GetRecordsCount(header));

    fprintf(stderr, "  Point Data Format          %d\n", 
                    LASHeader_GetDataFormatId(header));

    fprintf(stderr, "  Point Data Record Length   %d\n", 
                    LASHeader_GetDataRecordLength(header));

    fprintf(stderr, "  Number of Point Records    %d\n", 
                    LASHeader_GetPointRecordsCount(header));

    fprintf(stderr, "  Number of Points by Return %d %d %d %d %d\n", 
                    LASHeader_GetPointRecordsByReturnCount(header, 0), 
                    LASHeader_GetPointRecordsByReturnCount(header, 1), 
                    LASHeader_GetPointRecordsByReturnCount(header, 2), 
                    LASHeader_GetPointRecordsByReturnCount(header, 3), 
                    LASHeader_GetPointRecordsByReturnCount(header, 4));

    fprintf(stderr, "  Scale Factor X Y Z         %.6f %.6f %.6f\n", 
                    LASHeader_GetScaleX(header), 
                    LASHeader_GetScaleY(header),
                    LASHeader_GetScaleZ(header));

    fprintf(stderr, "  Offset X Y Z               %.6f %.6f %.6f\n", 
                    LASHeader_GetOffsetX(header), 
                    LASHeader_GetOffsetY(header), 
                    LASHeader_GetOffsetZ(header));

    fprintf(stderr, "  Min X Y Z                  %.6f %.6f %.6f\n",
                    LASHeader_GetMinX(header), 
                    LASHeader_GetMinY(header), 
                    LASHeader_GetMinZ(header));

    fprintf(stderr, "  Max X Y Z                  %.6f %.6f %.6f\n", 
                    LASHeader_GetMaxX(header), 
                    LASHeader_GetMaxY(header), 
                    LASHeader_GetMaxZ(header));

    free(pszSignature);
    free(pszProjectId);
    free(pszSystemId);
    free(pszSoftwareId);
    
}

void RepairHeader(LASHeaderH header, LASPointSummary* summary) {

    int repair_bounding_box = FALSE;
    int update_return_counts = FALSE;    
    int err = 0;
    int i = 0;
 
    if (! header) {
        LASError_Print("Inputted header to RepairHeader was NULL ");
        exit(1);
    } 

    if (! summary) {
        LASError_Print("Inputted summary to RepairHeader was NULL ");
        exit(1);

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
            LASError_Print("Could not set minimum for header ");
            exit(1);
        }
        err = LASHeader_SetMax( header, 
                                LASPoint_GetX(summary->pmax), 
                                LASPoint_GetY(summary->pmax), 
                                LASPoint_GetZ(summary->pmax)
                              );
        if (err) {
            LASError_Print("Could not set minimum for header ");
            exit(1);
        }

    }


    for (i = 0; i < 5; i++) {

        if (LASHeader_GetPointRecordsByReturnCount(header, i) != 
            summary->number_of_points_by_return[i]) 
        {
            update_return_counts = TRUE;
            break;
        }
    }
    
    if (update_return_counts) {
        fprintf(stderr, "  Reparing Point Count by Return...\n");
        for (i = 0; i < 5; i++) {
            LASHeader_SetPointRecordsByReturnCount( header,  
                                                    i, 
                                                    summary->number_of_points_by_return[i]);
        }                
    }   

}