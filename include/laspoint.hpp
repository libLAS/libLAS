/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS Point class, and appropriate enumerations that go with. 
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef __INCLUDE_LASPOINT_HPP
#define __INCLUDE_LASPOINT_HPP

#include <exception>
#include <lastypes.hpp>
namespace liblas {

enum LASClassifications {
    Created = 0,
    Unclassified,
    Ground,
    LowVegetation,
    HighVegetation,
    Building,
    LowPoint,
    ModelKeyPoint,
    Water,
    OverlapPoints = 12
};

class LASPoint {
    float nfX;
    float nfY;
    float nfZ;
    float nfIntensity;
    double ndGPSTime;
    LASClassifications nClassification;
public:
    LASPoint(float nfX, float nfY, float nfZ, float nfIntensity, double ndTime);
    float GetX() const { return nfX; }
    float GetY() const { return nfY; }
    float GetZ() const { return nfZ; }
    float GetIntensity() const { return nfIntensity; }
    double GetTime() const { return ndGPSTime; }
    LASClassifications GetClassification() const { return nClassification; }
};

}; /* end namespace liblas */

#endif /* ndef __INCLUDE_LASPOINT_HPP */
