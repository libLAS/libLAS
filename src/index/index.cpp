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

#include <liblas/index/index.hpp>

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

void LASIndex::Setup()
{   
    // set default values for member variables
    // actual initialization of the index must happen externally with
    // Initialize.  This allows the changing of the member variables 
    // with the setters/getters.

    m_Pagesize = 4096;
    m_idxType = eExternalIndex;
    m_idxId = 1;
    
    m_idxCapacity = 1000;
    m_idxLeafCap = 1000;
    m_idxDimension = 3;
    
    m_idxFillFactor = 0.7;
    
    m_bufferCapacity = 10;
    m_bufferWriteThrough = false;

    m_idxExists = false;
    
    m_idxFilename = std::string("");
    m_Initialized = false;
}

LASIndex::LASIndex()
{
    std::cout << "Blank Index Constructor called!" << std::endl;
    Setup();
}


LASIndex::LASIndex(std::string& filename) 
{
    Setup();
    m_idxFilename = filename;
}




LASIndex::LASIndex(LASIndex const& other) 
{
    detail::ignore_unused_variable_warning(other);
    std::cout << "Index copy called" << std::endl;
}

LASIndex::~LASIndex() 
{
    std::cout << "~LASIndex called" << std::endl;
    
    if (m_rtree != 0)
        delete m_rtree;
    if (m_buffer != 0)
        delete m_buffer;
    if (m_storage != 0)
        delete m_storage;

}

void LASIndex::Initialize()
{
    m_storage = CreateStorage(m_idxFilename);
    
    m_buffer = CreateIndexBuffer(*m_storage);

    if (m_idxExists == true) {
        std::cout << "loading existing index from file " << std::endl;
        m_rtree = LoadIndex();
    }
    else
    {
        throw std::runtime_error("can't create index with only a filename, must have LASDatastream");
    }
    
    m_Initialized = true;
}



void LASIndex::Initialize(LASIndexDataStream& strm)
{
    m_storage = CreateStorage(m_idxFilename);
    m_buffer = CreateIndexBuffer(*m_storage);

    if (m_idxExists == true) {
        std::cout << "loading existing index from LASIndexDataStream " << std::endl;
        m_rtree = LoadIndex();
    }
    else
    {
        std::cout << "Creating new index from LASIndexDataStream  ... " << std::endl;
        m_rtree = CreateIndex(strm);
    }
    m_Initialized = true;

}

LASVariableRecord* LASIndex::GetVLR()
{
    if (m_idxType == eMemoryIndex) { return static_cast<VLRStorageManager*>(m_storage)->getVLR();}
    else
        return new LASVariableRecord();
}

SpatialIndex::ISpatialIndex* LASIndex::CreateIndex(LASIndexDataStream& strm) 
{
    using namespace SpatialIndex;
    
    ISpatialIndex* index = 0;
    
    try{
        index = RTree::createAndBulkLoadNewRTree(   SpatialIndex::RTree::BLM_STR,
                                                      strm,
                                                      *m_buffer,
                                                      m_idxFillFactor,
                                                      m_idxCapacity,
                                                      m_idxLeafCap,
                                                      m_idxDimension,
                                                      SpatialIndex::RTree::RV_RSTAR,
                                                      m_idxId);
        bool ret = index->isIndexValid();
        if (ret == false) 
            throw std::runtime_error(   "Spatial index error: index is not "
                                        "valid after createAndBulkLoadNewRTree");

        return index;
    } catch (Tools::Exception& e) {
        std::ostringstream os;
        os << "Spatial Index Error: " << e.what();
        throw std::runtime_error(os.str());
    }    
}

SpatialIndex::ISpatialIndex* LASIndex::LoadIndex() 
{
    using namespace SpatialIndex;
    
    ISpatialIndex* index = 0;
    
    try{
        index = RTree::loadRTree(*m_buffer,m_idxId);
        bool ret = index->isIndexValid();
        if (ret == false) 
            throw std::runtime_error(   "Spatial index error: index is not"
                                        " valid after loadRTree");

        return index;
    } catch (Tools::Exception& e) {
        std::ostringstream os;
        os << "Spatial Index Error: " << e.what();
        throw std::runtime_error(os.str());
    }    
}

