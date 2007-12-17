/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS translation to ASCII text with optional configuration
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lasreader.h"

void usage()
{
  fprintf(stderr,"usage:\n");
  fprintf(stderr,"las2txt lidar.las\n");
  fprintf(stderr,"las2txt -parse xyziar lidar.las lidar.txt\n");
  fprintf(stderr,"las2txt -i lidar.las -o lidar.laz -parse xyz\n");
  fprintf(stderr,"las2txt -parse xyzt -verbose lidar.las\n");
  fprintf(stderr,"las2txt -parse xyz lidar.las.gz\n");
  fprintf(stderr,"las2txt -h\n");
  fprintf(stderr,"---------------------------------------------\n");
  fprintf(stderr,"The '-parse txyz' flag specifies how to format each\n");
  fprintf(stderr,"each line of the ASCII file. For example, 'txyzia'\n");
  fprintf(stderr,"means that the first number of each line should be the\n");
  fprintf(stderr,"gpstime, the next three numbers should be the x, y, and\n");
  fprintf(stderr,"z coordinate, the next number should be the intensity\n");
  fprintf(stderr,"and the next number should be the scan angle.\n");
  fprintf(stderr,"The supported entries are a - scan angle, i - intensity,\n");
  fprintf(stderr,"n - number of returns for given pulse, r - number of\n");
  fprintf(stderr,"this return, c - classification, u - user data,\n");
  fprintf(stderr,"p - point source ID, e - edge of flight line flag, and\n");
  fprintf(stderr,"d - direction of scan flag.\n");
  fprintf(stderr,"---------------------------------------------\n");
  fprintf(stderr,"The '-sep space' flag specifies what separator to use. The\n");
  fprintf(stderr,"default is a space but 'tab', 'komma', 'colon', 'hyphen',\n");
  fprintf(stderr,"'dot', or 'semicolon' are other possibilities.\n");
  fprintf(stderr,"---------------------------------------------\n");
  fprintf(stderr,"The '-header pound' flag results in the header information\n");
  fprintf(stderr,"being printed at the beginning of the ASCII file in form of\n");
  fprintf(stderr,"a comment that starts with the special character '#'. Also\n");
  fprintf(stderr,"possible are 'percent', 'dollar', 'semicolon', 'komma',\n");
  fprintf(stderr,"'star', 'colon', or 'semicolon' as that special character.\n");
  exit(1);
}


static int lidardouble2string(char* string, double value)
{
  int len;
  len = sprintf(string, "%f", value) - 1;
  while (string[len] == '0') len--;
  if (string[len] != '.') len++;
  string[len] = '\0';
  return len;
}

static int lidardouble2string(char* string, double value0, double value1, double value2, bool eol)
{
  int len;
  len = sprintf(string, "%f", value0) - 1;
  while (string[len] == '0') len--;
  if (string[len] != '.') len++;
  len += sprintf(&(string[len]), " %f", value1) - 1;
  while (string[len] == '0') len--;
  if (string[len] != '.') len++;
  len += sprintf(&(string[len]), " %f", value2) - 1;
  while (string[len] == '0') len--;
  if (string[len] != '.') len++;
  if (eol) string[len++] = '\012';
  string[len] = '\0';
  return len;
}

