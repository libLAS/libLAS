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

#ifndef LIBLAS_LASINDEX_HPP_INCLUDED
#define LIBLAS_LASINDEX_HPP_INCLUDED

#include <liblas/lasvariablerecord.hpp>
#include <liblas/laspoint.hpp>

#include <spatialindex/SpatialIndex.h>


//std
#include <string>
#include <vector>
#include <stack>
#include <sys/stat.h>

namespace liblas {

class LASDataStream;

class LASIndex
{
public:

    
    LASIndex();
    LASIndex(LASDataStream& strm, std::string& filename);
    LASIndex(std::string& filename);
    /// Copy constructor.
    LASIndex(LASIndex const& other);
    ~LASIndex();
    
    /// Assignment operator.
    LASIndex& operator=(LASIndex const& rhs);
    
    /// Comparison operator.
    bool operator==(const LASIndex& other) const;

    SpatialIndex::ISpatialIndex& index() {return *m_rtree;}
    
    void insert(LASPoint& p, int64_t id);
    std::vector<uint32_t>* intersects(double minx, double miny, double maxx, double maxy, double minz, double maxz);
private:

    SpatialIndex::IStorageManager* m_storage;
    SpatialIndex::StorageManager::IBuffer* m_buffer;
    SpatialIndex::ISpatialIndex* m_rtree;
    std::string m_indexname;
    
    void Init();
};

class LASVisitor : public SpatialIndex::IVisitor
{
public:
    // size_t m_indexIO;
    // size_t m_leafIO;
    
    std::vector<uint32_t>* m_vector;
    

public:
    LASVisitor(std::vector<uint32_t>* vect){m_vector = vect;}

    void visitNode(const SpatialIndex::INode& n)
    {
        //         std::cout << "visitNode" << std::endl;
        // if (n.isLeaf()) m_leafIO++;
        // else m_indexIO++;
    }

    void visitData(const SpatialIndex::IData& d)
    {
        SpatialIndex::IShape* pS;
        d.getShape(&pS);
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

        // std::cout << d.getIdentifier() << std::endl;
        m_vector->push_back(d.getIdentifier());
            // the ID of this data entry is an answer to the query. I will just print it to stdout.
    }

    void visitData(std::vector<const SpatialIndex::IData*>& v)
    {
        std::cout << v[0]->getIdentifier() << " " << v[1]->getIdentifier() << std::endl;
    }
};

class LASDataStream : public SpatialIndex::IDataStream
{
public:
    LASDataStream(LASReader* reader);

    ~LASDataStream()
    {
        if (m_pNext != 0) delete m_pNext;
    };

    SpatialIndex::IData* getNext();
    bool hasNext() throw (Tools::NotSupportedException);

    size_t size() throw (Tools::NotSupportedException);
    void rewind() throw (Tools::NotSupportedException);

    bool readPoint();

protected:
    liblas::LASReader* m_reader;
    SpatialIndex::RTree::Data* m_pNext;
    SpatialIndex::id_type m_id;
};




extern SpatialIndex::IStorageManager* returnLASStorageManager(Tools::PropertySet& in);
extern SpatialIndex::IStorageManager* createNewLASStorageManager();

class LASStorageManager : public SpatialIndex::IStorageManager
{
public:
    LASStorageManager(Tools::PropertySet&);

    virtual ~LASStorageManager();

    virtual void loadByteArray(const SpatialIndex::id_type id, size_t& len, uint8_t** data);
    virtual void storeByteArray(SpatialIndex::id_type& id, const size_t len, const uint8_t* const data);
    virtual void deleteByteArray(const SpatialIndex::id_type id);

private:
    class Entry
    {
    public:
        byte* m_pData;
        size_t m_length;

        Entry(size_t l, const uint8_t* const d) : m_pData(0), m_length(l)
        {
            m_pData = new uint8_t[m_length];
            memcpy(m_pData, d, m_length);
        }

        ~Entry() { delete[] m_pData; }
    }; // Entry

    std::vector<Entry*> m_buffer;
    std::stack<SpatialIndex::id_type> m_emptyPages;
}; // MemoryStorageManager


}

#endif // LIBLAS_LASINDEX_HPP_INCLUDED
