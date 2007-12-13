/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Common interface for the two classes that write 
 *          points raw or compressed.
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LAS_POINT_WRITER_H
#define LAS_POINT_WRITER_H

#include "lasdefinitions.h"

class LASpointWriter
{
public:
  virtual bool write_point(LASpoint* point, double gps_time = 0)=0;
  virtual ~LASpointWriter(){};
};

#endif
