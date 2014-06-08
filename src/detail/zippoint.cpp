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
static uint16_t laszip_recordid = 22204;
static std::string laszip_description = "http://laszip.org";


ZipPoint::ZipPoint(PointFormatName format, const std::vector<VariableRecord>& vlrs)
    : m_lz_point(NULL)
    , m_lz_point_size(0)
{

    boost::scoped_ptr<LASzip> s(new LASzip());
    m_zip.swap(s);

    const VariableRecord* vlr = NULL;
    for (unsigned int i=0; i<vlrs.size(); i++)
    {
        const VariableRecord& p = vlrs[i];
        if (IsZipVLR(p))
        {
            vlr = &p;
            break;
        }
    }

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

    if (vlr)
    {
        bool ok(false);
        ok = m_zip->unpack(&(vlr->GetData()[0]), vlr->GetData().size());
        if (!ok)
        {
            std::ostringstream oss;
            oss << "Error unpacking zip VLR data: " << std::string(m_zip->get_error());
            throw liblas_error(oss.str());
        }

    } else
    {

        if (!m_zip->setup(pointFormat, pointSize))
        {
            std::ostringstream oss;
            oss << "Error setting up LASzip for format " << pointFormat <<": " << m_zip->get_error(); 
            throw liblas_error(oss.str());
        }
    }

    ConstructItems();
    return;
}

ZipPoint::~ZipPoint()
{

    delete[] m_lz_point;

    return;
}


void ZipPoint::ConstructItems()
{


    // construct the object that will hold a laszip point

    // compute the point size
    m_lz_point_size = 0;
    for (unsigned int i = 0; i < m_zip->num_items; i++)
        m_lz_point_size += m_zip->items[i].size;

    // create the point data
    unsigned int point_offset = 0;
    m_lz_point = new unsigned char*[m_zip->num_items];
    
    boost::scoped_array<uint8_t> d( new uint8_t[ m_lz_point_size ] );
    m_lz_point_data.swap(d);
    for (unsigned i = 0; i < m_zip->num_items; i++)
    {
        m_lz_point[i] = &(m_lz_point_data[point_offset]);
        point_offset += m_zip->items[i].size;
    }
    
    assert (point_offset == m_lz_point_size);
    return;
}


void ZipPoint::ConstructVLR(VariableRecord& v) const
{

    unsigned char* data;
    int num;
    m_zip->pack(data, num);

    // Ick.
    std::vector<uint8_t> vdata;
    for (int i=0; i<num; i++)
    {
        vdata.push_back(data[i]);
    }

    // set the header
    v.SetReserved(0xAABB);
    v.SetUserId(laszip_userid);
    v.SetRecordId(laszip_recordid);
    v.SetRecordLength(static_cast<uint16_t>(num));
    v.SetDescription(laszip_description);
    v.SetData(vdata);

    v.SetRecordLength((uint16_t)num);
    
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
