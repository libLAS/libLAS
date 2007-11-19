/*
===============================================================================

  FILE:  laspointreader0raw.h
  
  CONTENTS:
  
    Reads a point of type 0 (without gps_time) in standard LAS format 1.1

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    22 February 2007 -- created about an hour before henna's 32nd birthday
  
===============================================================================
*/
#ifndef LAS_POINT_READER_0RAW_H
#define LAS_POINT_READER_0RAW_H

#include "laspointreader.h"

#include <stdio.h>

class LASpointReader0raw : public LASpointReader
{
public:
  inline bool read_point(LASpoint* point, double* gps_time = 0)
  {
    return (fread(point, sizeof(LASpoint), 1, file) == 1);
  };
  LASpointReader0raw(FILE* file){this->file = file;};
  ~LASpointReader0raw(){};
private:
  FILE* file;
};

#endif
