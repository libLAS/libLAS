/******************************************************************************
 * $Id: storage.cpp 1298 2009-06-17 16:18:57Z hobu $
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

#include <liblas/index/query.hpp>
#include <liblas/cstdint.hpp>

#include <cstddef>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

namespace liblas
{

LASQuery::LASQuery() : m_count(0), m_first(true) {}

void LASQuery::getNextEntry(const SpatialIndex::IEntry& entry, SpatialIndex::id_type& nextEntry, bool& hasNext)
{
    // the first time we are called, entry points to the root.
    if (m_first)
    {
        SpatialIndex::IShape* ps = 0; // FIXME: No RAII, use smart pointer
        entry.getShape(&ps);
        ps->getMBR(bounds);
        delete ps;
        
        // any other time through we're no longer at the root so 
        // we wouldn't get the entire index's bounding box
        m_first = false;
    }
    const SpatialIndex::INode* n = dynamic_cast<const SpatialIndex::INode*>(&entry);
    assert(0 != n);

    // traverse only index nodes at levels 2 and higher.
    if (n != 0 && n->getLevel() > 0)
    {
        m_count +=n->getChildrenCount();
        for (std::size_t cChild = 0; cChild < n->getChildrenCount(); cChild++)
        {
            m_ids.push(n->getChildIdentifier(cChild));
        }
    }

    if (n->isLeaf())
    {
        SpatialIndex::IShape* ps = 0; // FIXME: Use RAII
        entry.getShape(&ps);
        SpatialIndex::Region* pr = dynamic_cast<SpatialIndex::Region*>(ps);
        assert(0 != pr);
        std::list<SpatialIndex::id_type> ids;
        for (std::size_t cChild = 0; cChild < n->getChildrenCount(); cChild++)
        {
            ids.push_back(n->getChildIdentifier(cChild));
        }
        LASQueryResult result(static_cast<uint32_t>(n->getIdentifier()));
        result.SetIDs(ids);
        result.SetBounds(pr);
        m_results.push_back(result);
    }
		    
    if (! m_ids.empty())
    {
        nextEntry = m_ids.front(); m_ids.pop();
        hasNext = true;
    }
    else
    {
        hasNext = false;
    }    
    
    // // stop after the root.
    // hasNext = false;
    // 
    // SpatialIndex::IShape* ps;
    // entry.getShape(&ps);
    //     // ps->getMBR(m_indexedSpace);
    // delete ps;
}


std::list<SpatialIndex::id_type> const& LASQueryResult::GetIDs() const
{
    return ids;
}

void LASQueryResult::SetIDs(std::list<SpatialIndex::id_type>& v) 
{
    ids.resize(v.size());
    std::copy(v.begin(), v.end(), ids.begin());
}
const SpatialIndex::Region*  LASQueryResult::GetBounds() const
{
    return bounds;
}

void LASQueryResult::SetBounds(const SpatialIndex::Region*  b) 
{
    assert(0 != b);
    bounds = new SpatialIndex::Region(*b);
}

LASQueryResult::LASQueryResult(LASQueryResult const& other)
{
    ids.resize(other.ids.size());
    std::copy(other.ids.begin(), other.ids.end(), ids.begin());
    m_id = other.m_id;
    
    bounds = other.bounds->clone();
}

LASQueryResult& LASQueryResult::operator=(LASQueryResult const& rhs)
{
    if (&rhs != this)
    {
        ids.resize(rhs.ids.size());
        std::copy(rhs.ids.begin(), rhs.ids.end(), ids.begin());
        m_id = rhs.m_id;
        bounds = rhs.bounds->clone();
    }
    return *this;
}


LASQuery::LASQuery(std::istream& input) : m_count(0), m_first(true) 
{
    long id_count = 0;
    long id = 0;
    long i = 0;

    
    double low[2];
    double high[2];
    
    double mins[2];
    double maxs[2];
    
    bool first = true;
    
    while(input) {
        input >> id >> id_count >> low[0] >> low[1] >> high[0] >> high[1];
        // printf("count:%d %.2f %.2f %.2f %.2f\n", id_count, low[0], low[1], high[0], high[1]);
        
        if (first) {
            mins[0] = low[0];
            mins[1] = low[1];
            maxs[0] = high[0];
            maxs[1] = high[1];
            first = false;
        }
        
        mins[0] = std::min(mins[0], low[0]);
        mins[1] = std::min(mins[1], low[1]);
        
        maxs[0] = std::max(maxs[0], high[0]);
        maxs[1] = std::max(maxs[1], high[1]);
        // if (!input.good()) continue;
        
        std::list<SpatialIndex::id_type> ids;
        for (int j=0; j<id_count; j++) {
            input >> i;
            ids.push_back(i);
        }
        SpatialIndex::Region* pr = new SpatialIndex::Region(low, high, 2);
        // printf("Ids size: %d %.3f\n", ids.size(), pr->getLow(0));
        LASQueryResult result(static_cast<uint32_t>(id));
        result.SetIDs(ids);
        result.SetBounds(pr);
        m_results.push_back(result);
        delete pr;
    }

    bounds = SpatialIndex::Region(mins, maxs, 2);

}
} // namespace liblas