int main(int argc, char *argv[])
{
  int i;
  bool ilas = false;
  bool otxt = false;
  bool verbose = false;
  char* file_name_in = 0;
  char* file_name_out = 0;
  char separator_sign = ' ';
  char header_comment_sign = '\0';
  char* parse_string = "xyz";
  char printstring[256];

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i],"-h") == 0)
    {
      usage();
    }
    else if (strcmp(argv[i],"-verbose") == 0)
    {
      verbose = true;
    }
    else if (strcmp(argv[i],"-parse") == 0)
    {
      i++;
      parse_string = argv[i];
    }
    else if (strcmp(argv[i],"-sep") == 0)
    {
      i++;
      if (strcmp(argv[i],"komma") == 0)
      {
        separator_sign = ',';
      }
      else if (strcmp(argv[i],"tab") == 0)
      {
        separator_sign = '\t';
      }
      else if (strcmp(argv[i],"dot") == 0 || strcmp(argv[i],"period") == 0)
      {
        separator_sign = '.';
      }
      else if (strcmp(argv[i],"colon") == 0)
      {
        separator_sign = ':';
      }
      else if (strcmp(argv[i],"scolon") == 0 || strcmp(argv[i],"semicolon") == 0)
      {
        separator_sign = ';';
      }
      else if (strcmp(argv[i],"hyphen") == 0 || strcmp(argv[i],"minus") == 0)
      {
        separator_sign = '-';
      }
      else if (strcmp(argv[i],"space") == 0)
      {
        separator_sign = ' ';
      }
      else
      {
        fprintf(stderr, "ERROR: unknown seperator '%s'\n",argv[i]);
        usage();
      }
    }
    else if ((strcmp(argv[i],"-header") == 0 || strcmp(argv[i],"-comment") == 0))
    {
      i++;
      if (strcmp(argv[i],"komma") == 0)
      {
        header_comment_sign = ',';
      }
      else if (strcmp(argv[i],"colon") == 0)
      {
        header_comment_sign = ':';
      }
      else if (strcmp(argv[i],"scolon") == 0 || strcmp(argv[i],"semicolon") == 0)
      {
        header_comment_sign = ';';
      }
      else if (strcmp(argv[i],"pound") == 0 || strcmp(argv[i],"hash") == 0)
      {
        header_comment_sign = '#';
      }
      else if (strcmp(argv[i],"percent") == 0)
      {
        header_comment_sign = '%';
      }
      else if (strcmp(argv[i],"dollar") == 0)
      {
        header_comment_sign = '$';
      }
      else if (strcmp(argv[i],"star") == 0)
      {
        header_comment_sign = '*';
      }
      else
      {
        fprintf(stderr, "ERROR: unknown comment symbol '%s'\n",argv[i]);
        usage();
      }
    }
    else if (strcmp(argv[i],"-ilas") == 0)
    {
      ilas = true;
    }
    else if (strcmp(argv[i],"-otxt") == 0)
    {
      otxt = true;
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
    else if (i == argc - 2 && file_name_in == 0 && file_name_out == 0)
    {
      file_name_in = argv[i];
    }
    else if (i == argc - 1 && file_name_in == 0 && file_name_out == 0)
    {
      file_name_in = argv[i];
    }
    else if (i == argc - 1 && file_name_in && file_name_out == 0)
    {
      file_name_out = argv[i];
    }
  }

  // open input file

  FILE* file_in;

  if (ilas)
  {
    // output from stdin
    file_in = stdin;
  }
  else
  {
    if (file_name_in)
    {
      if (strstr(file_name_in, ".gz"))
      {
        fprintf(stderr, "ERROR: no support for gzipped input\n");
        exit(1);
      }
      else
      {
        file_in = fopen(file_name_in, "rb");
      }
      if (file_in == 0)
      {
        fprintf(stderr, "ERROR: could not open '%s' for read\n",file_name_in);
        exit(1);
      }
    }
    else
    {
      fprintf(stderr, "ERROR: no input file specified\n");
      usage();
    }
  }

  LASreader* lasreader = new LASreader();
  if (lasreader->open(file_in) == false)
  {
    fprintf (stderr, "ERROR: lasreader open failed for '%s'\n", file_name_in);
  }

  // open output file
  
  FILE* file_out;

  if (otxt)
  {
    // output to stdout
    file_out = stdout;
  }
  else
  {
    // create output file name if needed 

    if (file_name_out == 0)
    {
      if (file_name_in == 0)
      {
        fprintf(stderr, "ERROR: no output file specified\n");
        usage();
      }

      int len = strlen(file_name_in);
      file_name_out = strdup(file_name_in);
      if (file_name_out[len-3] == '.' && file_name_out[len-2] == 'g' && file_name_out[len-1] == 'z')
      {
        len = len - 4;
      }
      while (len > 0 && file_name_out[len] != '.')
      {
        len--;
      }
      file_name_out[len] = '.';
      file_name_out[len+1] = 't';
      file_name_out[len+2] = 'x';
      file_name_out[len+3] = 't';
      file_name_out[len+4] = '\0';
    }

    // open output file

    file_out = fopen(file_name_out, "w");

    // fail if output file does not open

    if (file_out == 0)
    {
      fprintf(stderr, "ERROR: could not open '%s' for write\n",file_name_out);
      exit(1);
    }
  }

  // output header info

  if (verbose)
  {
    LASheader* header = &(lasreader->header);
    fprintf(stderr, "file signature:            '%s'\n", header->file_signature);
    fprintf(stderr, "file source ID:            %d\n", header->file_source_id);
    fprintf(stderr, "reserved:                  %d\n", header->reserved);
    fprintf(stderr, "project ID GUID data 1-4:  %d %d %d '%s'\n", header->project_ID_GUID_data_1, header->project_ID_GUID_data_2, header->project_ID_GUID_data_3, header->project_ID_GUID_data_4);
    fprintf(stderr, "version major.minor:       %d.%d\n", header->version_major, header->version_minor);
    fprintf(stderr, "system_identifier:         '%s'\n", header->system_identifier);
    fprintf(stderr, "generating_software:       '%s'\n", header->generating_software);
    fprintf(stderr, "file creation day/year:    %d/%d\n", header->file_creation_day, header->file_creation_year);
    fprintf(stderr, "header size                %d\n", header->header_size);
    fprintf(stderr, "offset to point data       %d\n", header->offset_to_point_data);
    fprintf(stderr, "number var. length records %d\n", header->number_of_variable_length_records);
    fprintf(stderr, "point data format          %d\n", header->point_data_format);
    fprintf(stderr, "point data record length   %d\n", header->point_data_record_length);
    fprintf(stderr, "number of point records    %d\n", header->number_of_point_records);
    fprintf(stderr, "number of points by return %d %d %d %d %d\n", header->number_of_points_by_return[0], header->number_of_points_by_return[1], header->number_of_points_by_return[2], header->number_of_points_by_return[3], header->number_of_points_by_return[4]);
    fprintf(stderr, "scale factor x y z         "); lidardouble2string(printstring, header->x_scale_factor, header->y_scale_factor, header->z_scale_factor, true); fprintf(stderr, printstring);
    fprintf(stderr, "offset x y z               "); lidardouble2string(printstring, header->x_offset, header->y_offset, header->z_offset, true); fprintf(stderr, printstring);
    fprintf(stderr, "min x y z                  "); lidardouble2string(printstring, header->min_x, header->min_y, header->min_z, true); fprintf(stderr, printstring);
    fprintf(stderr, "max x y z                  "); lidardouble2string(printstring, header->max_x, header->max_y, header->max_z, true); fprintf(stderr, printstring);
  }

  if (header_comment_sign)
  {
    LASheader* header = &(lasreader->header);
    fprintf(file_out, "%c file signature:            '%s'\012", header_comment_sign, header->file_signature);
    fprintf(file_out, "%c file source ID:            %d\012", header_comment_sign, header->file_source_id);
    fprintf(file_out, "%c reserved:                  %d\012", header_comment_sign, header->reserved);
    fprintf(file_out, "%c project ID GUID data 1-4:  %d %d %d '%s'\012", header_comment_sign, header->project_ID_GUID_data_1, header->project_ID_GUID_data_2, header->project_ID_GUID_data_3, header->project_ID_GUID_data_4);
    fprintf(file_out, "%c version major.minor:       %d.%d\012", header_comment_sign, header->version_major, header->version_minor);
    fprintf(file_out, "%c system_identifier:         '%s'\012", header_comment_sign, header->system_identifier);
    fprintf(file_out, "%c generating_software:       '%s'\012", header_comment_sign, header->generating_software);
    fprintf(file_out, "%c file creation day/year:    %d/%d\012", header_comment_sign, header->file_creation_day, header->file_creation_year);
    fprintf(file_out, "%c header size                %d\012", header_comment_sign, header->header_size);
    fprintf(file_out, "%c offset to point data       %d\012", header_comment_sign, header->offset_to_point_data);
    fprintf(file_out, "%c number var. length records %d\012", header_comment_sign, header->number_of_variable_length_records);
    fprintf(file_out, "%c point data format          %d\012", header_comment_sign, header->point_data_format);
    fprintf(file_out, "%c point data record length   %d\012", header_comment_sign, header->point_data_record_length);
    fprintf(file_out, "%c number of point records    %d\012", header_comment_sign, header->number_of_point_records);
    fprintf(file_out, "%c number of points by return %d %d %d %d %d\012", header_comment_sign, header->number_of_points_by_return[0], header->number_of_points_by_return[1], header->number_of_points_by_return[2], header->number_of_points_by_return[3], header->number_of_points_by_return[4]);
    fprintf(file_out, "%c scale factor x y z         ", header_comment_sign); lidardouble2string(printstring, header->x_scale_factor, header->y_scale_factor, header->z_scale_factor, true); fprintf(file_out, printstring);
    fprintf(file_out, "%c offset x y z               ", header_comment_sign); lidardouble2string(printstring, header->x_offset, header->y_offset, header->z_offset, true); fprintf(file_out, printstring);
    fprintf(file_out, "%c min x y z                  ", header_comment_sign); lidardouble2string(printstring, header->min_x, header->min_y, header->min_z, true); fprintf(file_out, printstring);
    fprintf(file_out, "%c max x y z                  ", header_comment_sign); lidardouble2string(printstring, header->max_x, header->max_y, header->max_z, true); fprintf(file_out, printstring);
  }

  double xyz[3];

  // read and convert the points to ASCII

  while (lasreader->read_point(xyz))
  {
    i = 0;
    while (true)
    {
      switch (parse_string[i])
      {
      case 'x': // the x coordinate
        lidardouble2string(printstring, xyz[0]); fprintf(file_out, printstring);
        break;
      case 'y': // the y coordinate
        lidardouble2string(printstring, xyz[1]); fprintf(file_out, printstring);
        break;
      case 'z': // the z coordinate
        lidardouble2string(printstring, xyz[2]); fprintf(file_out, printstring);
        break;
      case 't': // the gps-time
        lidardouble2string(printstring,lasreader->gps_time); fprintf(file_out, printstring);
        break;
      case 'i': // the intensity
        fprintf(file_out, "%d", lasreader->point.intensity);
        break;
      case 'a': // the scan angle
        fprintf(file_out, "%d", lasreader->point.scan_angle_rank);
        break;
      case 'r': // the number of the return
        fprintf(file_out, "%d", lasreader->point.return_number);
        break;
      case 'c': // the classification
        fprintf(file_out, "%d", lasreader->point.classification);
        break;
      case 'u': // the user data
        fprintf(file_out, "%d", lasreader->point.user_data);
        break;
      case 'n': // the number of returns of given pulse
        fprintf(file_out, "%d", lasreader->point.number_of_returns_of_given_pulse);
        break;
      case 'p': // the point source ID
        fprintf(file_out, "%d", lasreader->point.point_source_ID);
        break;
      case 'e': // the edge of flight line flag
        fprintf(file_out, "%d", lasreader->point.edge_of_flight_line);
        break;
      case 'd': // the direction of scan flag
        fprintf(file_out, "%d", lasreader->point.scan_direction_flag);
        break;
      }
      i++;
      if (parse_string[i])
      {
        fprintf(file_out, "%c", separator_sign);
      }
      else
      {
        fprintf(file_out, "\012");
        break;
      }
    }
  }

  // close the reader

  lasreader->close();

  // close the input file
  
  fclose(file_in);

  // close the output file

  fclose(file_out);

  return 0;
}
