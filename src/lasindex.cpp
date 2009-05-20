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

#include <liblas/lasindex.hpp>
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

void LASIndex::Init()
{   

    m_Pagesize = 4096;
    m_idxType = eExternalIndex;
    m_idxId = 1;
    
    m_idxCapacity = 100;
    m_idxLeafCap = 100;
    m_idxDimension = 3;
    
    m_idxFillFactor = 0.7;
    
    m_bufferCapacity = 10;
    m_bufferWriteThrough = false;

    m_idxExists = false;
}

LASIndex::LASIndex()
{
    std::cout << "Blank Index Constructor called!" << std::endl;
    Init();
}

SpatialIndex::ISpatialIndex* LASIndex::CreateIndex(LASDataStream& strm) 
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
            throw std::runtime_error(   "Spatial index error: index is not"
                                        " valid after createAndBulkLoadNewRTree");

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
LASIndex::LASIndex(LASDataStream& strm, std::string& filename)
{
    using namespace SpatialIndex;
    
    Init();
    
    m_storage = CreateStorage(filename);
    m_buffer = CreateIndexBuffer(*m_storage);

    if (m_idxExists == true) {
        std::cout << "loading existing index from LASDataStream " << std::endl;
        m_rtree = LoadIndex();
    }
    else
    {
        std::cout << "Creating new index from LASDataStream  ... " << std::endl;
        m_rtree = CreateIndex(strm);
    }        
}


SpatialIndex::IStorageManager* LASIndex::CreateStorage(std::string& filename)
{
    using namespace SpatialIndex::StorageManager;
    
    SpatialIndex::IStorageManager* storage = 0;
    if (m_idxType == eExternalIndex) {

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

        try{
            std::cout << "creating new DiskStorage " << filename << std::endl;            
            storage = createNewMemoryStorageManager();
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

LASIndex::LASIndex(std::string& filename) 
{

    Init();
    
    m_storage = CreateStorage(filename);
    
    m_buffer = CreateIndexBuffer(*m_storage);

    if (m_idxExists == true) {
        std::cout << "loading existing index from file " << std::endl;
        m_rtree = LoadIndex();
    }
    else
    {
        throw std::runtime_error("can't create index with only a filename, must have LASDatastream");
    }   
    
}

LASIndex::LASIndex(LASIndex const& other) 
{
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
    
    try{
        m_rtree->insertData(0, 0, SpatialIndex::Region(min, max, 3), id);
    } catch (Tools::Exception& e) {
        std::string s = e.what();
        std::cout << "error inserting index value" << s <<std::endl; exit(1);
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
    
    std::cout.setf(std::ios_base::fixed);
    
    // std::cout << "minx: " << min[0] << " miny: "<<min[1] << " maxx: " <<max[0] << " maxy: " << max[1] << " minz: " << min[2] << " maxz: " << max[2] <<std::endl;
    if (min[0] > max[0] || min[1] > max[1]) {std::cout << "epic fail!" << std::endl;};
    std::vector<uint32_t>* vect = new std::vector<uint32_t>;
    LASVisitor* visitor = new LASVisitor(vect);
    
    const SpatialIndex::Region *region = new SpatialIndex::Region(min, max, 3);
    // std::cout << *region << std::endl;
    m_rtree->intersectsWithQuery(*region, *visitor);
    // std::cout << index() <<std::endl;
    return vect;
    
}





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




LASDataStream::LASDataStream(LASReader *reader) : m_reader(reader), m_pNext(0), m_id(0)
{
    bool read = readPoint();
    
    if (read)
        m_id = 0;

    std::cout<<"Initiating LASDataStream... read=" << read <<std::endl;
    
}

bool LASDataStream::readPoint()
{
    double min[3], max[3];
    
    bool doRead = m_reader->ReadNextPoint();
    LASPoint* p;
    if (doRead)
        p = (LASPoint*) &(m_reader->GetPoint());
    else
        return false;
    
    double x = p->GetX();
    double y = p->GetY();
    double z = p->GetZ();

    min[0] = x; min[1] = y; min[2] = z;
    max[0] = x; max[1] = y; max[2] = z;
    

    SpatialIndex::Region r = SpatialIndex::Region(min, max, 3);
    m_pNext = new SpatialIndex::RTree::Data(0, 0, r, m_id);
    
     // std::cout << "Read point " << r <<  "Id: " << m_id << std::endl;
    return true;
}


SpatialIndex::IData* LASDataStream::getNext()
{
    if (m_pNext == 0) return 0;

    SpatialIndex::RTree::Data* ret = m_pNext;
    m_pNext = 0;
    readPoint();
    m_id +=1;
    return ret;
}

bool LASDataStream::hasNext() throw (Tools::NotSupportedException)
{
    // std::cout << "LASDataStream::hasNext called ..." << std::endl;
    return (m_pNext != 0);
}

size_t LASDataStream::size() throw (Tools::NotSupportedException)
{
    throw Tools::NotSupportedException("Operation not supported.");
}

void LASDataStream::rewind() throw (Tools::NotSupportedException)
{
    // std::cout << "LASDataStream::rewind called..." << std::endl;

    if (m_pNext != 0)
    {
     delete m_pNext;
     m_pNext = 0;
    }
    
    m_reader->Reset();
    readPoint();
}

LASVisitor::LASVisitor(std::vector<uint32_t>* vect){m_vector = vect;}

void LASVisitor::visitNode(const SpatialIndex::INode& n)
{
            std::cout << "visitNode" << std::endl;
    if (n.isLeaf()) m_leafIO++;
    else m_indexIO++;
}

void LASVisitor::visitData(const SpatialIndex::IData& d)
{
    SpatialIndex::IShape* pS;
    d.getShape(&pS);
    SpatialIndex::Region *r = new SpatialIndex::Region();
    pS->getMBR(*r);
    std::cout <<"found shape: " << *r << " dimension: " <<pS->getDimension() << std::endl;
        // do something.
    delete pS;

    // data should be an array of characters representing a Region as a string.
    uint8_t* pData = 0;
    size_t cLen = 0;
    d.getData(cLen, &pData);
    // do something.
    //string s = reinterpret_cast<char*>(pData);
    //cout << s << endl;
    delete[] pData;

     // std::cout << "found: " <<d.getIdentifier() << std::endl;
    m_vector->push_back(d.getIdentifier());
        // the ID of this data entry is an answer to the query. I will just print it to stdout.
}

void LASVisitor::visitData(std::vector<const SpatialIndex::IData*>& v)
{
    std::cout << v[0]->getIdentifier() << " " << v[1]->getIdentifier() << std::endl;
}

} // namespace liblas