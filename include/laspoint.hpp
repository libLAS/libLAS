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

#ifndef LASPOINT_HPP_INCLUDED
#define LASPOINT_HPP_INCLUDED

#include <exception>
#include <lastypes.hpp>
namespace liblas {

enum ClassificationType
{
    eCreated = 0,
    eUnclassified,
    eGround,
    eLowVegetation,
    eHighVegetation,
    eBuilding,
    eLowPoint,
    eModelKeyPoint,
    eWater,
    eOverlapPoints = 12
};

class Point
{
public:
    Point(float nfX, float nfY, float nfZ, float nfIntensity, double ndTime);
    float GetX() const { return nfX; }
    float GetY() const { return nfY; }
    float GetZ() const { return nfZ; }
    float GetIntensity() const { return nfIntensity; }
    double GetTime() const { return ndGPSTime; }
    ClassificationType GetClassification() const { return nClassification; }
private:
    float nfX;
    float nfY;
    float nfZ;
    float nfIntensity;
    double ndGPSTime;
    ClassificationType nClassification;
};

}; /* end namespace liblas */

#endif /* ndef LASPOINT_HPP_INCLUDED */
