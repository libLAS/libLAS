/***************************************************************************
 * $Id: lasheader.h 89 2007-12-16 03:16:41Z hobu $
 * $Date: 2007-12-15 22:16:41 -0500 (Sat, 15 Dec 2007) $
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Reads LIDAR points from the LAS format 
 *          (Version 1.x , March 07, 2005).
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/
 
#ifndef LASHEADER_H_INCLUDED
#define LASHEADER_H_INCLUDED

class LASheader
{
public:
  char file_signature[4];
  unsigned short file_source_id;
  unsigned short reserved;
  unsigned int project_ID_GUID_data_1;
  unsigned short project_ID_GUID_data_2;
  unsigned short project_ID_GUID_data_3;
  char project_ID_GUID_data_4[8];
  char version_major;
  char version_minor;
  char system_identifier[32];
  char generating_software[32];
  unsigned short file_creation_day;
  unsigned short file_creation_year;
  unsigned short header_size;
  unsigned int offset_to_point_data;
  unsigned int number_of_variable_length_records;
  unsigned char point_data_format;
  unsigned short point_data_record_length;
  unsigned int number_of_point_records;
  unsigned int number_of_points_by_return[5];
  double x_scale_factor;
  double y_scale_factor;
  double z_scale_factor;
  double x_offset;
  double y_offset;
  double z_offset;
  double max_x;
  double min_x;
  double max_y;
  double min_y;
  double max_z;
  double min_z;

  LASheader()
  {
    for (int i = 0; i < sizeof(LASheader); i++) ((char*)this)[i] = 0;
    file_signature[0] = 'L'; file_signature[1] = 'A'; file_signature[2] = 'S'; file_signature[3] = 'F';
    version_major = 1;
    version_minor = 1;
    header_size = 227;
    offset_to_point_data = 227;
    point_data_record_length = 20;
    x_scale_factor = 0.01;
    y_scale_factor = 0.01;
    z_scale_factor = 0.01;
  };
};


#endif