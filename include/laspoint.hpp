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

namespace liblas
{

class LASPoint
{
public:

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

    LASPoint(float nfX, float nfY, float nfZ, float nfIntensity, double ndTime);

    float GetX() const;
    float GetY() const;
    float GetZ() const;
    float GetIntensity() const;
    double GetTime() const;
    ClassificationType GetClassification() const;

private:
    
    float m_nfX;
    float m_nfY;
    float m_nfZ;
    float m_nfIntensity;
    double m_ndGPSTime;
    ClassificationType m_nClassification;
};

inline float LASPoint::GetX() const
{
    return m_nfX;
}
inline float LASPoint::GetY() const
{
    return m_nfY;
}
inline float LASPoint::GetZ() const
{
    return m_nfZ;
}
inline float LASPoint::GetIntensity() const
{
    return m_nfIntensity;
}
inline double LASPoint::GetTime() const
{
    return m_ndGPSTime;
}
inline LASPoint::ClassificationType LASPoint::GetClassification() const
{
    return m_nClassification;
}

}; /* end namespace liblas */

#endif /* ndef LASPOINT_HPP_INCLUDED */
