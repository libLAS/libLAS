/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Header and Point classes for reading and writing LIDAR points
 *          in the LAS format
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LAS_DEFINITIONS_H
#define LAS_DEFINITIONS_H

class LASpoint
{
public:
  int x;
  int y;
  int z;
  unsigned short intensity;
  unsigned char return_number : 3;
  unsigned char number_of_returns_of_given_pulse : 3;
  unsigned char scan_direction_flag : 1;
  unsigned char edge_of_flight_line : 1;
  unsigned char classification;
  char scan_angle_rank;
  unsigned char user_data;
  unsigned short point_source_ID;

  LASpoint()
  {
    x=0;
    y=0;
    z=0;
    intensity=0;
    edge_of_flight_line=0;
    scan_direction_flag=0;
    number_of_returns_of_given_pulse = 0;
    return_number = 0;
    classification = 0;
    scan_angle_rank = 0;
    user_data = 0;
    point_source_ID = 0;
  };
};

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

class LASvariable_header
{
public:
  unsigned short reserved;
  char user_id[16]; 
  unsigned short record_id;
  unsigned short record_length_after_header;
  char description[32];
};

class LASvariable_header_geo_keys
{
public:
  unsigned short key_directory_version;
  unsigned short key_revision;
  unsigned short minor_revision;
  unsigned short number_of_keys;
};

class LASvariable_header_key_entry
{
public:
  unsigned short key_id;
  unsigned short tiff_tag_location;
  unsigned short count;
  unsigned short value_offset;
};

static const char * LASpointClassification [] = {
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

#endif
