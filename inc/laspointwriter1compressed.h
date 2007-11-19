/*
===============================================================================

  FILE:  laspointwriter1compressed.h
  
  CONTENTS:
  
    Writes a point of type 1 (with gps_time) in our compressed LAS format 1.1

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
#ifndef LAS_POINT_WRITER_1COMPRESSED_H
#define LAS_POINT_WRITER_1COMPRESSED_H

#include "laspointwriter.h"

#include "rangemodel.h"
#include "rangeencoder.h"
#include "integercompressor_newer.h"

#include <stdio.h>

class LASpointWriter1compressed : public LASpointWriter
{
public:
  bool write_point(LASpoint* point, double gps_time = 0);
  LASpointWriter1compressed(FILE* file);
  ~LASpointWriter1compressed();

private:
  FILE* file;
  LASpoint last_point;
  int last_x_diff;
  int last_y_diff;
  double last_gps_time;
  int last_gps_time_diff;
  void init_encoder();
  RangeEncoder* re;
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
  IntegerCompressorNewer* ic_gps_time;
  RangeModel* rm_gps_time_multi;
  int multi_extreme_counter;
};

#endif
