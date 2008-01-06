/***************************************************************************
 * $Id: lasinfo.c 87 2007-12-16 02:40:46Z hobu $
 * $Date: 2007-12-15 20:40:46 -0600 (Sat, 15 Dec 2007) $
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS information reporting and setting
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/


#include <liblas.h>
#define NULL 0

void usage()
{
    // fprintf(stderr,"usage:\n");
    // fprintf(stderr,"lasinfo lidar.las\n");
    // fprintf(stderr,"lasinfo -variable lidar.las\n");
    // fprintf(stderr,"lasinfo -variable -nocheck lidar.las\n");
    // fprintf(stderr,"lasinfo -i lidar.las -nocheck\n");
    // fprintf(stderr,"lasinfo -i lidar.las -repair\n");
    // fprintf(stderr,"lasinfo -i lidar.las -repair_bounding_box -file_creation 8 2007\n");
    // fprintf(stderr,"lasinfo -i lidar.las -system_identifier \"hello world!\" -generating_software \"this is a test (-:\"\n");
    exit(1);
}
 
int main(int argc, char *argv[])
{
       int i;
       char* file_name = NULL;
       LASReaderH reader = NULL;
    //   bool parse_variable_header = false;
    //   bool check_points = true;
    //   bool repair_header = false;
    //   bool repair_bounding_box = false;
    //   bool change_header = false;
    // char* system_identifier = 0;
    // char* generating_software = 0;
    // unsigned short file_creation_day = 0;
    // unsigned short file_creation_year = 0;
    
    //   char* file_name = 0;
    //   bool ilas = false;
    // 
    // 
    
      for (i = 1; i < argc; i++)
      {
        if (strcmp(argv[i],"-h") == 0)
        {
          usage();
        }
        else if (strcmp(argv[i],"-i") == 0)
        {
          i++;
          file_name = argv[i];
        }

        else if (i == argc - 1 && file_name == 0)
        {
          file_name = argv[i];
        }
        else
        {
          usage();
        }
      }
      reader = LASReader_Create(file_name);
    // 
    //   FILE* file;
    //   if (file_name)
    //   {
    //     if (strstr(file_name, ".gz"))
    //     {
    //       fprintf(stderr, "ERROR: no support for gzipped input\n");
    //       exit(1);
    //     }
    //     else
    //     {
    //       file = fopen(file_name, "rb");
    //     }
    //     if (file == 0)
    //     {
    //       fprintf (stderr, "ERROR: could not open file '%s'\n", file_name);
    //       usage();
    //     }
    //   }
    //   else if (ilas)
    //   {
    //     file = stdin;
    //   }
    //   else
    //   {
    //     fprintf (stderr, "ERROR: no input specified\n");
    //     usage();
    //   }
    // 
    //   LASreader* lasreader = new LASreader();
    //   if (lasreader->open(file,(parse_variable_header == false)) == false)
    //   {
    //     fprintf (stderr, "ERROR: lasreader open failed for '%s'\n", file_name);
    //   }
    // 
    //   LASheader* header = &(lasreader->header);
    // 
    //   // print header info
    //   char printstring[256];
    // 
    //   fprintf(stderr, "reporting all LAS header entries:\n");
    //   fprintf(stderr, "  file signature:            '%s'\n", header->file_signature);
    //   fprintf(stderr, "  file source ID:            %d\n", header->file_source_id);
    //   fprintf(stderr, "  reserved:                  %d\n", header->reserved);
    //   fprintf(stderr, "  project ID GUID data 1-4:  %d %d %d '%s'\n", header->project_ID_GUID_data_1, header->project_ID_GUID_data_2, header->project_ID_GUID_data_3, header->project_ID_GUID_data_4);
    //   fprintf(stderr, "  version major.minor:       %d.%d\n", header->version_major, header->version_minor);
    //   fprintf(stderr, "  system_identifier:         '%s'\n", header->system_identifier);
    //   fprintf(stderr, "  generating_software:       '%s'\n", header->generating_software);
    //   fprintf(stderr, "  file creation day/year:    %d/%d\n", header->file_creation_day, header->file_creation_year);
    //   fprintf(stderr, "  header size                %d\n", header->header_size);
    //   fprintf(stderr, "  offset to point data       %d\n", header->offset_to_point_data);
    //   fprintf(stderr, "  number var. length records %d\n", header->number_of_variable_length_records);
    //   fprintf(stderr, "  point data format          %d\n", header->point_data_format);
    //   fprintf(stderr, "  point data record length   %d\n", header->point_data_record_length);
    //   fprintf(stderr, "  number of point records    %d\n", header->number_of_point_records);
    //   fprintf(stderr, "  number of points by return %d %d %d %d %d\n", header->number_of_points_by_return[0], header->number_of_points_by_return[1], header->number_of_points_by_return[2], header->number_of_points_by_return[3], header->number_of_points_by_return[4]);
    //   fprintf(stderr, "  scale factor x y z         "); lidardouble2string(printstring, header->x_scale_factor, header->y_scale_factor, header->z_scale_factor, true); fprintf(stderr, printstring);
    //   fprintf(stderr, "  offset x y z               "); lidardouble2string(printstring, header->x_offset, header->y_offset, header->z_offset, true); fprintf(stderr, printstring);
    //   fprintf(stderr, "  min x y z                  "); lidardouble2string(printstring, header->min_x, header->min_y, header->min_z, true); fprintf(stderr, printstring);
    //   fprintf(stderr, "  max x y z                  "); lidardouble2string(printstring, header->max_x, header->max_y, header->max_z, true); fprintf(stderr, printstring);
    // 
    //   // maybe print variable header
    // 
    //   if (parse_variable_header)
    //   {
    //     LASvariable_header variable_header;
    //     unsigned int offset_to_point_data_counter = header->header_size;
    // 
    //     for (int i = 0; i < (int)header->number_of_variable_length_records; i++)
    //     {
    //       if (fread(&variable_header, sizeof(LASvariable_header), 1, file) != 1)
    //       {
    //         fprintf(stderr,"ERROR: reading variable_header %d of %d\n",i,header->number_of_variable_length_records);
    //         return 1;
    //       }
    //       fprintf(stderr, "variable length header record %d of %d:\n", i, (int)header->number_of_variable_length_records);
    //       fprintf(stderr, "  reserved             %d\n", variable_header.reserved);
    //       fprintf(stderr, "  user ID              '%s'\n", variable_header.user_id);
    //       fprintf(stderr, "  record ID            %d\n", variable_header.record_id);
    //       fprintf(stderr, "  length after header  %d\n", variable_header.record_length_after_header);
    //       fprintf(stderr, "  description          '%s'\n", variable_header.description);
    // 
    //       for (int j = 0; j < variable_header.record_length_after_header; j++)
    //       {
    //         fgetc(file);
    //       }
    //       offset_to_point_data_counter += (sizeof(LASvariable_header)+variable_header.record_length_after_header);
    //     }
    //     // move file pointer to where the points starts
    //     for (; offset_to_point_data_counter < header->offset_to_point_data; offset_to_point_data_counter++)
    //     {
    //       fgetc(file);
    //     }
    //   }
    // 
    //   // some additional histograms
    // 
    //   unsigned int number_of_point_records = 0;
    //   unsigned int number_of_points_by_return[8] = {0,0,0,0,0,0,0,0};
    //   int number_of_returns_of_given_pulse[8] = {0,0,0,0,0,0,0,0};
    //   int classification[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //   int classification_synthetic = 0;
    //   int classification_keypoint = 0;
    //   int classification_withheld = 0;
    // 
    //   // loop over the points
    // 
    //   if (check_points)
    //   {
    //     fprintf(stderr, "reporting minimum and maximum for all %d LAS point record entries ...\n",lasreader->npoints);
    //     LASpoint point_min;
    //     LASpoint point_max;
    //     double gps_time_min;
    //     double gps_time_max;
    //     double coordinates[3];
    //     double min[3];
    //     double max[3];
    //     lasreader->read_point(coordinates);
    //     point_min = lasreader->point;
    //     point_max = lasreader->point;
    //     gps_time_min = lasreader->gps_time;
    //     gps_time_max = lasreader->gps_time;
    //     VecCopy3dv(min, coordinates);
    //     VecCopy3dv(max, coordinates);
    //     number_of_point_records = 1;
    //     number_of_points_by_return[lasreader->point.return_number]++;
    //     number_of_returns_of_given_pulse[lasreader->point.number_of_returns_of_given_pulse]++;
    //     classification[(lasreader->point.classification & 31)]++;
    //     if (lasreader->point.classification & 32) classification_synthetic++;
    //     if (lasreader->point.classification & 64) classification_keypoint++;
    //     if (lasreader->point.classification & 128) classification_withheld++;
    //     while (lasreader->read_point(coordinates))
    //     {
    //       if (lasreader->point.x < point_min.x) point_min.x = lasreader->point.x;
    //       else if (lasreader->point.x > point_max.x) point_max.x = lasreader->point.x;
    //       if (lasreader->point.y < point_min.y) point_min.y = lasreader->point.y;
    //       else if (lasreader->point.y > point_max.y) point_max.y = lasreader->point.y;
    //       if (lasreader->point.z < point_min.z) point_min.z = lasreader->point.z;
    //       else if (lasreader->point.z > point_max.z) point_max.z = lasreader->point.z;
    //       if (lasreader->point.intensity < point_min.intensity) point_min.intensity = lasreader->point.intensity;
    //       else if (lasreader->point.intensity > point_max.intensity) point_max.intensity = lasreader->point.intensity;
    //       if (lasreader->point.edge_of_flight_line < point_min.edge_of_flight_line) point_min.edge_of_flight_line = lasreader->point.edge_of_flight_line;
    //       else if (lasreader->point.edge_of_flight_line > point_max.edge_of_flight_line) point_max.edge_of_flight_line = lasreader->point.edge_of_flight_line;
    //       if (lasreader->point.scan_direction_flag < point_min.scan_direction_flag) point_min.scan_direction_flag = lasreader->point.scan_direction_flag;
    //       else if (lasreader->point.scan_direction_flag > point_max.scan_direction_flag) point_max.scan_direction_flag = lasreader->point.scan_direction_flag;
    //       if (lasreader->point.number_of_returns_of_given_pulse < point_min.number_of_returns_of_given_pulse) point_min.number_of_returns_of_given_pulse = lasreader->point.number_of_returns_of_given_pulse;
    //       else if (lasreader->point.number_of_returns_of_given_pulse > point_max.number_of_returns_of_given_pulse) point_max.number_of_returns_of_given_pulse = lasreader->point.number_of_returns_of_given_pulse;
    //       if (lasreader->point.return_number < point_min.return_number) point_min.return_number = lasreader->point.return_number;
    //       else if (lasreader->point.return_number > point_max.return_number) point_max.return_number = lasreader->point.return_number;
    //       if (lasreader->point.classification < point_min.classification) point_min.classification = lasreader->point.classification;
    //       else if (lasreader->point.classification > point_max.classification) point_max.classification = lasreader->point.classification;
    //       if (lasreader->point.scan_angle_rank < point_min.scan_angle_rank) point_min.scan_angle_rank = lasreader->point.scan_angle_rank;
    //       else if (lasreader->point.scan_angle_rank > point_max.scan_angle_rank) point_max.scan_angle_rank = lasreader->point.scan_angle_rank;
    //       if (lasreader->point.user_data < point_min.user_data) point_min.user_data = lasreader->point.user_data;
    //       else if (lasreader->point.user_data > point_max.user_data) point_max.user_data = lasreader->point.user_data;
    //       if (lasreader->point.point_source_ID < point_min.point_source_ID) point_min.point_source_ID = lasreader->point.point_source_ID;
    //       else if (lasreader->point.point_source_ID > point_max.point_source_ID) point_max.point_source_ID = lasreader->point.point_source_ID;
    //       if (lasreader->point.point_source_ID < point_min.point_source_ID) point_min.point_source_ID = lasreader->point.point_source_ID;
    //       else if (lasreader->point.point_source_ID > point_max.point_source_ID) point_max.point_source_ID = lasreader->point.point_source_ID;
    //       if (lasreader->gps_time < gps_time_min) gps_time_min = lasreader->gps_time;
    //       else if (lasreader->gps_time > gps_time_max) gps_time_max = lasreader->gps_time;
    //       VecUpdateMinMax3dv(min, max, coordinates);
    //       number_of_point_records++;
    //       number_of_points_by_return[lasreader->point.return_number]++;
    //       number_of_returns_of_given_pulse[lasreader->point.number_of_returns_of_given_pulse]++;
    //       classification[(lasreader->point.classification & 31)]++;
    //       if (lasreader->point.classification & 32) classification_synthetic++;
    //       if (lasreader->point.classification & 64) classification_keypoint++;
    //       if (lasreader->point.classification & 128) classification_withheld++;
    //     }
    // 
    //     lasreader->close();
    //     fclose(file);
    // 
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

  return 0;
}
