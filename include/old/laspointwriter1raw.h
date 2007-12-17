/***************************************************************************
 * $Id: laspointwriter1raw.h 89 2007-12-16 03:16:41Z hobu $
 * $Date: 2007-12-15 22:16:41 -0500 (Sat, 15 Dec 2007) $
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Writes a point of type 0 (with gps_time) in 
 *          standard LAS format 1.1
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LAS_POINT_WRITER_1RAW_H
#define LAS_POINT_WRITER_1RAW_H

#include "laspointwriter.h"

#include <cstdio>

class LASpointWriter1raw : public LASpointWriter
{
public:
  inline bool write_point(LASpoint* point, double gps_time = 0)
  {
    fwrite(point, sizeof(LASpoint), 1, file);
    return (fwrite(&(gps_time), sizeof(double), 1, file) == 1);
  }
  LASpointWriter1raw(FILE* file){this->file = file;};
  ~LASpointWriter1raw(){};
private:
  FILE* file;
};

#endif
