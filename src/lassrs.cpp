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

#include <liblas/lassrs.hpp>
#include <liblas/detail/utility.hpp>
#include <iostream>

namespace liblas {

LASSRS::LASSRS() 

{

    m_tiff = NULL;
    m_gtiff = NULL;
}

LASSRS& LASSRS::operator=(LASSRS const& rhs)
{
    if (&rhs != this)
    {
        m_tiff = NULL;
        m_gtiff = NULL;        
        SetVLRs(rhs.GetVLRs());
        GetGTIF();
    }
    return *this;
}

LASSRS::~LASSRS() 

{
    
#ifdef HAVE_LIBGEOTIFF
    if (m_gtiff)
    {
        GTIFFree(m_gtiff);
        m_gtiff = NULL;
    }
    if (m_tiff)
    {
        ST_Destroy(m_tiff);
        m_tiff = NULL;
    }
#endif

}

LASSRS::LASSRS(LASSRS const& other) 
{
    m_tiff = NULL;
    m_gtiff = NULL;
    SetVLRs(other.GetVLRs());
    GetGTIF();
}

LASSRS::LASSRS(const std::vector<LASVLR>& vlrs) 
{
    m_tiff = NULL;
    m_gtiff = NULL;
    SetVLRs(vlrs);
    GetGTIF();
}

/// Keep a copy of the VLRs that are related to GeoTIFF SRS information.
void LASSRS::SetVLRs(const std::vector<LASVLR>& vlrs)
{
    std::vector<LASVLR>::const_iterator i;
    
    std::string const uid("LASF_Projection");
    
    // Wipe out any existing VLRs that might exist on the LASSRS
    m_vlrs.clear();
    
    // We only copy VLR records from the list which are related to GeoTIFF keys.
    // They must have an id of "LASF_Projection" and a record id that's related.
    for (i = vlrs.begin(); i != vlrs.end(); ++i)
    {
        if (IsGeoVLR(*i)) {
            m_vlrs.push_back(*i);
        }
        // //GTIFF_GEOKEYDIRECTORY == 34735
        // if (uid == (*i).GetUserId(true).c_str() && 34735 == (*i).GetRecordId()) {
        //     m_vlrs.push_back(*i);
        // }
        // 
        // // GTIFF_DOUBLEPARAMS == 34736
        // if (uid == (*i).GetUserId(true).c_str() && 34736 == (*i).GetRecordId()) {
        //     m_vlrs.push_back(*i);
        // }
        // 
        // // GTIFF_ASCIIPARAMS == 34737
        // if (uid == (*i).GetUserId(true).c_str() && 34737 == (*i).GetRecordId()) {
        //     m_vlrs.push_back(*i);
        // }
    }
}

void LASSRS::AddVLR(const LASVLR& vlr) 
{
    if (IsGeoVLR(vlr)) {
        m_vlrs.push_back(vlr);
    }
}
bool LASSRS::IsGeoVLR(const LASVLR& vlr) const
{
    std::string const uid("LASF_Projection");
    if (uid == vlr.GetUserId(true).c_str() && 34735 == vlr.GetRecordId()) {
        return true;
    }
    
    // GTIFF_DOUBLEPARAMS == 34736
    if (uid == vlr.GetUserId(true).c_str() && 34736 == vlr.GetRecordId()) {
        return true;
    }
    
    // GTIFF_ASCIIPARAMS == 34737
    if (uid == vlr.GetUserId(true).c_str() && 34737 == vlr.GetRecordId()) {
        return true;
    }
    return false;
}

std::vector<LASVLR> LASSRS::GetVLRs() const
{
    return m_vlrs;
}

void LASSRS::ResetVLRs()
{

    m_vlrs.clear();

#ifndef HAVE_LIBGEOTIFF
    return;
#else

    int ret = 0;
    short* kdata = NULL;
    short kvalue = 0;
    double* ddata = NULL;
    double dvalue = 0;
    uint8_t* adata = NULL;
    uint8_t avalue = NULL;
    int dtype = 0;
    int dcount = 0;
    int ktype = 0;
    int kcount = 0;
    int acount = 0;
    int atype =0;
    
    if (!m_tiff) throw std::invalid_argument("m_tiff was null, cannot reset VLRs without m_tiff");
    if (!m_gtiff) throw std::invalid_argument("m_gtiff was null, cannot reset VLRs without m_gtiff");

    //GTIFF_GEOKEYDIRECTORY == 34735
    ret = ST_GetKey(m_tiff, 34735, &kcount, &ktype, (void**)&kdata);
    if (ret)
    {    
        LASVLR record;
        int i = 0;
        record.SetRecordId(34735);
        record.SetUserId("LASF_Projection");
        std::vector<uint8_t> data;

        // Shorts are 2 bytes in length
        uint16_t length = 2*kcount;

        record.SetRecordLength(length);
        
        // Copy the data into the data vector
        for (i = 0; i < kcount; i++)
        {
            kvalue = kdata[i];
            
            uint8_t* v = reinterpret_cast<uint8_t*>(&kvalue); 
            
            data.push_back(v[0]);
            data.push_back(v[1]);
        }

        record.SetData(data);
        m_vlrs.push_back(record);
    }

    // GTIFF_DOUBLEPARAMS == 34736
    ret = ST_GetKey(m_tiff, 34736, &dcount, &dtype, (void**)&ddata);
    if (ret)
    {    
        LASVLR record;
        int i = 0;
        record.SetRecordId(34736);
        record.SetUserId("LASF_Projection");
        std::vector<uint8_t> data;

        // Doubles are 8 bytes in length
        uint16_t length = 8*dcount;
        record.SetRecordLength(length);
        
        // Copy the data into the data vector
        for (i=0; i<dcount;i++)
        {
            dvalue = ddata[i];
            
            uint8_t* v =  reinterpret_cast<uint8_t*>(&dvalue);
            
            data.push_back(v[0]);
            data.push_back(v[1]);
            data.push_back(v[2]);
            data.push_back(v[3]);
            data.push_back(v[4]);
            data.push_back(v[5]);
            data.push_back(v[6]);
            data.push_back(v[7]);   
        }        
        record.SetData(data);
        m_vlrs.push_back(record);
    }
    
    // GTIFF_ASCIIPARAMS == 34737
    ret = ST_GetKey(m_tiff, 34737, &acount, &atype, (void**)&adata);
    if (ret) 
    {                    
         LASVLR record;
         int i = 0;
         record.SetRecordId(34737);
         record.SetUserId("LASF_Projection");
         std::vector<uint8_t> data;

         // whoa.  If the returned count was 0, it is because there 
         // was a bug in libgeotiff that existed before r1531 where it 
         // didn't calculate the string length for an ASCII geotiff tag.
         // We need to throw an exception in this case because we're
         // screwed, and if we don't we'll end up writing bad GeoTIFF keys.
         if (!acount) {
             throw std::runtime_error("GeoTIFF ASCII key with no returned size. " 
                                      "Upgrade your libgeotiff to a version greater "
                                      "than r1531 (libgeotiff 1.2.6)");
                     }
         uint16_t length = acount;
         record.SetRecordLength(length);
         
         // Copy the data into the data vector
         for (i=0; i<acount;i++) {
             avalue = adata[i];
             
             uint8_t* v =  reinterpret_cast<uint8_t*>(&avalue);
             
             data.push_back(v[0]);
             data.push_back(v[1]);
             data.push_back(v[2]);
             data.push_back(v[3]);
             data.push_back(v[4]);
             data.push_back(v[5]);
             data.push_back(v[6]);
             data.push_back(v[7]);
             
         }
         record.SetData(data);
         m_vlrs.push_back(record);
    }

    
#endif // ndef HAVE_LIBGEOTIFF

}

const GTIF* LASSRS::GetGTIF(){

#ifdef HAVE_LIBGEOTIFF
    // If we already have m_gtiff and m_tiff, that is because we have 
    // already called GetGTIF once before.  VLRs ultimately drive how the 
    // LASSRS is defined, not the GeoTIFF keys.  
    if (m_tiff) {ST_Destroy(m_tiff); m_tiff = NULL;}
    if (m_gtiff) {GTIFFree(m_gtiff); m_gtiff = NULL; }
    
    m_tiff = ST_Create();
    std::string const uid("LASF_Projection");
    
    // Nothing is going to happen here if we don't have any VLRs describing
    // SRS information on the LASSRS.  
    for (uint16_t i = 0; i < m_vlrs.size(); ++i)
    {
        LASVLR record = m_vlrs[i];
        std::vector<uint8_t> data = record.GetData();
        if (uid == record.GetUserId(true).c_str() && 34735 == record.GetRecordId())
        {
            int count = data.size()/sizeof(int16_t);
            ST_SetKey(m_tiff, record.GetRecordId(), count, STT_SHORT, &(data[0]));
        }

        if (uid == record.GetUserId(true).c_str() && 34736 == record.GetRecordId())
        {
            int count = data.size() / sizeof(double);
            ST_SetKey(m_tiff, record.GetRecordId(), count, STT_DOUBLE, &(data[0]));
        }        

        if (uid == record.GetUserId(true).c_str() && 34737 == record.GetRecordId())
        {
            int count = data.size()/sizeof(uint8_t);
            ST_SetKey(m_tiff, record.GetRecordId(), count, STT_ASCII, &(data[0]));
        }
    }

    m_gtiff = GTIFNewSimpleTags(m_tiff);
    if (!m_gtiff) 
        throw std::runtime_error("The geotiff keys could not read from VLR records");
    return m_gtiff;

#endif

    return NULL;
}

/// Fetch the SRS as WKT
std::string LASSRS::GetWKT() const 
{

#ifdef HAVE_GDAL
    GTIFDefn sGTIFDefn;
    char* pszWKT = NULL;
    if (!m_gtiff) return std::string("");
    if( GTIFGetDefn( m_gtiff, &sGTIFDefn ) ) {
        pszWKT = GTIFGetOGISDefn( m_gtiff, &sGTIFDefn );
        if (pszWKT) {
            std::string tmp(pszWKT);
            std::free(pszWKT);
            return tmp;
        }
    }
#endif
    return std::string("");
}

       
void LASSRS::SetWKT(std::string const& v)
{
    if (! m_gtiff ) GetGTIF(); 

#ifdef HAVE_GDAL
    
    int ret = 0;
    ret = GTIFSetFromOGISDefn( m_gtiff, v.c_str() );
    if (!ret) 
        throw std::invalid_argument("could not set m_gtiff from WKT");
    ret = GTIFWriteKeys(m_gtiff);
    if (!ret) 
    {
        throw std::runtime_error("The geotiff keys could not be written");
    }
    ResetVLRs();
#else
    
    throw std::runtime_error("GDAL is not available, LASSRS could not be set from WKT");

#endif

}

std::string LASSRS::GetProj4() const 
{

#ifdef HAVE_GDAL
    
    std::string wkt = GetWKT();
    const char* poWKT = wkt.c_str();
    
    OGRSpatialReference* poSRS = new OGRSpatialReference();
    if( poSRS->importFromWkt((char **) &poWKT) != OGRERR_NONE )
    {
        delete poSRS;
        return std::string("");
    }
    
    char* proj4;
    poSRS->exportToProj4(&proj4);
    std::string tmp(proj4);
    std::free(proj4);
    
    delete poSRS;
    
    return tmp;    

#endif

// if we have libgeotiff but not GDAL, we'll use the 
// simple method in libgeotiff
#ifdef HAVE_LIBGEOTIFF
#ifndef HAVE_GDAL
    GTIFDefn defn;

    if (m_gtiff && GTIFGetDefn(m_gtiff, &defn)) 
    {
        char* proj4def = GTIFGetProj4Defn(&defn);
        std::string tmp(proj4def);
        std::free(proj4def);

        return tmp;
    }
#endif
#endif

// If we have neither GDAL nor proj.4, we can't do squat
    return std::string("");
}

void LASSRS::SetProj4(std::string const& v)
{

    if (! m_gtiff ) GetGTIF(); ResetVLRs();
   
#ifdef HAVE_GDAL

    char* poWKT = NULL;
    const char* poProj4 = v.c_str();
    OGRSpatialReference* poSRS = new OGRSpatialReference();
    if( poSRS->importFromProj4((char *) poProj4) != OGRERR_NONE )
    {
        delete poSRS;
        throw std::invalid_argument("could not import proj4 into OSRSpatialReference SetProj4");
    }
    
    poSRS->exportToWkt(&poWKT);
    delete poSRS;
    
    std::string tmp(poWKT);
    std::free(poWKT);
        
    int ret = 0;
    ret = GTIFSetFromOGISDefn( m_gtiff, tmp.c_str() );
    if (!ret) throw std::invalid_argument("could not set m_gtiff from Proj4");

    ret = GTIFWriteKeys(m_gtiff);
    if (!ret) 
    {
        throw std::runtime_error("The geotiff keys could not be written");
    }

    GTIFDefn defn;

    if (m_gtiff && GTIFGetDefn(m_gtiff, &defn)) 
    {
        char* proj4def = GTIFGetProj4Defn(&defn);
        std::string tmp(proj4def);
        std::free(proj4def);
    }
    
#endif

// if we have libgeotiff but not GDAL, we'll use the 
// simple method in libgeotiff
#ifdef HAVE_LIBGEOTIFF
#ifndef HAVE_GDAL

    int ret = 0;
    ret = GTIFSetFromProj4( m_gtiff, v.c_str());
    if (!ret) 
    {
        throw std::invalid_argument("PROJ.4 string is invalid or unsupported");
    }
    ret = GTIFWriteKeys(m_gtiff);
    if (!ret) 
    {
        throw std::runtime_error("The geotiff keys could not be written");
    }    
#endif
#endif
    ResetVLRs();

    ;
}


} // namespace liblas
