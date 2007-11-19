/*
===============================================================================

  FILE:  laspointwriter0raw.h
  
  CONTENTS:
  
    Writes a point of type 0 (without gps_time) in standard LAS format 1.1

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    21 February 2007 -- created after having the longest talk ever with my aunt
  
===============================================================================
*/
#ifndef LAS_POINT_WRITER_0RAW_H
#define LAS_POINT_WRITER_0RAW_H

#include "laspointwriter.h"

#include <stdio.h>

class LASpointWriter0raw : public LASpointWriter
{
public:
  inline bool write_point(LASpoint* point, double gps_time = 0)
  {
    return (fwrite(point, sizeof(LASpoint), 1, file) == 1);
  };

  LASpointWriter0raw(FILE* file){this->file = file;};
  ~LASpointWriter0raw(){};
private:
  FILE* file;
};

#endif
