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
#include <liblas/capi/las_config.h>

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

// Fake out the compiler if we don't have libgeotiff
#ifndef HAVE_LIBGEOTIFF
    typedef struct GTIFS * GTIF;
    typedef struct ST_TIFFS * ST_TIFF;
#endif

namespace liblas {

/// Spatial Reference System container for libLAS
class LASSRS
{
public:



    /// Default constructor.
    LASSRS();

    /// Destructor.
    /// If libgeotiff is enabled, deallocates libtiff and libgeotiff objects used internally.
    ~LASSRS();

    /// Constructor creating LASSRS instance from given Variable-Length Record.
    LASSRS(const std::vector<LASVLR>& vlrs);

    /// Copy constryctor.
    LASSRS(LASSRS const& other);

    /// Assignment operator.
    LASSRS& operator=(LASSRS const& rhs);
    

    /// Returns a pointer to the internal GTIF*.  Only available if 
    /// you have libgeotiff linked in.
    const GTIF* GetGTIF();
    
    void SetGTIF(const GTIF* gtiff, const TIFF* tiff);

    /// Returns the OGC WKT describing Spatial Reference System.
    /// If GDAL is linked, it uses GDAL's operations and methods to determine 
    /// the WKT.  If GDAL is not linked, no WKT is returned.
    std::string GetWKT() const;
    
    /// Sets the SRS using GDAL's OGC WKT. If GDAL is not linked, this 
    /// operation has no effect.
    /// \param v - a string containing the WKT string.  
    void SetWKT(std::string const& v);
    
    /// Returns the Proj.4 string describing the Spatial Reference System.
    /// If GDAL is linked, it uses GDAL's operations and methods to determine 
    /// the Proj.4 string -- otherwise, if libgeotiff is linked, it uses 
    /// that.  Note that GDAL's operations are much more mature and 
    /// support more coordinate systems and descriptions.
    std::string GetProj4() const;

    /// Sets the Proj.4 string describing the Spatial Reference System.
    /// If GDAL is linked, it uses GDAL's operations and methods to determine 
    /// the Proj.4 string -- otherwise, if libgeotiff is linked, it uses 
    /// that.  Note that GDAL's operations are much more mature and 
    /// support more coordinate systems and descriptions.
    /// \param v - a string containing the Proj.4 string.
    void SetProj4(std::string const& v);
    
    /// Set the LASVLRs for the LASSRS.  SetVLRs will only copy 
    /// VLR records that pertain to the GeoTIFF keys, and extraneous 
    /// VLR records will not be copied.
    /// \param vlrs - A list of VLRs that contains VLRs describing GeoTIFF keys
    void SetVLRs(const std::vector<LASVLR>& vlrs);
    
    /// Add a VLR representing GeoTIFF keys to the SRS
    void AddVLR(const LASVLR& vlr);
    
    /// Return a copy of the LASVLRs that LASSRS maintains
    std::vector<LASVLR> GetVLRs() const;
    

private:

    GTIF* m_gtiff;
    ST_TIFF* m_tiff;

    std::vector<LASVLR> m_vlrs;
    bool IsGeoVLR(const LASVLR& vlr) const;

    /// Reset the VLRs of the LASSRS using the existing GTIF* and ST_TIF*
    /// Until this method is called, 
    /// the LASSRS will only contain a SRS description using the VLRs 
    /// that it was first instantiated with.  SetWKT and SetProj4 can 
    /// be used to change the GTIF* 
    void ResetVLRs();
};

} // namespace liblas

#ifdef HAVE_GDAL
LAS_C_START
char LAS_DLL * GTIFGetOGISDefn(GTIF*, GTIFDefn*);
int  LAS_DLL   GTIFSetFromOGISDefn(GTIF*, const char*);

void SetLinearUnitCitation(GTIF* psGTIF, char* pszLinearUOMName);
void SetGeogCSCitation(GTIF* psGTIF, OGRSpatialReference* poSRS, char* angUnitName, int nDatum, short nSpheroid);
LAS_C_END
#endif

#endif // LIBLAS_LASSRS_HPP_INCLUDED
