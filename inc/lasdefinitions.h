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

#include "laspoint.h"

#include "lasheader.h"

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
