/***************************************************************************
 * $Id: laspointreader.h 89 2007-12-16 03:16:41Z hobu $
 * $Date: 2007-12-15 22:16:41 -0500 (Sat, 15 Dec 2007) $
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Common interface for the classes that read points raw or compressed.
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LAS_POINT_READER_H
#define LAS_POINT_READER_H

#include "lasdefinitions.h"

class LASpointReader
{
public:
  virtual bool read_point(LASpoint* point, double* gps_time = 0)=0;
  virtual ~LASpointReader(){};
};

#endif
