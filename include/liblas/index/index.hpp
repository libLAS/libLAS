/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS indexing class 
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

#ifndef LIBLAS_INDEX_INDEX_HPP_INCLUDED
#define LIBLAS_INDEX_INDEX_HPP_INCLUDED

#include <liblas/laspoint.hpp>


#ifndef _MSC_VER
#include <spatialindex/SpatialIndex.h>
#else
#include <SpatialIndex.h>
#endif

#include <liblas/index/datastream.hpp>
#include <liblas/index/storage.hpp>
#include <liblas/index/visitor.hpp>


//std
#include <string>
#include <vector>
#include <stack>
#include <sys/stat.h>

namespace liblas {

class LASIndex
{
public:

    enum IndexType
    {
        eMemoryIndex = 1, ///< A transient memory index that will go away
        eVLRIndex = 2, ///< An index that will store its data in VLR records
        eExternalIndex = 3 ///< An index that will store its data in files alongside the .las file (.las.dat & .las.idx)
    };
    
    LASIndex();
    LASIndex(std::string& filename);
    /// Copy constructor.
    LASIndex(LASIndex const& other);
    ~LASIndex();
    
    void Initialize(LASIndexDataStream& strm);
    void Initialize();
    
    /// Assignment operator.
    LASIndex& operator=(LASIndex const& rhs);
    
    /// Comparison operator.
    bool operator==(const LASIndex& other) const;

    
    /// Inserts a LASPoint into the index with a given id
    /// \param p the LASPoint to insert
    /// \param id the id to associate with the point
    void insert(LASPoint& p, int64_t id);
    
    /// Intersects the index with the given cube and returns a 
    /// std::vector of ids.  Caller has ownership of the vector.
    /// \param minx minimum X value of the cube
    /// \param maxx maximum X value of the cube
    /// \param miny minimum Y value of the cube
    /// \param maxy maximum Y value of the cube
    /// \param minz minimum Z value of the cube
    /// \param maxz maximum Z value of the cube
    /// \return std::vector<uint32_t> that the caller owns containing the ids that intersect the query.
    std::vector<uint32_t>* intersects(double minx, double miny, double maxx, double maxy, double minz, double maxz);

    /// Sets the page size for the index when stored externally
    /// \param v - page value.  Defaults to 4096.
    void SetPageSize(uint32_t v) { m_Pagesize = v; }

    /// Get index page size for indexes that are stored externally
    /// \return index page size.
    uint32_t GetPageSize() { return m_Pagesize; }
    
    /// Sets the index type
    /// \param v - index type.  Defaults to eExternalIndex.
    void SetType(IndexType v) { m_idxType = v; }

    /// Gets the index type
    /// \return index type.
    IndexType GetType() { return m_idxType; }
    
    LASVariableRecord* GetVLR();
    
private:

    SpatialIndex::IStorageManager* m_storage;
    SpatialIndex::StorageManager::IBuffer* m_buffer;
    SpatialIndex::ISpatialIndex* m_rtree;

    uint32_t m_Pagesize;
    IndexType m_idxType;
    SpatialIndex::id_type m_idxId;
    uint32_t m_idxCapacity;
    uint32_t m_idxLeafCap;
    uint32_t m_idxDimension;   
    double m_idxFillFactor; 
    bool m_idxExists;

    std::string m_idxFilename;

    uint16_t m_bufferCapacity;
    bool m_bufferWriteThrough;
    
    bool m_Initialized;

    
    void Setup();
    SpatialIndex::IStorageManager* CreateStorage(std::string& filename);
    SpatialIndex::StorageManager::IBuffer* CreateIndexBuffer(SpatialIndex::IStorageManager& storage);
    SpatialIndex::ISpatialIndex* CreateIndex(LASIndexDataStream& strm);
    SpatialIndex::ISpatialIndex* LoadIndex();
    bool ExternalIndexExists(std::string& filename);
};



}

#endif // LIBLAS_INDEX_INDEX_HPP_INCLUDED
