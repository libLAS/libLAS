/*
===============================================================================

  FILE:  laspointreader0compressed.h
  
  CONTENTS:
  
    Reads a point of type 0 (without gps_time) from our compressed LAS format 1.1

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    22 February 2007 -- created 45 minutes before henna's 32nd birthday
  
===============================================================================
*/
#ifndef LAS_POINT_READER_0COMPRESSED_H
#define LAS_POINT_READER_0COMPRESSED_H

#include "laspointreader.h"

#include "rangemodel.h"
#include "rangedecoder.h"
#include "integercompressor_newer.h"

#include <stdio.h>

class LASpointReader0compressed : public LASpointReader
{
public:
  bool read_point(LASpoint* point, double* gps_time = 0);
  LASpointReader0compressed(FILE* file);
  ~LASpointReader0compressed();

private:
  FILE* file;
  LASpoint last_point;
  int last_x_diff;
  int last_y_diff;
  void init_decoder();
  RangeDecoder* rd;
  IntegerCompressorNewer* ic_dx;
  IntegerCompressorNewer* ic_dy;
  IntegerCompressorNewer* ic_z;
  RangeModel* rm_changed_values;
  IntegerCompressorNewer* ic_intensity;
  RangeModel* rm_bit_byte;
  RangeModel* rm_classification;
  IntegerCompressorNewer* ic_scan_angle_rank;
  RangeModel* rm_user_data;
  IntegerCompressorNewer* ic_point_source_ID;
};

#endif
