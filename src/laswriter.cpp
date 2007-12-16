/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS writer implemenation
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#include "laswriter.h"

#define ENABLE_LAS_COMPRESSION_SUPPORT
#undef ENABLE_LAS_COMPRESSION_SUPPORT

#include "laspointwriter0raw.h"
#include "laspointwriter1raw.h"

#ifdef ENABLE_LAS_COMPRESSION_SUPPORT
#include "laspointwriter0compressed.h"
#include "laspointwriter1compressed.h"
#endif // ENABLE_LAS_COMPRESSION_SUPPORT

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <cstdlib>
#include <cstring>

bool LASwriter::open(FILE* file, LASheader* header, int compression)
{
  if (file == 0)
  {
    fprintf(stderr,"ERROR: file pointer is zero\n");
    return false;
  }

  if (header == 0)
  {
    fprintf(stderr,"ERROR: header pointer is zero\n");
    return false;
  }

#ifdef _WIN32
  if (file == stdout)
  {
    if(_setmode( _fileno( stdout ), _O_BINARY ) == -1 )
    {
      fprintf(stderr, "ERROR: cannot set stdout to binary (untranslated) mode\n");
    }
  }
#endif

  this->file = file;

  // check header contents

  if (strncmp(header->file_signature, "LASF", 4) != 0)
  {
    fprintf(stderr,"ERROR: wrong file signature '%s'\n", header->file_signature);
    return false;
  }
  if ((header->version_major != 1) || ((header->version_minor != 0) && (header->version_minor != 1)))
  {
    fprintf(stderr,"WARNING: unknown version %d.%d (should be 1.0 or 1.1)\n", header->version_major, header->version_minor);
  }
  if (header->header_size != 227)
  {
    fprintf(stderr,"WARNING: header size is %d but should be 227\n", header->header_size);
  }
  if (header->offset_to_point_data < header->header_size)
  {
    fprintf(stderr,"ERROR: offset to point data %d is smaller than header size %d\n", header->offset_to_point_data, header->header_size);
    return false;
  }
  if (header->point_data_format == 0)
  {
    if (header->point_data_record_length != 20)
    {
      fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", header->point_data_record_length);
    }
  }
  else if (header->point_data_format == 1)
  {
    if (header->point_data_record_length != 28)
    {
      fprintf(stderr,"WARNING: wrong point data record length of %d instead of 28 for format 1\n", header->point_data_record_length);
    }
  }
  else
  {
    fprintf(stderr,"WARNING: unknown point data format %d\n", header->point_data_format);
  }
  if (header->x_scale_factor == 0 || header->y_scale_factor == 0 || header->z_scale_factor == 0)
  {
    fprintf(stderr,"WARNING: some scale factors are zero %g %g %g. those are set to 0.01.\n", header->x_scale_factor, header->y_scale_factor, header->z_scale_factor);
    if (header->x_scale_factor == 0) header->x_scale_factor = 0.01;
    if (header->y_scale_factor == 0) header->y_scale_factor = 0.01;
    if (header->z_scale_factor == 0) header->z_scale_factor = 0.01;
  }
  if (header->max_x < header->min_x || header->max_y < header->min_y || header->max_z < header->min_z)
  {
    fprintf(stderr,"WARNING: invalid bounding box [ %g %g %g / %g %g %g ]\n", header->min_x, header->min_y, header->min_z, header->max_x, header->max_y, header->max_z);
  }

  // create the right point writer in dependance on compression and point data format

  if (compression)
  {
#ifdef ENABLE_LAS_COMPRESSION_SUPPORT
    if (header->point_data_format)
    {
      pointWriter = new LASpointWriter1compressed(file);
    }
    else
    {
      pointWriter = new LASpointWriter0compressed(file);
    }
    // change the format to compressed
    header->point_data_format |= 128;
#else // ENABLE_LAS_COMPRESSION_SUPPORT
    fprintf(stderr,"ERROR: this version of the laswriter does not support compression\n");
    return false;
#endif // ENABLE_LAS_COMPRESSION_SUPPORT
  }
  else
  {
    if (header->point_data_format)
    {
      pointWriter = new LASpointWriter1raw(file);
    }
    else
    {
      pointWriter = new LASpointWriter0raw(file);
    }
  }

  // write header variable after variable (to avoid alignment issues)

  if (fwrite(&(header->file_signature), sizeof(char), 4, file) != 4)
  {
    fprintf(stderr,"ERROR: writing header->file_signature\n");
    return false;
  }
  if (fwrite(&(header->file_source_id), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->file_source_id\n");
    return false;
  }
  if (fwrite(&(header->reserved), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->reserved\n");
    return false;
  }
  if (fwrite(&(header->project_ID_GUID_data_1), sizeof(unsigned int), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->project_ID_GUID_data_1\n");
    return false;
  }
  if (fwrite(&(header->project_ID_GUID_data_2), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->project_ID_GUID_data_2\n");
    return false;
  }
  if (fwrite(&(header->project_ID_GUID_data_3), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->project_ID_GUID_data_3\n");
    return false;
  }
  if (fwrite(&(header->project_ID_GUID_data_4), sizeof(char), 8, file) != 8)
  {
    fprintf(stderr,"ERROR: writing header->project_ID_GUID_data_4\n");
    return false;
  }
  if (fwrite(&(header->version_major), sizeof(char), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->version_major\n");
    return false;
  }
  if (fwrite(&(header->version_minor), sizeof(char), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->version_minor\n");
    return false;
  }
  if (fwrite(&(header->system_identifier), sizeof(char), 32, file) != 32)
  {
    fprintf(stderr,"ERROR: writing header->system_identifier\n");
    return false;
  }
  if (fwrite(&(header->generating_software), sizeof(char), 32, file) != 32)
  {
    fprintf(stderr,"ERROR: writing header->generating_software\n");
    return false;
  }
  if (fwrite(&(header->file_creation_day), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->file_creation_day\n");
    return false;
  }
  if (fwrite(&(header->file_creation_year), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->file_creation_year\n");
    return false;
  }
  if (fwrite(&(header->header_size), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->header_size\n");
    return false;
  }
  if (fwrite(&(header->offset_to_point_data), sizeof(unsigned int), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->offset_to_point_data\n");
    return false;
  }
  if (fwrite(&(header->number_of_variable_length_records), sizeof(unsigned int), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->number_of_variable_length_records\n");
    return false;
  }
  if (fwrite(&(header->point_data_format), sizeof(unsigned char), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->point_data_format\n");
    return false;
  }
  if (fwrite(&(header->point_data_record_length), sizeof(unsigned short), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->point_data_record_length\n");
    return false;
  }
  if (fwrite(&(header->number_of_point_records), sizeof(unsigned int), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->number_of_point_records\n");
    return false;
  }
  if (fwrite(&(header->number_of_points_by_return), sizeof(unsigned int), 5, file) != 5)
  {
    fprintf(stderr,"ERROR: writing header->number_of_points_by_return\n");
    return false;
  }
  if (fwrite(&(header->x_scale_factor), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->x_scale_factor\n");
    return false;
  }
  if (fwrite(&(header->y_scale_factor), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->y_scale_factor\n");
    return false;
  }
  if (fwrite(&(header->z_scale_factor), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->z_scale_factor\n");
    return false;
  }
  if (fwrite(&(header->x_offset), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->x_offset\n");
    return false;
  }
  if (fwrite(&(header->y_offset), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->y_offset\n");
    return false;
  }
  if (fwrite(&(header->z_offset), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->z_offset\n");
    return false;
  }
  if (fwrite(&(header->max_x), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->max_x\n");
    return false;
  }
  if (fwrite(&(header->min_x), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->min_x\n");
    return false;
  }
  if (fwrite(&(header->max_y), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->max_y\n");
    return false;
  }
  if (fwrite(&(header->min_y), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->min_y\n");
    return false;
  }
  if (fwrite(&(header->max_z), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->max_z\n");
    return false;
  }
  if (fwrite(&(header->min_z), sizeof(double), 1, file) != 1)
  {
    fprintf(stderr,"ERROR: writing header->min_z\n");
    return false;
  }

#ifdef ENABLE_LAS_COMPRESSION_SUPPORT
  // change the format to compressed back to what it was
  header->point_data_format &= 127;
#endif // ENABLE_LAS_COMPRESSION_SUPPORT

  npoints = header->number_of_point_records;
  p_count = 0;

  return true;
}

bool LASwriter::write_point(LASpoint* point, double gps_time)
{
  p_count++;
  return pointWriter->write_point(point, gps_time);
}

void LASwriter::close()
{
  if (npoints && p_count != npoints)  fprintf(stderr,"WARNING: written %d points but expected %d points\n", p_count, npoints);
  p_count = -1;
  file = 0;
  if (pointWriter) 
  {
    delete pointWriter;
    pointWriter = 0;
  }
}

LASwriter::LASwriter()
{
  npoints = -1;
  p_count = -1;
  file = 0;
  pointWriter = 0;
}

LASwriter::~LASwriter()
{
}
