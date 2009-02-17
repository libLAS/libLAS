/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS SRS class 
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2009, Howard Butler
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/

#ifndef LIBLAS_LASSRS_HPP_INCLUDED
#define LIBLAS_LASSRS_HPP_INCLUDED

#include <liblas/lasrecordheader.hpp>

#include <liblas/cstdint.hpp>
#include <liblas/detail/fwd.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/exception.hpp>

// GDAL OSR
#ifdef HAVE_GDAL
#include "ogr_srs_api.h"
#include "cpl_port.h"
#include "cpl_serv.h"
#include "geo_tiffp.h"
#define CPL_ERROR_H_INCLUDED

#include "geo_normalize.h"
#include "geovalues.h"
#include "ogr_spatialref.h"
#include "gdal.h"
#include "xtiffio.h"
#include "cpl_multiproc.h"

#endif

// GeoTIFF
#ifdef HAVE_LIBGEOTIFF
#include "geotiff.h"
#include "geo_simpletags.h"
#include "geo_normalize.h"
#include "geo_simpletags.h"
#include "geovalues.h"
#endif // HAVE_LIBGEOTIFF

// std
#include <stdexcept> // std::out_of_range
#include <cstdlib> // std::size_t
#include <string>

namespace liblas {

/// Spatial Reference System container for libLAS
class LASSRS
{
public:



    LASSRS();
    ~LASSRS();
    LASSRS(const std::vector<LASVLR>& vlrs);
    LASSRS(LASSRS const& other);
    LASSRS& operator=(LASSRS const& rhs);
    
    const GTIF* GetGTIF();
    void SetGTIF(const GTIF* gtiff);
    void ResetVLRs();
    
    std::string GetWKT() const;
    void SetWKT(std::string const& v);
    
    std::string GetProj4() const;
    void SetProj4(std::string const& v);
    
    void SetVLRs(const std::vector<LASVLR>& vlrs);
    std::vector<LASVLR> GetVLRs() const;
    

private:
  
    GTIF* m_gtiff;
    ST_TIFF* m_tiff;
  
    std::vector<LASVLR> m_vlrs;

protected:
    
};



} // namespace liblas

#ifdef HAVE_GDAL
char CPL_DLL *  GTIFGetOGISDefn( GTIF *, GTIFDefn * );
int  CPL_DLL   GTIFSetFromOGISDefn( GTIF *, const char * );

void SetLinearUnitCitation(GTIF* psGTIF, char* pszLinearUOMName);
void SetGeogCSCitation(GTIF * psGTIF, OGRSpatialReference *poSRS, char* angUnitName, int nDatum, short nSpheroid);
#endif

#endif // LIBLAS_LASSRS_HPP_INCLUDED
