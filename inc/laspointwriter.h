/*
===============================================================================

  FILE:  LASpointWriter.h
  
  CONTENTS:
  
    Common interface for the two classes that write points raw or compressed.

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
