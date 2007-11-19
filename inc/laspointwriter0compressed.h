/*
===============================================================================

  FILE:  laspointwriter0compressed.h
  
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
#ifndef LAS_POINT_WRITER_0COMPRESSED_H
#define LAS_POINT_WRITER_0COMPRESSED_H

#include "laspointwriter.h"

#include "rangemodel.h"
#include "rangeencoder.h"
#include "integercompressor_newer.h"

#include <stdio.h>

class LASpointWriter0compressed : public LASpointWriter
{
public:
  bool write_point(LASpoint* point, double gps_time = 0);
  LASpointWriter0compressed(FILE* file);
  ~LASpointWriter0compressed();

private:
  FILE* file;
  LASpoint last_point;
  int last_x_diff;
  int last_y_diff;
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
};

#endif
