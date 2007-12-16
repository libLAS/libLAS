/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Header and Point classes for reading and writing LIDAR points
 *          in the LAS format
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg isenburg@cs.unc.edu 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/
 
#ifndef LASPOINT_H_INCLUDED
#define LASPOINT_H_INCLUDED

class LASpoint
{
public:
  int x;
  int y;
  int z;
  unsigned short intensity;
  unsigned char return_number : 3;
  unsigned char number_of_returns_of_given_pulse : 3;
  unsigned char scan_direction_flag : 1;
  unsigned char edge_of_flight_line : 1;
  unsigned char classification;
  char scan_angle_rank;
  unsigned char user_data;
  unsigned short point_source_ID;

  LASpoint()
  {
    x=0;
    y=0;
    z=0;
    intensity=0;
    edge_of_flight_line=0;
    scan_direction_flag=0;
    number_of_returns_of_given_pulse = 0;
    return_number = 0;
    classification = 0;
    scan_angle_rank = 0;
    user_data = 0;
    point_source_ID = 0;
  };
};

#endif