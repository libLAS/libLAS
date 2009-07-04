/******************************************************************************
 * $Id: visitor.hpp 1291 2009-05-28 19:12:14Z hobu $
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS querying class 
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

#ifndef LIBLAS_INDEX_QUERY_HPP_INCLUDED
#define LIBLAS_INDEX_QUERY_HPP_INCLUDED


#ifndef _MSC_VER
#include <spatialindex/SpatialIndex.h>
#else
#include <SpatialIndex.h>
#endif

//std
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <sys/stat.h>

namespace liblas {

class LASQueryResult 
{
private:
    std::list<SpatialIndex::id_type> ids;
    SpatialIndex::Region* bounds;
    uint32_t m_id;
    LASQueryResult();
public:
    LASQueryResult(uint32_t id) : bounds(0), m_id(id){};
    ~LASQueryResult() {if (bounds!=0) delete bounds;};

    /// Copy constructor.
    LASQueryResult(LASQueryResult const& other);

    /// Assignment operator.
    LASQueryResult& operator=(LASQueryResult const& rhs);
        
    std::list<SpatialIndex::id_type> const& GetIDs() const;
    void SetIDs(std::list<SpatialIndex::id_type>& v);
    const SpatialIndex::Region* GetBounds() const;
    void SetBounds(const SpatialIndex::Region*  b);
    uint32_t GetID() const {return m_id;}
    void SetID(uint32_t v) {m_id = v;}
};

class LASQuery : public SpatialIndex::IQueryStrategy
{
private:
    std::queue<SpatialIndex::id_type> m_ids;
    uint32_t m_count;
    std::list<LASQueryResult> m_results;
    bool m_first;
    
public:

    LASQuery();
    ~LASQuery() {
        std::cout << "child count was" << m_count << std::endl;
        std::cout << "results count was" << m_results.size() << std::endl;};
    void getNextEntry(const SpatialIndex::IEntry& entry, SpatialIndex::id_type& nextEntry, bool& hasNext);
    
    std::list<LASQueryResult>& GetResults() {return m_results;}
    SpatialIndex::Region bounds;
};



}

#endif // LIBLAS_INDEX_QUERY_HPP_INCLUDED
