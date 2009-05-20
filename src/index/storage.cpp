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


SpatialIndex::IStorageManager* returnLASStorageManager(Tools::PropertySet& ps)
{
    SpatialIndex::IStorageManager* sm = new LASStorageManager(ps);
    return sm;
}

SpatialIndex::IStorageManager* createNewLASStorageManager()
{
    Tools::PropertySet ps;
    return returnLASStorageManager(ps);
}

LASStorageManager::LASStorageManager(Tools::PropertySet& ps)
{
}

LASStorageManager::~LASStorageManager()
{
    for (std::vector<Entry*>::iterator it = m_buffer.begin(); it != m_buffer.end(); it++) delete *it;
}

void LASStorageManager::loadByteArray(const SpatialIndex::id_type id, size_t& len, uint8_t** data)
{
    Entry* e;
    try
    {
        e = m_buffer.at(id);
        if (e == 0) throw Tools::InvalidPageException(id);
    }
    catch (std::out_of_range)
    {
        throw Tools::InvalidPageException(id);
    }

    len = e->m_length;
    *data = new uint8_t[len];

    memcpy(*data, e->m_pData, len);
}

void LASStorageManager::storeByteArray(SpatialIndex::id_type& id, const size_t len, const uint8_t* const data)
{
    if (id == SpatialIndex::StorageManager::NewPage)
    {
        Entry* e = new Entry(len, data);

        if (m_emptyPages.empty())
        {
            m_buffer.push_back(e);
            id = m_buffer.size() - 1;
        }
        else
        {
            id = m_emptyPages.top(); m_emptyPages.pop();
            m_buffer[id] = e;
        }
    }
    else
    {
        Entry* e_old;
        try
        {
            e_old = m_buffer.at(id);
            if (e_old == 0) throw Tools::InvalidPageException(id);
        }
        catch (std::out_of_range)
        {
            throw Tools::InvalidPageException(id);
        }

        Entry* e = new Entry(len, data);

        delete e_old;
        m_buffer[id] = e;
    }
}

void LASStorageManager::deleteByteArray(const SpatialIndex::id_type id)
{
    Entry* e;
    try
    {
        e = m_buffer.at(id);
        if (e == 0) throw Tools::InvalidPageException(id);
    }
    catch (std::out_of_range)
    {
        throw Tools::InvalidPageException(id);
    }

    m_buffer[id] = 0;
    m_emptyPages.push(id);

    delete e;
}





} // namespace liblas