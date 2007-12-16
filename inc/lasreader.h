/***************************************************************************
 * $Id$
 * $Date$
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

#ifndef LAS_READER_H
#define LAS_READER_H

#include "lasdefinitions.h"
#include "laspointreader.h"

#include <cstdio>

class LASreader
{
public:
  LASheader header;
  LASpoint point;
  double gps_time;

  int npoints;
  int p_count;

  bool open(FILE* file, bool skip_to_point_data=true, bool only_skip_variable_header=false);

  bool read_point();
  bool read_point(float* coordinates);
  bool read_point(double* coordinates);

  void close();

  LASreader();
  ~LASreader();

private:
  FILE* file;
  LASpointReader* pointReader;
};

#endif
