/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS indexing steam class 
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

#include <liblas/index/datastream.hpp>
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




LASIndexDataStream::LASIndexDataStream(LASReader *reader, long dimension) : m_reader(reader), m_pNext(0), m_id(0), m_idxDimension(dimension)
{
    bool read = readPoint();
    if (read) m_id = 0;
}

LASIndexDataStream::~LASIndexDataStream()
{
    if (m_pNext != 0) delete m_pNext;
}

bool LASIndexDataStream::readPoint()
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
    

    SpatialIndex::Region r = SpatialIndex::Region(min, max, m_idxDimension);
    m_pNext = new SpatialIndex::RTree::Data(0, 0, r, m_id);
    
     // std::cout << "Read point " << r <<  "Id: " << m_id << std::endl;
    return true;
}


SpatialIndex::IData* LASIndexDataStream::getNext()
{
    if (m_pNext == 0) return 0;

    SpatialIndex::RTree::Data* ret = m_pNext;
    m_pNext = 0;
    readPoint();
    m_id +=1;
    return ret;
}

bool LASIndexDataStream::hasNext() throw (Tools::NotSupportedException)
{
    // std::cout << "LASIndexDataStream::hasNext called ..." << std::endl;
    return (m_pNext != 0);
}

uint32_t LASIndexDataStream::size() throw (Tools::NotSupportedException)
{
    throw Tools::NotSupportedException("Operation not supported.");
}

void LASIndexDataStream::rewind() throw (Tools::NotSupportedException)
{
    // std::cout << "LASIndexDataStream::rewind called..." << std::endl;

    if (m_pNext != 0)
    {
     delete m_pNext;
     m_pNext = 0;
    }
    
    m_reader->Reset();
    readPoint();
}

} // namespace liblas
