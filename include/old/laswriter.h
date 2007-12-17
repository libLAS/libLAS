/***************************************************************************
 * $Id: laswriter.h 89 2007-12-16 03:16:41Z hobu $
 * $Date: 2007-12-15 22:16:41 -0500 (Sat, 15 Dec 2007) $
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Writes LIDAR points from the LAS format 
 *          (Version 1.x , March 07, 2005).
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LAS_WRITER_H
#define LAS_WRITER_H

#include "lasdefinitions.h"
#include "laspointwriter.h"

#include <cstdio>

class LASwriter
{
public:
  int npoints;
  int p_count;

  bool open(FILE* file, LASheader* header, int compression = 0);

  bool write_point(LASpoint* point, double gps_time = 0);

  void close();

  LASwriter();
  ~LASwriter();

private:
  FILE* file;
  LASpointWriter* pointWriter;
};

#endif
