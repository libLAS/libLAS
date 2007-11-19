/*
===============================================================================

  FILE:  LASwriter.h
  
  CONTENTS:
  
    Writes LIDAR points to the LAS format (Version 1.x, March 07, 2005).

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    21 February 2007 -- created after eating Sarah's veggies with peanutsauce
  
===============================================================================
*/
#ifndef LAS_WRITER_H
#define LAS_WRITER_H

#include "lasdefinitions.h"
#include "laspointwriter.h"

#include <stdio.h>

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
