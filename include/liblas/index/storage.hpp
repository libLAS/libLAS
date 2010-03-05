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

#ifndef LIBLAS_INDEX_STORAGE_HPP_INCLUDED
#define LIBLAS_INDEX_STORAGE_HPP_INCLUDED

#include <liblas/lasvariablerecord.hpp>


#ifndef _MSC_VER
#include <spatialindex/SpatialIndex.h>
#else
#include <SpatialIndex.h>
#endif

//std
#include <string>
#include <vector>
#include <stack>
#include <sys/stat.h>

namespace liblas {


extern SpatialIndex::IStorageManager* returnVLRStorageManager(Tools::PropertySet& in);
extern SpatialIndex::IStorageManager* createNewVLRStorageManager();

class VLRStorageManager : public SpatialIndex::IStorageManager
{
public:
    VLRStorageManager(Tools::PropertySet&);

    virtual ~VLRStorageManager();

    virtual void loadByteArray(const SpatialIndex::id_type id, ::uint32_t& len, ::uint8_t** data);
    virtual void storeByteArray(SpatialIndex::id_type& id, const ::uint32_t len, const ::uint8_t* const data);
    virtual void deleteByteArray(const SpatialIndex::id_type id);
    
    liblas::VariableRecord* getVLR() const;

private:
    
    liblas::VariableRecord m_data;
    liblas::VariableRecord m_ids;

    // class Entry
    // {
    // public:
    //     byte* m_pData;
    //     size_t m_length;
    // 
    //     Entry(size_t l, const uint8_t* const d) : m_pData(0), m_length(l)
    //     {
    //         m_pData = new uint8_t[m_length];
    //         memcpy(m_pData, d, m_length);
    //     }
    // 
    //     ~Entry() { delete[] m_pData; }
    // }; // Entry
    
    typedef std::vector<liblas::VariableRecord*> vlrbuffer_t;
    vlrbuffer_t m_vlrbuffer;
    std::stack<SpatialIndex::id_type> m_emptyPages;

    liblas::VariableRecord* makeVLR(const std::size_t len, const ::uint8_t* data);

}; // MemoryStorageManager


}

#endif // LIBLAS_INDEX_STORAGE_HPP_INCLUDED
