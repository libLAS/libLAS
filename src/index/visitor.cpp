/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS indexing visitor class 
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

LASVisitor::LASVisitor(std::vector<uint32_t>* vect) : m_vector(vect)
{
    assert(0 != vect);
}

void LASVisitor::visitNode(const SpatialIndex::INode& n)
{
            std::cout << "visitNode" << std::endl;
    if (n.isLeaf()) m_leafIO++;
    else m_indexIO++;
}

void LASVisitor::visitData(const SpatialIndex::IData& d)
{
    SpatialIndex::IShape* pS = 0;
    d.getShape(&pS);
    assert(0 != pS);

    SpatialIndex::Region *r = new SpatialIndex::Region();
    assert(0 != r);
    pS->getMBR(*r);
    std::cout <<"found shape: " << *r << " dimension: " <<pS->getDimension() << std::endl;
        // do something.
    delete pS;
    delete r;

    // data should be an array of characters representing a Region as a string.
    uint8_t* pData = 0;
    uint32_t cLen = 0;
    d.getData(cLen, &pData);
    // do something.
    //string s = reinterpret_cast<char*>(pData);
    //cout << s << endl;
    delete[] pData;

    m_vector->push_back(static_cast<uint32_t>(d.getIdentifier()));
    // the ID of this data entry is an answer to the query. I will just print it to stdout.
}

void LASVisitor::visitData(std::vector<const SpatialIndex::IData*>& v)
{
    std::cout << v[0]->getIdentifier() << " " << v[1]->getIdentifier() << std::endl;
}

} // namespace liblas
