/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  laszip helper functions for C++ libLAS 
 * Author:   Michael P. Gerlek (mpg@flaxen.com)
 *
 ******************************************************************************
 * Copyright (c) 2010, Michael P. Gerlek
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

#ifdef HAVE_LASZIP

#include <liblas/liblas.hpp>
#include <liblas/detail/private_utility.hpp>
#include <liblas/detail/zippoint.hpp>
// laszip
#include <laszip/laszip.hpp>
#include <laszip/laszipper.hpp>
// std
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstdlib> // std::size_t
#include <cassert>

namespace liblas { namespace detail { 

static std::string laszip_userid("laszip encoded");
static boost::uint16_t laszip_recordid = 22204;
static std::string laszip_description = "encoded for sequential access";


ZipPoint::ZipPoint(PointFormatName format, const std::vector<VariableRecord>& vlrs)
    : his_vlr_num(0)
    , his_vlr_data(0)
    , our_vlr_num(0)
    , our_vlr_data(0)
    , m_num_items(0)
    , m_items(NULL)
    , m_lz_point(NULL)
    , m_lz_point_data(NULL)
    , m_lz_point_size(0)
{
    ConstructItems(format);

    const VariableRecord* vlr = NULL;
    for (unsigned int i=0; i<vlrs.size(); i++)
    {
        const VariableRecord& p = vlrs[i];
        if (p.GetRecordId() == 22204)
        {
            vlr = &p;
            break;
        }
    }
    if (vlr)
    {
        our_vlr_num = vlr->GetData().size();
        our_vlr_data = new unsigned char[our_vlr_num];
        for (int i=0; i<our_vlr_num; i++)
        {
            our_vlr_data[i] = vlr->GetData()[i];
        }
    }

    return;
}

ZipPoint::~ZipPoint()
{
    m_num_items = 0;
    delete[] m_items;
    m_items = NULL;

    delete[] m_lz_point;
    delete[] m_lz_point_data;

    delete[] our_vlr_data;

    return;
}


void ZipPoint::ConstructItems(PointFormatName format)
{
    switch (format)
    {
    case ePointFormat0:
        m_num_items = 1;
        m_items = new LASitem[1];
        m_items[0].type = LASitem::POINT10;
        m_items[0].size = 20;
        break;

    case ePointFormat1:
        m_num_items = 2;
        m_items = new LASitem[2];
        m_items[0].type = LASitem::POINT10;
        m_items[0].size = 20;
        m_items[1].type = LASitem::GPSTIME11;
        m_items[1].size = 8;
        break;

    case ePointFormat2:
        m_num_items = 2;
        m_items = new LASitem[2];
        m_items[0].type = LASitem::POINT10;
        m_items[0].size = 20;
        m_items[1].type = LASitem::RGB12;
        m_items[1].size = 6;
        break;

    case ePointFormat3:
        m_num_items = 3;
        m_items = new LASitem[3];
        m_items[0].type = LASitem::POINT10;
        m_items[0].size = 20;
        m_items[1].type = LASitem::GPSTIME11;
        m_items[1].size = 8;
        m_items[2].type = LASitem::RGB12;
        m_items[2].size = 6;
        break;

    default:
        throw liblas_error("Bad point format in header");
    }

    // construct the object that will hold a laszip point

    // compute the point size
    m_lz_point_size = 0;
    for (unsigned int i = 0; i < m_num_items; i++)
        m_lz_point_size += m_items[i].size;

    // create the point data
    unsigned int point_offset = 0;
    m_lz_point = new unsigned char*[m_num_items];
    m_lz_point_data = new unsigned char[m_lz_point_size];
    for (unsigned i = 0; i < m_num_items; i++)
    {
        m_lz_point[i] = &(m_lz_point_data[point_offset]);
        point_offset += m_items[i].size;
    }
    
    return;
}


void ZipPoint::ConstructVLR(VariableRecord& v, PointFormatName format) const
{
    boost::uint16_t record_length_after_header = (boost::uint16_t)(34+6*m_num_items);

    // set the header
    v.SetReserved(0xAABB);
    v.SetUserId(laszip_userid);
    v.SetRecordId(laszip_recordid);
    v.SetRecordLength(record_length_after_header);
    v.SetDescription(laszip_description);

    unsigned char pointFormat = 0;
    unsigned short pointSize = 0;
    switch (format)
    {
    case ePointFormat0:
        pointFormat = 0;
        pointSize = ePointSize0;
        break;
    case ePointFormat1:
        pointFormat = 1;
        pointSize = ePointSize1;
        break;
    case ePointFormat2:
        pointFormat = 2;
        pointSize = ePointSize2;
        break;
    case ePointFormat3:
        pointFormat = 3;
        pointSize = ePointSize3;
        break;
    default:
        throw liblas_error("point format not supported by laszip");
    }

    LASzip laszip;
    laszip.setup(pointFormat, pointSize);

    LASzipper zipper;
    
    unsigned char* data;
    int num;
    laszip.pack(data, num);

    // Ick.
    std::vector<boost::uint8_t> vdata;
    for (int i=0; i<num; i++)
    {
        vdata.push_back(data[i]);
    }
        
    v.SetData(vdata);

    v.SetRecordLength((boost::uint16_t)num);
    
    return;
}


bool ZipPoint::ValidateVLR(std::vector<VariableRecord> const& vlrs) const
{
    std::vector<VariableRecord>::const_iterator it;
    for (it = vlrs.begin(); it != vlrs.end(); ++it)
    {
        VariableRecord const& vlr = *it;
        if (IsZipVLR(vlr))
        {
            return ValidateVLR(vlr);
        }
    }

    // if we didn't find one, but we don't care (for now)
    return true;
}

bool ZipPoint::IsZipVLR(const VariableRecord& vlr) const
{
    if (laszip_userid == vlr.GetUserId(true).c_str() && laszip_recordid == vlr.GetRecordId())
    {
        return true;
    }

    return false;
}

bool ZipPoint::ValidateVLR(const VariableRecord& vlr) const
{
    if (laszip_userid != vlr.GetUserId(true).c_str())
        return false;
    
    if (laszip_recordid != vlr.GetRecordId())
        return false;

    // (all validation checking is now done inside the liblas library)

    return true;
}


}} // namespace liblas::detail

#endif // HAVE_LASZIP
