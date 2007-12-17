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

    LASPoint();
    LASPoint(float fX, float fY, float fZ, float fIntensity, double dTime);

    float GetX() const;
    float GetY() const;
    float GetZ() const;
    float GetIntensity() const;
    double GetTime() const;
    ClassificationType GetClassification() const;

private:
    
    float m_fX;
    float m_fY;
    float m_fZ;
    float m_fIntensity;
    double m_dGPSTime;
    ClassificationType m_eClassification;
};

inline float LASPoint::GetX() const
{
    return m_fX;
}
inline float LASPoint::GetY() const
{
    return m_fY;
}
inline float LASPoint::GetZ() const
{
    return m_fZ;
}
inline float LASPoint::GetIntensity() const
{
    return m_fIntensity;
}
inline double LASPoint::GetTime() const
{
    return m_dGPSTime;
}
inline LASPoint::ClassificationType LASPoint::GetClassification() const
{
    return m_eClassification;
}

} /* end namespace liblas */

#endif /* ndef LASPOINT_HPP_INCLUDED */
