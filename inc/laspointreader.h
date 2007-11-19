/*
===============================================================================

  FILE:  LASpointReader.h
  
  CONTENTS:
  
    Common interface for the classes that read points raw or compressed.

  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2007  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    22 February 2007 -- created about an hour before henna's birthday
  
===============================================================================
*/
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
