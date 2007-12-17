/***************************************************************************
 * $Id: laspoint.hpp 109 2007-12-17 06:53:23Z mloskot $
 * $Date: 2007-12-17 07:53:23 +0100 (Mon, 17 Dec 2007) $
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

#include <laspoint.hpp>

namespace liblas
{

LASPoint::LASPoint(double dX, double dY, double dZ,
                   uint16_t nIntensity, double dTime)
    : m_dX(dX), m_dY(dY), m_dZ(dZ),
      m_nIntensity(nIntensity),
      m_dGPSTime(dTime)
{
}

} /* end namespace liblas */
