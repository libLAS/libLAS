/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Header Reader implementation for C++ libLAS 
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
 
#include <liblas/detail/reader/header.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/lasvariablerecord.hpp>


namespace liblas { namespace detail { namespace reader {

Header::Header(std::istream& ifs) :
    m_ifs(ifs)
{
}

Header::~Header()
{

}


void Header::read()
{
    using detail::read_n;

    // Helper variables
    uint8_t n1 = 0;
    uint16_t n2 = 0;
    uint32_t n4 = 0;
    double x1 = 0;
    double y1 = 0;
    double z1 = 0;
    double x2 = 0;
    double y2 = 0;
    double z2 = 0;
    std::string buf;
    std::string fsig;

    m_ifs.seekg(0);

    // 1. File Signature
    read_n(fsig, m_ifs, 4);
    m_header.SetFileSignature(fsig);

    // 2. File Source ID
    read_n(n2, m_ifs, sizeof(n2));
    m_header.SetFileSourceId(n2);

    // 3. Reserved
    // This data must always contain Zeros.
    read_n(n2, m_ifs, sizeof(n2));

    // 4-7. Project ID
    uint32_t d1 = 0;
    uint16_t d2 = 0;
    uint16_t d3 = 0;
    uint8_t d4[8] = { 0 };
    read_n(d1, m_ifs, sizeof(d1));
    read_n(d2, m_ifs, sizeof(d2));
    read_n(d3, m_ifs, sizeof(d3));
    read_n(d4, m_ifs, sizeof(d4));
    liblas::guid g(d1, d2, d3, d4);
    m_header.SetProjectId(g);

    // 8. Version major
    read_n(n1, m_ifs, sizeof(n1));
    m_header.SetVersionMajor(n1);

    // 9. Version minor
    read_n(n1, m_ifs, sizeof(n1));
    m_header.SetVersionMinor(n1);

    // 10. System ID
    read_n(buf, m_ifs, 32);
    m_header.SetSystemId(buf);

    // 11. Generating Software ID
    read_n(buf, m_ifs, 32);
    m_header.SetSoftwareId(buf);

    // 12. File Creation Day of Year
    read_n(n2, m_ifs, sizeof(n2));
    m_header.SetCreationDOY(n2);

    // 13. File Creation Year
    read_n(n2, m_ifs, sizeof(n2));
    m_header.SetCreationYear(n2);

    // 14. Header Size
    // NOTE: Size of the stanard header block must always be 227 bytes
    read_n(n2, m_ifs, sizeof(n2));

    // 15. Offset to data
    read_n(n4, m_ifs, sizeof(n4));
    if (n4 < m_header.GetHeaderSize())
    {
        // TODO: Move this test to LASHeader::Validate()
        throw std::domain_error("offset to point data smaller than header size");
    }
    m_header.SetDataOffset(n4);

    // 16. Number of variable length records
    read_n(n4, m_ifs, sizeof(n4));
    m_header.SetRecordsCount(n4);

    // 17. Point Data Format ID
    read_n(n1, m_ifs, sizeof(n1));
    if (n1 == liblas::ePointFormat0)
    {
        m_header.SetDataFormatId(liblas::ePointFormat0);
    } 
    else if (n1 == liblas::ePointFormat1)
    {
        m_header.SetDataFormatId(liblas::ePointFormat1);
    }
    else if (n1 == liblas::ePointFormat2)
    {
        m_header.SetDataFormatId(liblas::ePointFormat2);
    }
    else if (n1 == liblas::ePointFormat3)
    {
        m_header.SetDataFormatId(liblas::ePointFormat3);
    }
    else
    {
        throw std::domain_error("invalid point data format");
    }
    
    // 18. Point Data Record Length
    read_n(n2, m_ifs, sizeof(n2));
    m_header.SetDataRecordLength(n2);

    // 19. Number of point records
    read_n(n4, m_ifs, sizeof(n4));
    m_header.SetPointRecordsCount(n4);

    // 20. Number of points by return
    std::vector<uint32_t>::size_type const srbyr = 5;
    uint32_t rbyr[srbyr] = { 0 };
    read_n(rbyr, m_ifs, sizeof(rbyr));
    for (std::size_t i = 0; i < srbyr; ++i)
    {
        m_header.SetPointRecordsByReturnCount(i, rbyr[i]);
    }

    // 21-23. Scale factors
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    m_header.SetScale(x1, y1, z1);

    // 24-26. Offsets
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    m_header.SetOffset(x1, y1, z1);

    // 27-28. Max/Min X
    read_n(x1, m_ifs, sizeof(x1));
    read_n(x2, m_ifs, sizeof(x2));

    // 29-30. Max/Min Y
    read_n(y1, m_ifs, sizeof(y1));
    read_n(y2, m_ifs, sizeof(y2));

    // 31-32. Max/Min Z
    read_n(z1, m_ifs, sizeof(z1));
    read_n(z2, m_ifs, sizeof(z2));

    m_header.SetMax(x1, y1, z1);
    m_header.SetMin(x2, y2, z2);

    // We're going to check the two bytes off the end of the header to 
    // see if they're pad bytes anyway.  Some softwares, notably older QTModeler, 
    // write 1.0-style pad bytes off the end of their files but state that the
    // offset is actually 2 bytes back.  We need to set the dataoffset 
    // appropriately in those cases anyway. 
    m_ifs.seekg(m_header.GetDataOffset());
    bool has_pad = HasLAS10PadSignature();
    if (has_pad) {
        std::streamsize const current_pos = m_ifs.tellg();
        m_ifs.seekg(current_pos + 2);
        m_header.SetDataOffset(m_header.GetDataOffset() + 2);
    }
    
    readvlrs();
}

bool Header::HasLAS10PadSignature() 
{
    uint8_t const sgn1 = 0xCC;
    uint8_t const sgn2 = 0xDD;
    uint8_t pad1 = 0x0; 
    uint8_t pad2 = 0x0;

    std::streamsize const current_pos = m_ifs.tellg();
    
    // If our little test reads off the end, we'll try to put the 
    // borken dishes back up in the cabinet
    try
    {
        detail::read_n(pad1, m_ifs, sizeof(uint8_t));
        detail::read_n(pad2, m_ifs, sizeof(uint8_t));
    }
    catch (std::out_of_range& e) 
    {
        ignore_unused_variable_warning(e);
        m_ifs.seekg(current_pos, std::ios::beg);
        return false;
    }
    catch (std::runtime_error& e)
    {
        ignore_unused_variable_warning(e);
        m_ifs.seekg(current_pos, std::ios::beg);
        return false;        
    }
    LIBLAS_SWAP_BYTES(pad1);
    LIBLAS_SWAP_BYTES(pad2);
    
    // Put the stream back where we found it
    m_ifs.seekg(current_pos, std::ios::beg);

    // FIXME: we have to worry about swapping issues
    // but some people write the pad bytes backwards 
    // anyway.  Let's check both ways.
    bool found = false;
    if (sgn1 == pad2 && sgn2 == pad1) found = true;
    if (sgn1 == pad1 && sgn2 == pad2) found = true;
    
    return found;
}

void Header::readvlrs()
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
         
        VariableRecord vlr;
        vlr.SetReserved(vlrh.reserved);
        vlr.SetUserId(std::string(vlrh.userId));
        vlr.SetDescription(std::string(vlrh.description));
        vlr.SetRecordLength(vlrh.recordLengthAfterHeader);
        vlr.SetRecordId(vlrh.recordId);
        vlr.SetData(data);

        m_header.AddVLR(vlr);
    }

    liblas::SpatialReference srs(m_header.GetVLRs());    
    m_header.SetSRS(srs);

}

}}} // namespace liblas::detail::reader
