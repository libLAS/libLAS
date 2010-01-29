/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  VLR Reader implementation for C++ libLAS 
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Howard Butler
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
 
#include <liblas/detail/reader/vlr.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/lasvariablerecord.hpp>


namespace liblas { namespace detail { namespace reader {

VLR::VLR(std::istream& ifs, const LASHeader& header) :
    m_ifs(ifs), m_header(header)
{
}

VLR::~VLR()
{

}

std::istream& VLR::GetStream() const
{
    return m_ifs;
}

void VLR::read()
{
    VLRHeader vlrh = { 0 };

    // seek to the start of the VLRs
    m_ifs.seekg(m_header.GetHeaderSize(), std::ios::beg);

    uint32_t count = m_header.GetRecordsCount();
    
    // We set the VLR records count to 0 because AddVLR 
    // will ++ it each time we add a VLR instance to the 
    // header.
    m_header.SetRecordsCount(0);
    for (uint32_t i = 0; i < count; ++i)
    {
        read_n(vlrh, m_ifs, sizeof(VLRHeader));

        uint16_t length = vlrh.recordLengthAfterHeader;
        if (length < 1)
        {
            throw std::domain_error("VLR record length must be at least 1 byte long");
        } 
        std::vector<uint8_t> data;
        data.resize(length);

        read_n(data.front(), m_ifs, length);
         
        LASVariableRecord vlr;
        vlr.SetReserved(vlrh.reserved);
        vlr.SetUserId(std::string(vlrh.userId));
        vlr.SetDescription(std::string(vlrh.description));
        vlr.SetRecordLength(vlrh.recordLengthAfterHeader);
        vlr.SetRecordId(vlrh.recordId);
        vlr.SetData(data);

        m_header.AddVLR(vlr);
    }
}


}}} // namespace liblas::detail::reader
