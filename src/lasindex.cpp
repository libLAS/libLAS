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


LASIndex::LASIndex()
{
    m_storage = SpatialIndex::StorageManager::createNewLASStorageManager();
    Init();
}

void LASIndex::Init()
{    
    uint16_t capacity = 10;
    bool writeThrough = false;
    m_buffer = SpatialIndex::StorageManager::createNewRandomEvictionsBuffer(*m_storage, capacity, writeThrough);

    // R-Tree parameters
    double fillFactor = 0.7;
    uint32_t indexCapacity = 100;
    uint32_t leafCapacity = 100;
    uint32_t dimension = 3;
    SpatialIndex::RTree::RTreeVariant variant = SpatialIndex::RTree::RV_RSTAR;

    // create R-tree
    SpatialIndex::id_type indexId=1;
    m_rtree = SpatialIndex::RTree::createNewRTree(*m_buffer, fillFactor, indexCapacity,
                                   leafCapacity, dimension, variant, indexId); 


}

LASIndex::LASIndex(std::string& filename) 
{
    // FIXME: This is not C, no need for struct.
    // FIXME: stat is very weak name! There tons of structs in various C libs (ie. struct stat; in Windows C lib)
    struct stat stats;
    std::ostringstream os;
    os << filename << ".dat";
    std::cout << "index name: " << os.str() << std::endl;

    std::string indexname = os.str();
    int ret = stat(indexname.c_str(),&stats);
    if (!ret) {
        std::cout << "loading existing index " << indexname << std::endl;
        try{
            m_storage = SpatialIndex::StorageManager::loadDiskStorageManager(filename);
        } catch (Tools::IllegalStateException& e) {
            std::string s = e.what();
            std::cout << "error loading index " << s <<std::endl; exit(1);
        }
    }
    else
    {
        std::cout << "Creating new index ... " << std::endl;
        try{
            m_storage = SpatialIndex::StorageManager::createNewDiskStorageManager(filename, 4096);
        } catch (Tools::IllegalStateException& e) {
            std::string s = e.what();
            std::cout << "error creating index" << s <<std::endl; exit(1);
        }
	}

    Init();
}

LASIndex::LASIndex(LASIndex const& other) 
{
    std::cout << "Index copy called" << std::endl;

}

LASIndex::~LASIndex() 
{
    std::cout << "~LASIndex called" << std::endl;
    delete m_rtree;
    delete m_buffer;
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
    
    min[0] = p.GetX(); min[1] = p.GetY(); min[2] = p.GetZ();
    max[0] = p.GetX(); max[1] = p.GetY(); max[2] = p.GetZ();
    try{
        index().insertData(0, 0, SpatialIndex::Region(min, max, 3), id);
    } catch (Tools::IllegalArgumentException& e) {
        std::string s = e.what();
        std::cout << "error inserting index value" << s <<std::endl; exit(1);
    }
}

std::vector<uint32_t>* LASIndex::intersects(double minx, double miny, double maxx, double maxy, double minz, double maxz)
{
    double min[3];
    double max[3];
    
    min[0] = minx; min[1] = miny; min[2] = minz;
    max[0] = maxx; max[1] = maxy; max[2] = maxz;
    
    std::cout.setf(std::ios_base::fixed);
    
    // std::cout << "minx: " << min[0] << " miny: "<<min[1] << " maxx: " <<max[0] << " maxy: " << max[1] << " minz: " << min[2] << " maxz: " << max[2] <<std::endl;
    if (min[0] > max[0] || min[1] > max[1]) {std::cout << "epic fail!" << std::endl;};
    std::vector<uint32_t>* vect = new std::vector<uint32_t>;
    LASVisitor* visitor = new LASVisitor(vect);
    
    const SpatialIndex::Region *region = new SpatialIndex::Region(min, max, 3);
    std::cout << *region << std::endl;
    index().intersectsWithQuery(*region, *visitor);
    
    return vect;
    
}


} // namespace liblas




SpatialIndex::IStorageManager* SpatialIndex::StorageManager::returnLASStorageManager(Tools::PropertySet& ps)
{
	IStorageManager* sm = new LASStorageManager(ps);
	return sm;
}

SpatialIndex::IStorageManager* SpatialIndex::StorageManager::createNewLASStorageManager()
{
	Tools::PropertySet ps;
	return returnLASStorageManager(ps);
}

SpatialIndex::StorageManager::LASStorageManager::LASStorageManager(Tools::PropertySet& ps)
{
}

SpatialIndex::StorageManager::LASStorageManager::~LASStorageManager()
{
	for (std::vector<Entry*>::iterator it = m_buffer.begin(); it != m_buffer.end(); it++) delete *it;
}

void SpatialIndex::StorageManager::LASStorageManager::loadByteArray(const id_type id, size_t& len, uint8_t** data)
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

void SpatialIndex::StorageManager::LASStorageManager::storeByteArray(id_type& id, const size_t len, const uint8_t* const data)
{
	if (id == NewPage)
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

void SpatialIndex::StorageManager::LASStorageManager::deleteByteArray(const id_type id)
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


namespace liblas
{



LASDataStream::LASDataStream(LASReader *reader) : m_reader(reader), m_pNext(0), m_id(0)
{
    bool read = readPoint();
    
    if (read)
        m_id = 0;

    
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
//        throw Tools::IllegalStateException("Unable to read first point for LASReader!");
    
    double x = p->GetX();
    double y = p->GetY();
    double z = p->GetZ();

    min[0] = x; min[1] = y; min[2] = z;
    max[0] = x; max[1] = y; max[2] = z;
    
    m_id = 0;
    SpatialIndex::Region r = SpatialIndex::Region(min, max, 3);
    m_pNext = new SpatialIndex::RTree::Data(0, 0, r, m_id);
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
    return (m_pNext != 0);
}

size_t LASDataStream::size() throw (Tools::NotSupportedException)
{
    throw Tools::NotSupportedException("Operation not supported.");
}

void LASDataStream::rewind() throw (Tools::NotSupportedException)
{

    if (m_pNext != 0)
    {
     delete m_pNext;
     m_pNext = 0;
    }
    
    m_reader->Reset();
    readPoint();
}

} // namespace liblas