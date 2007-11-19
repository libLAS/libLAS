/*
===============================================================================

  FILE:  laszip.cpp
  
  CONTENTS:
  
    This tool reads and writes LIDAR data in the LAS format. Because our reader
    and writer for the LAS format also support compression we can use this tool
    to compress or decompress LIDAR files from standard *.las to our *.las.lz 
    or *.laz format.

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    14 February 2007 -- created after picking flowers for the Valentine dinner
  
===============================================================================
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lasreader.h"
#include "laswriter.h"

#ifdef _WIN32
extern "C" FILE* fopenGzipped(const char* filename, const char* mode);
#endif

void usage()
{
  fprintf(stderr,"usage:\n");
  fprintf(stderr,"laszip lidar.las\n");
  fprintf(stderr,"laszip lidar.laz\n");
  fprintf(stderr,"laszip lidar.las lidar.las.lz\n");
  fprintf(stderr,"laszip lidar.las.lz\n");
  fprintf(stderr,"laszip -i lidar.las -o lidar_compressed.laz\n");
  fprintf(stderr,"laszip -i lidar.las -olaz > lidar_compressed.laz\n");
  fprintf(stderr,"laszip -h\n");
  exit(1);
}

void ptime(const char *const msg)
{
  float t= ((float)clock())/CLOCKS_PER_SEC;
  fprintf(stderr, "cumulative CPU time thru %s = %f\n", msg, t);
}

int main(int argc, char *argv[])
{
  int i;
  bool dry = false;
  bool verbose = false;
  char* file_name_in = 0;
  char* file_name_out = 0;
  bool ilas = false;
  bool ilaz = false;
  bool olas = false;
  bool olaz = false;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i],"-dry") == 0)
    {
      dry = true;
    }
    else if (strcmp(argv[i],"-verbose") == 0)
    {
      verbose = true;
    }
    else if (strcmp(argv[i],"-h") == 0)
    {
      usage();
    }
    else if (strcmp(argv[i],"-i") == 0)
    {
      i++;
      file_name_in = argv[i];
    }
    else if (strcmp(argv[i],"-o") == 0)
    {
      i++;
      file_name_out = argv[i];
    }
    else if (strcmp(argv[i],"-ilas") == 0)
    {
      ilas = true;
    }
    else if (strcmp(argv[i],"-ilaz") == 0)
    {
      ilaz = true;
    }
    else if (strcmp(argv[i],"-olas") == 0)
    {
      olas = true;
    }
    else if (strcmp(argv[i],"-olaz") == 0)
    {
      olaz = true;
    }
    else if (i == argc - 2 && file_name_in == 0 && file_name_out == 0)
    {
      file_name_in = argv[i];
    }
    else if (i == argc - 1 && file_name_in == 0 && file_name_out == 0)
    {
      file_name_in = argv[i];
    }
    else if (i == argc - 1 && file_name_in != 0 && file_name_out == 0)
    {
      file_name_out = argv[i];
    }
  }

  // open input file

  FILE* file_in;
  if (file_name_in)
  {
    if (strstr(file_name_in, ".gz"))
    {
#ifdef _WIN32
      file_in = fopenGzipped(file_name_in, "rb");
#else
      fprintf(stderr, "ERROR: no support for gzipped input\n");
      exit(1);
#endif
    }
    else
    {
      file_in = fopen(file_name_in, "rb");
    }
    if (file_in == 0)
    {
      fprintf(stderr, "ERROR: could not open '%s'\n",file_name_in);
      usage();
    }
  }
  else if (ilas || ilaz)
  {
    file_in = stdin;
  }
  else
  {
    fprintf(stderr, "ERROR: no input specified\n");
    usage();
  }

  LASreader* lasreader = new LASreader();

  if (lasreader->open(file_in,false) == false)
  {
    fprintf(stderr, "ERROR: could not open lasreader\n");
    exit(1);
  }

  // maybe only a read pass

  if (dry)
  {
    LASpoint point_min;
    LASpoint point_max;
    double gps_time_min;
    double gps_time_max;

    if (verbose)
    {
      lasreader->read_point();
      point_min = lasreader->point;
      point_max = lasreader->point;
      gps_time_min = lasreader->gps_time;
      gps_time_max = lasreader->gps_time;
      while (lasreader->read_point())
      {
        if (lasreader->point.x < point_min.x) point_min.x = lasreader->point.x;
        else if (lasreader->point.x > point_max.x) point_max.x = lasreader->point.x;
        if (lasreader->point.y < point_min.y) point_min.y = lasreader->point.y;
        else if (lasreader->point.y > point_max.y) point_max.y = lasreader->point.y;
        if (lasreader->point.z < point_min.z) point_min.z = lasreader->point.z;
        else if (lasreader->point.z > point_max.z) point_max.z = lasreader->point.z;
        if (lasreader->point.intensity < point_min.intensity) point_min.intensity = lasreader->point.intensity;
        else if (lasreader->point.intensity > point_max.intensity) point_max.intensity = lasreader->point.intensity;
        if (lasreader->point.edge_of_flight_line < point_min.edge_of_flight_line) point_min.edge_of_flight_line = lasreader->point.edge_of_flight_line;
        else if (lasreader->point.edge_of_flight_line > point_max.edge_of_flight_line) point_max.edge_of_flight_line = lasreader->point.edge_of_flight_line;
        if (lasreader->point.scan_direction_flag < point_min.scan_direction_flag) point_min.scan_direction_flag = lasreader->point.scan_direction_flag;
        else if (lasreader->point.scan_direction_flag > point_max.scan_direction_flag) point_max.scan_direction_flag = lasreader->point.scan_direction_flag;
        if (lasreader->point.number_of_returns_of_given_pulse < point_min.number_of_returns_of_given_pulse) point_min.number_of_returns_of_given_pulse = lasreader->point.number_of_returns_of_given_pulse;
        else if (lasreader->point.number_of_returns_of_given_pulse > point_max.number_of_returns_of_given_pulse) point_max.number_of_returns_of_given_pulse = lasreader->point.number_of_returns_of_given_pulse;
        if (lasreader->point.return_number < point_min.return_number) point_min.return_number = lasreader->point.return_number;
        else if (lasreader->point.return_number > point_max.return_number) point_max.return_number = lasreader->point.return_number;
        if (lasreader->point.classification < point_min.classification) point_min.classification = lasreader->point.classification;
        else if (lasreader->point.classification > point_max.classification) point_max.classification = lasreader->point.classification;
        if (lasreader->point.scan_angle_rank < point_min.scan_angle_rank) point_min.scan_angle_rank = lasreader->point.scan_angle_rank;
        else if (lasreader->point.scan_angle_rank > point_max.scan_angle_rank) point_max.scan_angle_rank = lasreader->point.scan_angle_rank;
        if (lasreader->point.user_data < point_min.user_data) point_min.user_data = lasreader->point.user_data;
        else if (lasreader->point.user_data > point_max.user_data) point_max.user_data = lasreader->point.user_data;
        if (lasreader->point.point_source_ID < point_min.point_source_ID) point_min.point_source_ID = lasreader->point.point_source_ID;
        else if (lasreader->point.point_source_ID > point_max.point_source_ID) point_max.point_source_ID = lasreader->point.point_source_ID;
        if (lasreader->point.point_source_ID < point_min.point_source_ID) point_min.point_source_ID = lasreader->point.point_source_ID;
        else if (lasreader->point.point_source_ID > point_max.point_source_ID) point_max.point_source_ID = lasreader->point.point_source_ID;
        if (lasreader->gps_time < gps_time_min) gps_time_min = lasreader->gps_time;
        else if (lasreader->gps_time > gps_time_max) gps_time_max = lasreader->gps_time;
      }
      fprintf(stderr, "x %d %d %d\n",point_min.x, point_max.x, point_max.x - point_min.x);
      fprintf(stderr, "y %d %d %d\n",point_min.y, point_max.y, point_max.y - point_min.y);
      fprintf(stderr, "z %d %d %d\n",point_min.z, point_max.z, point_max.z - point_min.z);
      fprintf(stderr, "intensity %d %d %d\n",point_min.intensity, point_max.intensity, point_max.intensity - point_min.intensity);
      fprintf(stderr, "edge_of_flight_line %d %d %d\n",point_min.edge_of_flight_line, point_max.edge_of_flight_line, point_max.edge_of_flight_line - point_min.edge_of_flight_line);
      fprintf(stderr, "scan_direction_flag %d %d %d\n",point_min.scan_direction_flag, point_max.scan_direction_flag, point_max.scan_direction_flag - point_min.scan_direction_flag);
      fprintf(stderr, "number_of_returns_of_given_pulse %d %d %d\n",point_min.number_of_returns_of_given_pulse, point_max.number_of_returns_of_given_pulse, point_max.number_of_returns_of_given_pulse - point_min.number_of_returns_of_given_pulse);
      fprintf(stderr, "return_number %d %d %d\n",point_min.return_number, point_max.return_number, point_max.return_number - point_min.return_number);
      fprintf(stderr, "classification %d %d %d\n",point_min.classification, point_max.classification, point_max.classification - point_min.classification);
      fprintf(stderr, "scan_angle_rank %d %d %d\n",point_min.scan_angle_rank, point_max.scan_angle_rank, point_max.scan_angle_rank - point_min.scan_angle_rank);
      fprintf(stderr, "user_data %d %d %d\n",point_min.user_data, point_max.user_data, point_max.user_data - point_min.user_data);
      fprintf(stderr, "point_source_ID %d %d %d\n",point_min.point_source_ID, point_max.point_source_ID, point_max.point_source_ID - point_min.point_source_ID);
      fprintf(stderr, "gps_time %.8f %.8f %.8f\n",gps_time_min, gps_time_max, gps_time_max - gps_time_min);
    }
    else
    {
      ptime("starting ...");
      while (lasreader->read_point());
      ptime("done.");
    }
  }
  else
  {
    // create output file name if needed 

    if (file_name_out == 0 && !olas && !olaz)
    {
      int len = strlen(file_name_in);
      file_name_out = strdup(file_name_in);
      if (strstr(file_name_in, ".las"))
      {
        file_name_out[len-4] = '.';
        file_name_out[len-3] = 'l';
        file_name_out[len-2] = 'a';
        file_name_out[len-1] = 'z';
      }
      else if (strstr(file_name_in, ".laz"))
      {
        file_name_out[len-4] = '.';
        file_name_out[len-3] = 'l';
        file_name_out[len-2] = 'a';
        file_name_out[len-1] = 's';
      }
      else if (strstr(file_name_in, ".las.lz"))
      {
        file_name_out[len-7] = '.';
        file_name_out[len-6] = 'l';
        file_name_out[len-5] = 'a';
        file_name_out[len-4] = 's';
        file_name_out[len-3] = '\0';
      }
    }

    // open output file

    int compression = 0;

    FILE* file_out;
    if (file_name_out)
    {
      file_out = fopen(file_name_out, "wb");
      if (strstr(file_name_out, ".laz") || strstr(file_name_out, ".las.lz"))
      {
        compression = 1;
      }
    }
    else if (olas || olaz)
    {
      file_out = stdout;
      if (olaz)
      {
        compression = 1;
      }
    }
    else
    {
      fprintf (stderr, "ERROR: no output specified\n");
      usage();
    }

    if (verbose) ptime("starting ...");

    LASwriter* laswriter = new LASwriter();

    if (laswriter->open(file_out, &(lasreader->header), compression) == false)
    {
      fprintf(stderr, "ERROR: could not open laswriter\n");
      exit(1);
    }

    // copy variable header (and user data)

    for (unsigned int u = lasreader->header.header_size; u < lasreader->header.offset_to_point_data; u++)
    {
      fputc(fgetc(file_in),file_out);
    }

    // loop over points

    while (lasreader->read_point())
    {
      laswriter->write_point(&(lasreader->point), lasreader->gps_time);
    }

    laswriter->close();
    fclose(file_out);

    if (verbose) ptime("done.");
  }

  lasreader->close();
  fclose(file_in);

  return 0;
}