SpatialIndex::IStorageManager* LASIndex::CreateStorage(std::string& filename)
{
    using namespace SpatialIndex::StorageManager;
    
    SpatialIndex::IStorageManager* storage = 0;
    if (m_idxType == eExternalIndex) {
        std::cout << "index type was eExternalIndex" << std::endl;
        if (ExternalIndexExists(filename) && !filename.empty()) {
            std::cout << "loading existing DiskStorage " << filename << std::endl;
            try{
                storage = loadDiskStorageManager(filename);
                m_idxExists = true;
                return storage;
            } catch (Tools::Exception& e) {
                std::ostringstream os;
                os << "Spatial Index Error: " << e.what();
                throw std::runtime_error(os.str());
            } 
        } else if (!filename.empty()){
            try{
                std::cout << "creating new DiskStorage " << filename << std::endl;            
                storage = createNewDiskStorageManager(filename, m_Pagesize);
                m_idxExists = false;
                return storage;
            } catch (Tools::Exception& e) {
                std::ostringstream os;
                os << "Spatial Index Error: " << e.what();
                throw std::runtime_error(os.str());
            }         
        }
    } else if (m_idxType == eMemoryIndex) {
        std::cout << "index type was eMemoryIndex" << std::endl;

        try{
            std::cout << "creating new createNewVLRStorageManager " << filename << std::endl;            
            storage = createNewVLRStorageManager();
            m_idxExists = false;
            return storage;
        } catch (Tools::Exception& e) {
            std::ostringstream os;
            os << "Spatial Index Error: " << e.what();
            throw std::runtime_error(os.str());
        } 
                    
    }
    return storage;               
}

SpatialIndex::StorageManager::IBuffer* LASIndex::CreateIndexBuffer(SpatialIndex::IStorageManager& storage)
{
    using namespace SpatialIndex::StorageManager;
    IBuffer* buffer = 0;
    try{
        if ( m_storage == 0 ) throw std::runtime_error("Storage was invalid to create index buffer");
        buffer = createNewRandomEvictionsBuffer(storage,
                                                m_bufferCapacity,
                                                m_bufferWriteThrough);
    } catch (Tools::Exception& e) {
        std::ostringstream os;
        os << "Spatial Index Error: " << e.what();
        throw std::runtime_error(os.str());
    }
    return buffer;
}


bool LASIndex::ExternalIndexExists(std::string& filename)
{
    struct stat stats;
    std::ostringstream os;
    os << filename << ".dat";
    
    // if we have already checked, we're done.
    if (m_idxExists == true) return true;

    std::string indexname = os.str();
    
    // ret is -1 for no file existing and 0 for existing
    int ret = stat(indexname.c_str(),&stats);

    bool output = false;
    if (ret == 0) output= true; else output =false;
    return output;
}



LASIndex& LASIndex::operator=(LASIndex const& rhs)
{
    std::cout << "Index assignment called" << std::endl;
    if (&rhs != this)
    {
    }
    return *this;
}

bool LASIndex::operator==(LASIndex const& other) const
{
    if (&other == this) return true;
        
    return true;
}

void LASIndex::insert(LASPoint& p, int64_t id) 
{
    double min[3];
    double max[3];
    
    min[0] = p.GetX(); 
    min[1] = p.GetY(); 
    min[2] = p.GetZ();
    
    max[0] = p.GetX(); 
    max[1] = p.GetY(); 
    max[2] = p.GetZ();
    
    if (m_Initialized == false)
        throw std::runtime_error("Spatial index has not been initialized");
        
    try {
        m_rtree->insertData(0, 0, SpatialIndex::Region(min, max, m_idxDimension), id);
    } catch (Tools::Exception& e) {
        std::ostringstream os;
        os << "Spatial Index Error: " << e.what();
        throw std::runtime_error(os.str());
    }
}

std::vector<uint32_t>* LASIndex::intersects(double minx, double miny, double maxx, double maxy, double minz, double maxz)
{
    double min[3];
    double max[3];
    
    min[0] = minx; 
    min[1] = miny; 
    min[2] = minz;
    
    max[0] = maxx; 
    max[1] = maxy; 
    max[2] = maxz;

    if (m_Initialized == false)
        throw std::runtime_error("Spatial index has not been initialized");
    
    // std::cout.setf(std::ios_base::fixed);
    // std::cout << "Query bounds" << SpatialIndex::Region(min, max, 3) << std::endl;

    std::vector<uint32_t>* vect = new std::vector<uint32_t>;
    LASVisitor* visitor = new LASVisitor(vect);

    SpatialIndex::Region* region = new SpatialIndex::Region(min, max, m_idxDimension);

    try {
        m_rtree->intersectsWithQuery(*region, *visitor);
    } catch (Tools::Exception& e) {
        std::ostringstream os;
        os << "Spatial Index Error: " << e.what();
        delete region;
        delete visitor;
        throw std::runtime_error(os.str());
    }

    delete region;
    delete visitor;
    return vect;
    
}

void LASIndex::Query(LASQuery& query)
{
    m_rtree->queryStrategy(query);
}

} // namespace liblas
