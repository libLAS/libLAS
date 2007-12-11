/*
===============================================================================

  FILE:  lasmerge.cpp
  
  CONTENTS:
  
    This tool merges multiple LAS file into a single LAS file and outputs it in
    the LAS format. As input the user either provides multiple LAS file names or
    a text file containing a list of LAS file names.

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    07 November 2007 -- created after email from luis.viveros@digimapas.cl
  
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
  fprintf(stderr,"lasmerge -i lasfiles.txt -o out.las\n");
  fprintf(stderr,"lasmerge -i file1.las -i file2.las -i file3.las -o out.las\n");
  fprintf(stderr,"lasmerge -i file1.las -i file2.las -olas > out.las\n");
  fprintf(stderr,"lasmerge -i lasfiles.txt -scale 0.01 -verbose -o out.las\n");
  fprintf(stderr,"lasmerge -h\n");
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
  bool verbose = false;
  int num_file_name_in = 0;
  int alloced_file_name_in = 32;
  char** file_names_in = (char**)malloc(sizeof(char*)*alloced_file_name_in);
  char* file_name_out = 0;
  bool olas = false;
  bool olaz = false;
  double* xyz_scale = 0;
  double* xyz_offset = 0;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i],"-verbose") == 0)
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
      if (num_file_name_in == alloced_file_name_in)
      {
        alloced_file_name_in *= 2;
        file_names_in = (char**)realloc(file_names_in,sizeof(char*)*alloced_file_name_in);
      }
      file_names_in[num_file_name_in] = argv[i];
      num_file_name_in++;
    }
    else if (strcmp(argv[i],"-o") == 0)
    {
      i++;
      file_name_out = argv[i];
    }
    else if (strcmp(argv[i],"-scale") == 0)
    {
      xyz_scale = new double[3];
			i++;
			sscanf(argv[i], "%lf", &(xyz_scale[2]));
 			xyz_scale[0] = xyz_scale[1] = xyz_scale[2];
    }
    else if (strcmp(argv[i],"-xyz_scale") == 0)
    {
      xyz_scale = new double[3];
			i++;
			sscanf(argv[i], "%lf", &(xyz_scale[0]));
			i++;
			sscanf(argv[i], "%lf", &(xyz_scale[1]));
			i++;
			sscanf(argv[i], "%lf", &(xyz_scale[2]));
    }
    else if (strcmp(argv[i],"-xyz_offset") == 0)
    {
      xyz_offset = new double[3];
			i++;
			sscanf(argv[i], "%lf", &(xyz_offset[0]));
			i++;
			sscanf(argv[i], "%lf", &(xyz_offset[1]));
			i++;
			sscanf(argv[i], "%lf", &(xyz_offset[2]));
    }
    else if (strcmp(argv[i],"-olas") == 0)
    {
      olas = true;
    }
    else if (strcmp(argv[i],"-olaz") == 0)
    {
      olaz = true;
    }
    else if (i == argc - 2 && num_file_name_in == 0 && file_name_out == 0)
    {
      file_names_in[0] = argv[i];
      num_file_name_in = 1;
    }
    else if (i == argc - 1 && num_file_name_in == 0 && file_name_out == 0)
    {
      file_names_in[0] = argv[i];
      num_file_name_in = 1;
    }
    else if (i == argc - 1 && num_file_name_in != 0 && file_name_out == 0)
    {
      file_name_out = argv[i];
    }
  }

  if (num_file_name_in == 0)
  {
    fprintf(stderr, "ERROR: no input specified\n");
    usage();
  }

  // if we have just one input file with extension different from *.las and *.LAS 
  if (num_file_name_in == 1 && strstr(file_names_in[0],".las") == 0 && strstr(file_names_in[0],".LAS") == 0)
  {
    char line[512];
    // then it should be a file containing filenames and we load the filenames
    num_file_name_in = 0;
    FILE* file = fopen(file_names_in[0], "r");
	  while (fgets(line, sizeof(char) * 512, file))
    {
      if (strstr(line,".las") || strstr(line,".LAS") || strstr(line,".laz") || strstr(line,".LAZ"))
      {
        if (num_file_name_in == alloced_file_name_in)
        {
          alloced_file_name_in *= 2;
          file_names_in = (char**)realloc(file_names_in,sizeof(char*)*alloced_file_name_in);
        }
        file_names_in[num_file_name_in] = strdup(line);
        i = strlen(file_names_in[num_file_name_in]) - 1;
        while (i && file_names_in[num_file_name_in][i] != 's' && file_names_in[num_file_name_in][i] != 'S' && file_names_in[num_file_name_in][i] != 'z' && file_names_in[num_file_name_in][i] != 'Z') i--;
        if (i)
        {
          file_names_in[num_file_name_in][i+1] = '\0';
          num_file_name_in++;
        }
        else
        {
          fprintf(stderr, "WARNING: cannot parse line '%s'\n",line);
        }
      }
      else
      {
        fprintf(stderr, "WARNING: no a valid LAS file name '%s'\n",line);
      }
    }
  }

  // create merged header
  LASheader merged_header;

  if (verbose) ptime("starting first pass.");
  fprintf(stderr, "first pass ... reading headers of %d LAS files...\n", num_file_name_in);

  for (i = 0; i < num_file_name_in; i++)
  {
    FILE* file_in;
    if (strstr(file_names_in[i], ".gz"))
    {
#ifdef _WIN32
      file_in = fopenGzipped(file_names_in[i], "rb");
#else
      fprintf(stderr, "ERROR: no support for gzipped input\n");
      exit(1);
#endif
    }
    else
    {
      file_in = fopen(file_names_in[i], "rb");
    }
    if (file_in == 0)
    {
      fprintf(stderr, "ERROR: could not open '%s'\n",file_names_in[i]);
      usage();
    }

    LASreader* lasreader = new LASreader();

    if (lasreader->open(file_in, false) == false)
    {
      fprintf(stderr, "ERROR: could not open lasreader\n");
      exit(1);
    }

    if (i == 0)
    {
      merged_header = lasreader->header;
    }
    else
    {
      merged_header.number_of_point_records += lasreader->header.number_of_point_records;
      merged_header.number_of_points_by_return[0] += lasreader->header.number_of_points_by_return[0];
      merged_header.number_of_points_by_return[1] += lasreader->header.number_of_points_by_return[1];
      merged_header.number_of_points_by_return[2] += lasreader->header.number_of_points_by_return[2];
      merged_header.number_of_points_by_return[3] += lasreader->header.number_of_points_by_return[3];
      merged_header.number_of_points_by_return[4] += lasreader->header.number_of_points_by_return[4];
      if (merged_header.max_x < lasreader->header.max_x) merged_header.max_x = lasreader->header.max_x;
      if (merged_header.max_y < lasreader->header.max_y) merged_header.max_y = lasreader->header.max_y;
      if (merged_header.max_z < lasreader->header.max_z) merged_header.max_z = lasreader->header.max_z;
      if (merged_header.min_x > lasreader->header.min_x) merged_header.min_x = lasreader->header.min_x;
      if (merged_header.min_y > lasreader->header.min_y) merged_header.min_y = lasreader->header.min_y;
      if (merged_header.min_z > lasreader->header.min_z) merged_header.min_z = lasreader->header.min_z;
    }

    lasreader->close();
    fclose(file_in);
  }

  if (verbose)
  {
    fprintf(stderr, "number_of_point_records %d\n",merged_header.number_of_point_records);
    fprintf(stderr, "number_of_points_by_return %d %d %d %d %d\n",merged_header.number_of_points_by_return[0],merged_header.number_of_points_by_return[1],merged_header.number_of_points_by_return[2],merged_header.number_of_points_by_return[3],merged_header.number_of_points_by_return[4]);
    fprintf(stderr, "x min %.8g max %.8\n", merged_header.min_x, merged_header.max_x);
    fprintf(stderr, "y min %.8g max %.8\n", merged_header.min_y, merged_header.max_y);
    fprintf(stderr, "z min %.8g max %.8\n", merged_header.min_z, merged_header.max_z);
  }

  if (xyz_scale)
  {
    merged_header.x_scale_factor = xyz_scale[0];
    merged_header.y_scale_factor = xyz_scale[1];
    merged_header.z_scale_factor = xyz_scale[2];
  }

  if (xyz_offset)
  {
    merged_header.x_offset = xyz_scale[0];
    merged_header.y_offset = xyz_scale[1];
    merged_header.z_offset = xyz_scale[2];
  }

  // check if the merged_header can support the enlarged bounding box
  int smallest_int = (1<<31)+10;
  int largest_int = smallest_int-1-20;

  // check x scale
  if (((merged_header.max_x - merged_header.x_offset) / merged_header.x_scale_factor) > largest_int ||
      ((merged_header.min_x - merged_header.x_offset) / merged_header.x_scale_factor) < smallest_int )
  {
    merged_header.x_scale_factor = 0.0000001;
    while (((merged_header.max_x - merged_header.x_offset) / merged_header.x_scale_factor) > largest_int ||
          ((merged_header.min_x - merged_header.x_offset) / merged_header.x_scale_factor) < smallest_int )
    {
      merged_header.x_scale_factor *= 10;
    }
    fprintf(stderr, "x_scale_factor of merged_header changed to %g\n", merged_header.x_scale_factor);
  }

  // check y scale
  if (((merged_header.max_y - merged_header.y_offset) / merged_header.y_scale_factor) > largest_int ||
      ((merged_header.min_y - merged_header.y_offset) / merged_header.y_scale_factor) < smallest_int )
  {
    merged_header.y_scale_factor = 0.0000001;
    while (((merged_header.max_y - merged_header.y_offset) / merged_header.y_scale_factor) > largest_int ||
          ((merged_header.min_y - merged_header.y_offset) / merged_header.y_scale_factor) < smallest_int )
    {
      merged_header.y_scale_factor *= 10;
    }
    fprintf(stderr, "y_scale_factor of merged_header changed to %g\n", merged_header.y_scale_factor);
  }
  
  // check z offset and z scale
  if (((merged_header.max_z - merged_header.z_offset) / merged_header.z_scale_factor) > largest_int ||
      ((merged_header.min_z - merged_header.z_offset) / merged_header.z_scale_factor) < smallest_int )
  {
    merged_header.z_scale_factor = 0.0000001;
    // re-check z offset and z scale
    if (((merged_header.max_z - merged_header.z_offset) / merged_header.z_scale_factor) > largest_int ||
        ((merged_header.min_z - merged_header.z_offset) / merged_header.z_scale_factor) < smallest_int )
    {
      merged_header.z_scale_factor *= 10;
    }
    fprintf(stderr, "z_scale_factor of merged_header changed to %g\n", merged_header.z_scale_factor);
  }
  

  // quit if no output specified

  if (file_name_out == 0 && !olas && !olaz)
  {
    fprintf(stderr, "no output specified. exiting ...\n");
    exit(1);
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

  // create the writer

  LASwriter* laswriter = new LASwriter();

  if (laswriter->open(file_out, &merged_header, compression) == false)
  {
    fprintf(stderr, "ERROR: could not open laswriter\n");
    exit(1);
  }

  if (verbose) ptime("starting second pass.");
  fprintf(stderr, "second pass ... merge %d LAS files into one ...\n", num_file_name_in);

  for (i = 0; i < num_file_name_in; i++)
  {
    FILE* file_in;
    if (strstr(file_names_in[i], ".gz"))
    {
#ifdef _WIN32
      file_in = fopenGzipped(file_names_in[i], "rb");
#else
      fprintf(stderr, "ERROR: no support for gzipped input\n");
      exit(1);
#endif
    }
    else
    {
      file_in = fopen(file_names_in[i], "rb");
    }
    if (file_in == 0)
    {
      fprintf(stderr, "ERROR: could not open '%s'\n",file_names_in[i]);
      usage();
    }

    LASreader* lasreader = new LASreader();

    if (lasreader->open(file_in, i != 0) == false)
    {
      fprintf(stderr, "ERROR: could not open lasreader\n");
      exit(1);
    }

    if (i == 0)
    {
      // copy variable header (and user data) from the first file
      for (unsigned int u = lasreader->header.header_size; u < lasreader->header.offset_to_point_data; u++)
      {
        fputc(fgetc(file_in),file_out);
      }
    }

    // do reader and writer have the same point transformation
    bool same = true;
    if (merged_header.x_offset != lasreader->header.x_offset) same = false;
    if (merged_header.y_offset != lasreader->header.y_offset) same = false;
    if (merged_header.z_offset != lasreader->header.z_offset) same = false;
    if (merged_header.x_scale_factor != lasreader->header.x_scale_factor) same = false;
    if (merged_header.y_scale_factor != lasreader->header.y_scale_factor) same = false;
    if (merged_header.z_scale_factor != lasreader->header.z_scale_factor) same = false;

    if (same)
    {
      while (lasreader->read_point())
      {
        laswriter->write_point(&(lasreader->point), lasreader->gps_time);
      }
    }
    else
    {
      double xyz[3];
      while (lasreader->read_point(xyz))
      {
			  lasreader->point.x = (int)(0.5 + (xyz[0] - merged_header.x_offset) / merged_header.x_scale_factor);
			  lasreader->point.y = (int)(0.5 + (xyz[1] - merged_header.y_offset) / merged_header.y_scale_factor);
			  lasreader->point.z = (int)(0.5 + (xyz[2] - merged_header.z_offset) / merged_header.z_scale_factor);
        laswriter->write_point(&(lasreader->point), lasreader->gps_time);
      }
    }

    lasreader->close();
    fclose(file_in);
  }

  laswriter->close();
  fclose(file_out);

  if (verbose) ptime("done.");

  return 0;
}
