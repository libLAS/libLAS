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
        eMediumVegetation,
        eHighVegetation,
        eBuilding,
        eLowPoint,
        eModelKeyPoint,
        eWater = 9,
        // = 10 // reserved for ASPRS Definition
        // = 11 // reserved for ASPRS Definition
        eOverlapPoints = 12
        // = 13-31 // reserved for ASPRS Definition
    };

    LASPoint(double dX, double dY, double dZ, uint16_t nIntensity, double dTime);

    double GetX() const;
    double GetY() const;
    double GetZ() const;
    uint16_t GetIntensity() const;
    double GetTime() const;
    ClassificationType GetClassification() const;

private:
    
    double m_dX;
    double m_dY;
    double m_dZ;
    uint16_t m_nIntensity;
    double m_dGPSTime;
    ClassificationType m_eClassification;
};

inline double LASPoint::GetX() const
{
    return m_dX;
}

inline double LASPoint::GetY() const
{
    return m_dY;
}

inline double LASPoint::GetZ() const
{
    return m_dZ;
}

inline uint16_t LASPoint::GetIntensity() const
{
    return m_nIntensity;
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
