/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS indexing storage class 
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

#include <liblas/index/storage.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/guid.hpp>
#include <liblas/lasreader.hpp>

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

namespace liblas
{


SpatialIndex::IStorageManager* returnVLRStorageManager(Tools::PropertySet& ps)
{
    SpatialIndex::IStorageManager* sm = new VLRStorageManager(ps);
    return sm;
}

SpatialIndex::IStorageManager* createNewVLRStorageManager()
{
    Tools::PropertySet ps;
    return returnVLRStorageManager(ps);
}

VLRStorageManager::VLRStorageManager(Tools::PropertySet& ps)
{
}

VLRStorageManager::~VLRStorageManager()
{

    // uint32_t vlrsize=0;
    for (std::vector<LASVariableRecord*>::iterator vit = m_vlrbuffer.begin(); vit != m_vlrbuffer.end(); vit++) {
        // vlrsize = vlrsize+(*vit)->GetRecordLength();
        delete *vit;
    }

}

void VLRStorageManager::loadByteArray(const SpatialIndex::id_type id, size_t& len, uint8_t** data)
{
    LASVariableRecord* v;
    try
    {
        v = m_vlrbuffer.at(id);
        if (v == 0) throw Tools::InvalidPageException(id);
    }
    catch (std::out_of_range)
    {
        throw Tools::InvalidPageException(id);
    }

    len = v->GetRecordLength();
    *data = new uint8_t[len];

    memcpy(*data, (uint8_t*)&(v->GetData()[0]), len);
}

void VLRStorageManager::storeByteArray(SpatialIndex::id_type& id, const size_t len, const uint8_t* const data)
{
    
    if (id == SpatialIndex::StorageManager::NewPage)
    {
        LASVariableRecord* v = makeVLR(len,data);

        if (m_emptyPages.empty())
        {
            m_vlrbuffer.push_back(v);
            id = m_vlrbuffer.size() - 1;
        }
        else
        {
            id = m_emptyPages.top(); m_emptyPages.pop();
            m_vlrbuffer[id] = v;
        }
    }
    else
    {
        LASVariableRecord* v_old;
        try
        {
            v_old = m_vlrbuffer.at(id);
            if (v_old == 0) throw Tools::InvalidPageException(id);
        }
        catch (std::out_of_range)
        {
            throw Tools::InvalidPageException(id);
        }

        LASVariableRecord* v = makeVLR(len,data);
        
        delete v_old;
        m_vlrbuffer[id] = v;
    }
}

void VLRStorageManager::deleteByteArray(const SpatialIndex::id_type id)
{
    LASVariableRecord* v;
    try
    {
        v = m_vlrbuffer.at(id);
        if (v == 0) throw Tools::InvalidPageException(id);
    }
    catch (std::out_of_range)
    {
        throw Tools::InvalidPageException(id);
    }

    m_vlrbuffer[id] = 0;
    m_emptyPages.push(id);

    delete v;
}


LASVariableRecord* VLRStorageManager::makeVLR(const size_t len, const uint8_t* data)
{
    LASVariableRecord* v = new LASVariableRecord();
    v->SetRecordLength(len);
    v->SetUserId("liblas.org");
    v->SetRecordId(2112);
    std::vector<uint8_t> d;
    for (size_t i=0;i<len;i++){d.push_back(data[i]);}
    v->SetData(d);
    return v;
}

LASVariableRecord* VLRStorageManager::getVLR() const
{
    return m_vlrbuffer[0];
}

} // namespace liblas