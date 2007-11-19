/*
===============================================================================

  FILE:  LASreader.h
  
  CONTENTS:
  
    Reads LIDAR points from the LAS format (Version 1.x , March 07, 2005).

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    18 February 2007 -- created after repairing 2 vacuum cleaners in the garden
  
===============================================================================
*/
#ifndef LAS_READER_H
#define LAS_READER_H

#include "lasdefinitions.h"
#include "laspointreader.h"

#include <stdio.h>

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
