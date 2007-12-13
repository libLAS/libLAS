/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS translation with optional configuration
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lasreader.h"
#include "laswriter.h"


void usage()
{
  fprintf(stderr,"usage:\n");
  fprintf(stderr,"las2las -remove_extra_header in.las out.las\n");
  fprintf(stderr,"las2las -i in.las -clip 63000000 483450000 63050000 483500000 -o out.las\n");
  fprintf(stderr,"las2las -i in.las -eliminate_return 2 -o out.las\n");
  fprintf(stderr,"las2las -i in.las -eliminate_scan_angle_above 15 -o out.las\n");
  fprintf(stderr,"las2las -i in.las -eliminate_intensity_below 1000 -olas > out.las\n");
  fprintf(stderr,"las2las -i in.las -first_only -clip 63000000 483450000 63050000 483500000 -o out.las\n");
  fprintf(stderr,"las2las -i in.las -last_only -eliminate_intensity_below 2000 -olas > out.las\n");
  fprintf(stderr,"las2las -h\n");
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
  char* file_name_in = 0;
  char* file_name_out = 0;
  bool olas = false;
  bool olaz = false;
  int* clip_xy_min = 0;
  int* clip_xy_max = 0;
  bool remove_extra_header = false;
  int elim_return = 0;
  int elim_scan_angle_above = 0;
  int elim_intensity_below = 0;
  bool first_only = false;
  bool last_only = false;

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
      file_name_in = argv[i];
    }
    else if (strcmp(argv[i],"-o") == 0)
    {
      i++;
      file_name_out = argv[i];
    }
    else if (strcmp(argv[i],"-olas") == 0)
    {
      olas = true;
    }
    else if (strcmp(argv[i],"-olaz") == 0)
    {
      olaz = true;
    }
    else if (strcmp(argv[i],"-clip") == 0 || strcmp(argv[i],"-clip_xy") == 0)
    {
      clip_xy_min = new int[2];
      i++;
      clip_xy_min[0] = atoi(argv[i]);
      i++;
      clip_xy_min[1] = atoi(argv[i]);
      clip_xy_max = new int[2];
      i++;
      clip_xy_max[0] = atoi(argv[i]);
      i++;
      clip_xy_max[1] = atoi(argv[i]);
    }
    else if (strcmp(argv[i],"-eliminate_return") == 0 || strcmp(argv[i],"-elim_return") == 0 || strcmp(argv[i],"-elim_ret") == 0)
    {
      i++;
      elim_return |= (1 << atoi(argv[i]));
    }
    else if (strcmp(argv[i],"-eliminate_scan_angle_above") == 0 || strcmp(argv[i],"-elim_scan_angle_above") == 0)
    {
      i++;
      elim_scan_angle_above = atoi(argv[i]);
    }
    else if (strcmp(argv[i],"-eliminate_intensity_below") == 0 || strcmp(argv[i],"-elim_intensity_below") == 0)
    {
      i++;
      elim_intensity_below = atoi(argv[i]);
    }
    else if (strcmp(argv[i],"-first_only") == 0)
    {
      first_only = true;
    }
    else if (strcmp(argv[i],"-last_only") == 0)
    {
      last_only = true;
    }
    else if (strcmp(argv[i],"-remove_extra_header") == 0)
    {
      remove_extra_header = true;
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
//#ifdef _WIN32
//      file_in = fopenGzipped(file_name_in, "rb");
//#else
      fprintf(stderr, "ERROR: no support for gzipped input\n");
      exit(1);
//#endif
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
  else
  {
    fprintf(stderr, "ERROR: no input specified\n");
    usage();
  }

  LASreader* lasreader = new LASreader();

  if (lasreader->open(file_in, true) == false)
  {
    fprintf(stderr, "ERROR: could not open lasreader\n");
    exit(1);
  }

  // do the first pass

  bool first_surviving_point = true;
  unsigned int surviving_number_of_point_records = 0;
  unsigned int surviving_number_of_points_by_return[] = {0,0,0,0,0,0,0,0};
  LASpoint surviving_point_min;
  LASpoint surviving_point_max;
  double surviving_gps_time_min;
  double surviving_gps_time_max;

  int clipped = 0;
  int eliminated_return = 0;
  int eliminated_scan_angle = 0;
  int eliminated_intensity = 0;
  int eliminated_first_only = 0;
  int eliminated_last_only = 0;

  if (verbose) ptime("start.");
  fprintf(stderr, "first pass reading %d points ...\n", lasreader->npoints);

  while (lasreader->read_point())
  {
    // put the points through all the tests
    if (last_only && lasreader->point.return_number != lasreader->point.number_of_returns_of_given_pulse)
    {
      eliminated_last_only++;
      continue;
    }
    if (first_only && lasreader->point.return_number != 1)
    {
      eliminated_first_only++;
      continue;
    }
    if (clip_xy_min && (lasreader->point.x < clip_xy_min[0] || lasreader->point.y < clip_xy_min[1]))
    {
      clipped++;
      continue;
    }
    if (clip_xy_max && (lasreader->point.x > clip_xy_max[0] || lasreader->point.y > clip_xy_max[1]))
    {
      clipped++;
      continue;
    }
    if (elim_return && (elim_return & (1 << lasreader->point.return_number)))
    {
      eliminated_return++;
      continue;
    }
    if (elim_scan_angle_above && (lasreader->point.scan_angle_rank > elim_scan_angle_above || lasreader->point.scan_angle_rank < -elim_scan_angle_above))
    {
      eliminated_scan_angle++;
      continue;
    }
    if (elim_intensity_below && lasreader->point.intensity < elim_intensity_below)
    {
      eliminated_intensity++;
      continue;
    }
    surviving_number_of_point_records++;
    surviving_number_of_points_by_return[lasreader->point.return_number-1]++;
    if (first_surviving_point)
    {
      surviving_point_min = lasreader->point;
      surviving_point_max = lasreader->point;
      surviving_gps_time_min = lasreader->gps_time;
      surviving_gps_time_max = lasreader->gps_time;
      first_surviving_point = false;
    }
    else
    {
      if (lasreader->point.x < surviving_point_min.x) surviving_point_min.x = lasreader->point.x;
      else if (lasreader->point.x > surviving_point_max.x) surviving_point_max.x = lasreader->point.x;
      if (lasreader->point.y < surviving_point_min.y) surviving_point_min.y = lasreader->point.y;
      else if (lasreader->point.y > surviving_point_max.y) surviving_point_max.y = lasreader->point.y;
      if (lasreader->point.z < surviving_point_min.z) surviving_point_min.z = lasreader->point.z;
      else if (lasreader->point.z > surviving_point_max.z) surviving_point_max.z = lasreader->point.z;
      if (lasreader->point.intensity < surviving_point_min.intensity) surviving_point_min.intensity = lasreader->point.intensity;
      else if (lasreader->point.intensity > surviving_point_max.intensity) surviving_point_max.intensity = lasreader->point.intensity;
      if (lasreader->point.edge_of_flight_line < surviving_point_min.edge_of_flight_line) surviving_point_min.edge_of_flight_line = lasreader->point.edge_of_flight_line;
      else if (lasreader->point.edge_of_flight_line > surviving_point_max.edge_of_flight_line) surviving_point_max.edge_of_flight_line = lasreader->point.edge_of_flight_line;
      if (lasreader->point.scan_direction_flag < surviving_point_min.scan_direction_flag) surviving_point_min.scan_direction_flag = lasreader->point.scan_direction_flag;
      else if (lasreader->point.scan_direction_flag > surviving_point_max.scan_direction_flag) surviving_point_max.scan_direction_flag = lasreader->point.scan_direction_flag;
      if (lasreader->point.number_of_returns_of_given_pulse < surviving_point_min.number_of_returns_of_given_pulse) surviving_point_min.number_of_returns_of_given_pulse = lasreader->point.number_of_returns_of_given_pulse;
      else if (lasreader->point.number_of_returns_of_given_pulse > surviving_point_max.number_of_returns_of_given_pulse) surviving_point_max.number_of_returns_of_given_pulse = lasreader->point.number_of_returns_of_given_pulse;
      if (lasreader->point.return_number < surviving_point_min.return_number) surviving_point_min.return_number = lasreader->point.return_number;
      else if (lasreader->point.return_number > surviving_point_max.return_number) surviving_point_max.return_number = lasreader->point.return_number;
      if (lasreader->point.classification < surviving_point_min.classification) surviving_point_min.classification = lasreader->point.classification;
      else if (lasreader->point.classification > surviving_point_max.classification) surviving_point_max.classification = lasreader->point.classification;
      if (lasreader->point.scan_angle_rank < surviving_point_min.scan_angle_rank) surviving_point_min.scan_angle_rank = lasreader->point.scan_angle_rank;
      else if (lasreader->point.scan_angle_rank > surviving_point_max.scan_angle_rank) surviving_point_max.scan_angle_rank = lasreader->point.scan_angle_rank;
      if (lasreader->point.user_data < surviving_point_min.user_data) surviving_point_min.user_data = lasreader->point.user_data;
      else if (lasreader->point.user_data > surviving_point_max.user_data) surviving_point_max.user_data = lasreader->point.user_data;
      if (lasreader->point.point_source_ID < surviving_point_min.point_source_ID) surviving_point_min.point_source_ID = lasreader->point.point_source_ID;
      else if (lasreader->point.point_source_ID > surviving_point_max.point_source_ID) surviving_point_max.point_source_ID = lasreader->point.point_source_ID;
      if (lasreader->point.point_source_ID < surviving_point_min.point_source_ID) surviving_point_min.point_source_ID = lasreader->point.point_source_ID;
      else if (lasreader->point.point_source_ID > surviving_point_max.point_source_ID) surviving_point_max.point_source_ID = lasreader->point.point_source_ID;
      if (lasreader->gps_time < surviving_gps_time_min) surviving_gps_time_min = lasreader->gps_time;
      else if (lasreader->gps_time > surviving_gps_time_max) surviving_gps_time_max = lasreader->gps_time;
    }
  }

  if (eliminated_first_only) fprintf(stderr, "eliminated based on first returns only: %d\n", eliminated_first_only);
  if (eliminated_last_only) fprintf(stderr, "eliminated based on last returns only: %d\n", eliminated_last_only);
  if (clipped) fprintf(stderr, "clipped: %d\n", clipped);
  if (eliminated_return) fprintf(stderr, "eliminated based on return number: %d\n", eliminated_return);
  if (eliminated_scan_angle) fprintf(stderr, "eliminated based on scan angle: %d\n", eliminated_scan_angle);
  if (eliminated_intensity) fprintf(stderr, "eliminated based on intensity: %d\n", eliminated_intensity);

  lasreader->close();
  fclose(file_in);

  if (verbose)
  {
    fprintf(stderr, "x %d %d %d\n",surviving_point_min.x, surviving_point_max.x, surviving_point_max.x - surviving_point_min.x);
    fprintf(stderr, "y %d %d %d\n",surviving_point_min.y, surviving_point_max.y, surviving_point_max.y - surviving_point_min.y);
    fprintf(stderr, "z %d %d %d\n",surviving_point_min.z, surviving_point_max.z, surviving_point_max.z - surviving_point_min.z);
    fprintf(stderr, "intensity %d %d %d\n",surviving_point_min.intensity, surviving_point_max.intensity, surviving_point_max.intensity - surviving_point_min.intensity);
    fprintf(stderr, "edge_of_flight_line %d %d %d\n",surviving_point_min.edge_of_flight_line, surviving_point_max.edge_of_flight_line, surviving_point_max.edge_of_flight_line - surviving_point_min.edge_of_flight_line);
    fprintf(stderr, "scan_direction_flag %d %d %d\n",surviving_point_min.scan_direction_flag, surviving_point_max.scan_direction_flag, surviving_point_max.scan_direction_flag - surviving_point_min.scan_direction_flag);
    fprintf(stderr, "number_of_returns_of_given_pulse %d %d %d\n",surviving_point_min.number_of_returns_of_given_pulse, surviving_point_max.number_of_returns_of_given_pulse, surviving_point_max.number_of_returns_of_given_pulse - surviving_point_min.number_of_returns_of_given_pulse);
    fprintf(stderr, "return_number %d %d %d\n",surviving_point_min.return_number, surviving_point_max.return_number, surviving_point_max.return_number - surviving_point_min.return_number);
    fprintf(stderr, "classification %d %d %d\n",surviving_point_min.classification, surviving_point_max.classification, surviving_point_max.classification - surviving_point_min.classification);
    fprintf(stderr, "scan_angle_rank %d %d %d\n",surviving_point_min.scan_angle_rank, surviving_point_max.scan_angle_rank, surviving_point_max.scan_angle_rank - surviving_point_min.scan_angle_rank);
    fprintf(stderr, "user_data %d %d %d\n",surviving_point_min.user_data, surviving_point_max.user_data, surviving_point_max.user_data - surviving_point_min.user_data);
    fprintf(stderr, "point_source_ID %d %d %d\n",surviving_point_min.point_source_ID, surviving_point_max.point_source_ID, surviving_point_max.point_source_ID - surviving_point_min.point_source_ID);
    fprintf(stderr, "gps_time %.8f %.8f %.8f\n",surviving_gps_time_min, surviving_gps_time_max, surviving_gps_time_max - surviving_gps_time_min);
  }

  // quit if no output specified

  if (file_name_out == 0 && !olas && !olaz)
  {
    fprintf(stderr, "no output specified. exiting ...\n");
    exit(1);
  }

  // re-open input file

  if (file_name_in)
  {
    if (strstr(file_name_in, ".gz"))
    {
//#ifdef _WIN32
//      file_in = fopenGzipped(file_name_in, "rb");
//#else
      fprintf(stderr, "ERROR: no support for gzipped input\n");
      exit(1);
//#endif
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
  else
  {
    fprintf(stderr, "ERROR: no input specified\n");
    usage();
  }

  if (lasreader->open(file_in,remove_extra_header) == false)
  {
    fprintf(stderr, "ERROR: could not re-open lasreader\n");
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

  // prepare the header

  LASheader surviving_header = lasreader->header;
  surviving_header.number_of_point_records = surviving_number_of_point_records;
  for (i = 0; i < 5; i++) surviving_header.number_of_points_by_return[i] = surviving_number_of_points_by_return[i];
  surviving_header.min_x = surviving_point_min.x*surviving_header.x_scale_factor + surviving_header.x_offset;
  surviving_header.max_x = surviving_point_max.x*surviving_header.x_scale_factor + surviving_header.x_offset;
  surviving_header.min_y = surviving_point_min.y*surviving_header.y_scale_factor + surviving_header.y_offset;
  surviving_header.max_y = surviving_point_max.y*surviving_header.y_scale_factor + surviving_header.y_offset;
  surviving_header.min_z = surviving_point_min.z*surviving_header.z_scale_factor + surviving_header.z_offset;
  surviving_header.max_z = surviving_point_max.z*surviving_header.z_scale_factor + surviving_header.z_offset;
  if (remove_extra_header) surviving_header.offset_to_point_data = surviving_header.header_size;

  fprintf(stderr, "second pass reading %d and writing %d points ...\n", lasreader->npoints, surviving_number_of_point_records);

  LASwriter* laswriter = new LASwriter();

  if (laswriter->open(file_out, &surviving_header, compression) == false)
  {
    fprintf(stderr, "ERROR: could not open laswriter\n");
    exit(1);
  }

  // copy variable header (and user data)

  if (!remove_extra_header)
  {
    for (unsigned int u = lasreader->header.header_size; u < lasreader->header.offset_to_point_data; u++)
    {
      fputc(fgetc(file_in),file_out);
    }
  }

  // loop over points

  while (lasreader->read_point())
  {
    // put the points through all the tests
    if (last_only && lasreader->point.return_number != lasreader->point.number_of_returns_of_given_pulse)
    {
      continue;
    }
    if (first_only && lasreader->point.return_number != 1)
    {
      continue;
    }
    if (clip_xy_min && (lasreader->point.x < clip_xy_min[0] || lasreader->point.y < clip_xy_min[1]))
    {
      continue;
    }
    if (clip_xy_max && (lasreader->point.x > clip_xy_max[0] || lasreader->point.y > clip_xy_max[1]))
    {
      continue;
    }
    if (elim_return && (elim_return & (1 << lasreader->point.return_number)))
    {
      continue;
    }
    if (elim_scan_angle_above && (lasreader->point.scan_angle_rank > elim_scan_angle_above || lasreader->point.scan_angle_rank < -elim_scan_angle_above))
    {
      continue;
    }
    if (elim_intensity_below && lasreader->point.intensity < elim_intensity_below)
    {
      continue;
    }
    laswriter->write_point(&(lasreader->point), lasreader->gps_time);
  }

  laswriter->close();
  fclose(file_out);

  lasreader->close();
  fclose(file_in);

  if (verbose) ptime("done.");

  return 0;
}
