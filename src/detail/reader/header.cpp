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
 
#include <liblas/header.hpp>
#include <liblas/variablerecord.hpp>
#include <liblas/detail/reader/header.hpp>
#include <liblas/detail/private_utility.hpp>
#include <liblas/detail/zippoint.hpp>
// boost
#include <boost/cstdint.hpp>
#include <boost/concept_check.hpp>
#include <boost/cstdint.hpp>

using namespace boost;

namespace liblas { namespace detail { namespace reader {

Header::Header(std::istream& ifs)
    : m_ifs(ifs)
    , m_header(new liblas::Header())
{
}

Header::~Header()
{
}

void Header::ReadHeader()
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
    m_header->SetFileSignature(fsig);

    // 2. File Source ID
    read_n(n2, m_ifs, sizeof(n2));
    m_header->SetFileSourceId(n2);

    // 3. Reserved
    read_n(n2, m_ifs, sizeof(n2));
    m_header->SetReserved(n2);

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
    m_header->SetProjectId(g);

    // 8. Version major
    read_n(n1, m_ifs, sizeof(n1));
    m_header->SetVersionMajor(n1);

    // 9. Version minor
    read_n(n1, m_ifs, sizeof(n1));
    m_header->SetVersionMinor(n1);

    // 10. System ID
    read_n(buf, m_ifs, 32);
    m_header->SetSystemId(buf);

    // 11. Generating Software ID
    read_n(buf, m_ifs, 32);
    m_header->SetSoftwareId(buf);

    // 12. File Creation Day of Year
    read_n(n2, m_ifs, sizeof(n2));
    m_header->SetCreationDOY(n2);

    // 13. File Creation Year
    read_n(n2, m_ifs, sizeof(n2));
    m_header->SetCreationYear(n2);

    // 14. Header Size
    // NOTE: Size of the stanard header block must always be 227 bytes
    read_n(n2, m_ifs, sizeof(n2));
    m_header->SetHeaderSize(n2);

    // 15. Offset to data
    read_n(n4, m_ifs, sizeof(n4));
    
    if (n4 < m_header->GetHeaderSize())
    {
        std::ostringstream msg; 
        msg <<  "The offset to the start of point data, "
            << n4 << ", is smaller than the header size, "
            << m_header->GetHeaderSize() << ".  This is "
            "an invalid condition and incorrectly written "
            "file.  We cannot ignore this error because we "
            "do not know where to begin seeking to read the "
            "file.  Please report whomever's software who "
            "wrote this file to the proper authorities.  They "
            "will be dealt with swiftly and humanely.";
        throw std::runtime_error(msg.str());
    }
    m_header->SetDataOffset(n4);

    // 16. Number of variable length records
    read_n(n4, m_ifs, sizeof(n4));
    m_header->SetRecordsCount(n4);

    // 17. Point Data Format ID
    read_n(n1, m_ifs, sizeof(n1));

    // the high two bits are reserved for laszip compression type
    uint8_t compression_bit_7 = (n1 & 0x80) >> 7;
    uint8_t compression_bit_6 = (n1 & 0x40) >> 6;
    if (!compression_bit_7 && !compression_bit_6)
    {
        m_header->SetCompressed(false);
    }
    else if (compression_bit_7 && !compression_bit_6)
    {
        m_header->SetCompressed(true);
    }
    else if (compression_bit_7 && compression_bit_6)
    {
        throw std::domain_error("This file was compressed with an earlier, experimental version of laszip; please contact 'martin.isenburg@gmail.com' for assistance.");
    }
    else
    {
        assert(!compression_bit_7 && compression_bit_6);
        throw std::domain_error("invalid point compression format");
    }

    // strip the high bits, to determine point type
    n1 &= 0x3f;
    boost::uint8_t n1_copy( n1 );
    if (n1 == liblas::ePointFormat0)
    {
        m_header->SetDataFormatId(liblas::ePointFormat0);
    } 
    else if (n1 == liblas::ePointFormat1)
    {
        m_header->SetDataFormatId(liblas::ePointFormat1);
    }
    else if (n1 == liblas::ePointFormat2)
    {
        m_header->SetDataFormatId(liblas::ePointFormat2);
    }
    else if (n1 == liblas::ePointFormat3)
    {
        m_header->SetDataFormatId(liblas::ePointFormat3);
    }
    else if (n1 == liblas::ePointFormat4)
    {
        m_header->SetDataFormatId(liblas::ePointFormat4);
    }
    else if (n1 == liblas::ePointFormat5)
    {
        m_header->SetDataFormatId(liblas::ePointFormat5);
    }
    else
    {
        throw std::domain_error("invalid point data format");
    }
    
    // 18. Point Data Record Length
    read_n(n2, m_ifs, sizeof(n2));
    // FIXME: We currently only use the DataFormatId, this needs to 
    // adjust the schema based on the difference between the DataRecordLength
    // and the base size of the pointformat.  If we have an XML schema in the 
    // form of a VLR in the file, we'll use that to apportion the liblas::Schema.
    // Otherwise, all bytes after the liblas::Schema::GetBaseByteSize will be 
    // a simple uninterpreted byte field. 
    // m_header->SetDataRecordLength(n2);

    // 19. Number of point records
    read_n(n4, m_ifs, sizeof(n4));
    m_header->SetPointRecordsCount(n4);

    // 20. Number of points by return
    // A few versions of the spec had this as 7, but 
    // https://lidarbb.cr.usgs.gov/index.php?showtopic=11388 says 
    // it is supposed to always be 5
    std::size_t const return_count_length = 5; 
    for (std::size_t i = 0; i < return_count_length; ++i)
    {
        uint32_t count = 0;
        read_n(count, m_ifs, sizeof(uint32_t));
        m_header->SetPointRecordsByReturnCount(i, count);
    }  

    // 21-23. Scale factors
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    m_header->SetScale(x1, y1, z1);

    // 24-26. Offsets
    read_n(x1, m_ifs, sizeof(x1));
    read_n(y1, m_ifs, sizeof(y1));
    read_n(z1, m_ifs, sizeof(z1));
    m_header->SetOffset(x1, y1, z1);

    // 27-28. Max/Min X
    read_n(x1, m_ifs, sizeof(x1));
    read_n(x2, m_ifs, sizeof(x2));

    // 29-30. Max/Min Y
    read_n(y1, m_ifs, sizeof(y1));
    read_n(y2, m_ifs, sizeof(y2));

    // 31-32. Max/Min Z
    read_n(z1, m_ifs, sizeof(z1));
    read_n(z2, m_ifs, sizeof(z2));

    m_header->SetMax(x1, y1, z1);
    m_header->SetMin(x2, y2, z2);

    // only go read VLRs if we have them.
    boost::uint16_t riegl_extra( 0 );
    if (m_header->GetRecordsCount() > 0)
        ReadVLRs( riegl_extra );

    if(0 != riegl_extra) {
        if (n1_copy == liblas::ePointFormat0)
        {
            m_header->SetDataFormatId(liblas::ePointFormat0,riegl_extra);
        }
        else if (n1_copy == liblas::ePointFormat1,riegl_extra)
        {
            m_header->SetDataFormatId(liblas::ePointFormat1,riegl_extra);
        }
        else if (n1_copy == liblas::ePointFormat2)
        {
            m_header->SetDataFormatId(liblas::ePointFormat2,riegl_extra);
        }
        else if (n1_copy == liblas::ePointFormat3)
        {
            m_header->SetDataFormatId(liblas::ePointFormat3,riegl_extra);
        }
        else if (n1_copy == liblas::ePointFormat4)
        {
            m_header->SetDataFormatId(liblas::ePointFormat4,riegl_extra);
        }
        else if (n1_copy == liblas::ePointFormat5)
        {
            m_header->SetDataFormatId(liblas::ePointFormat5,riegl_extra);
        }
        else
        {
            throw std::domain_error("invalid point data format");
        }
    }
    
    boost::uint32_t pad = m_header->GetDataOffset() - (m_header->GetHeaderSize() + m_header->GetVLRBlockSize());
    m_header->SetHeaderPadding(pad);

    // If we're eof, we need to reset the state
    if (m_ifs.eof())
        m_ifs.clear();

    // Seek to the data offset so we can start reading points
    m_ifs.seekg(m_header->GetDataOffset());

}

bool Header::HasLAS10PadSignature() 
{
    uint8_t const sgn1 = 0xCC;
    uint8_t const sgn2 = 0xDD;
    uint8_t pad1 = 0x0; 
    uint8_t pad2 = 0x0;

    std::streamsize const current_pos = m_ifs.tellg();
    
    // If our little test reads off the end of the file (in the case 
    // of a file with just a header and no points), we'll try to put the 
    // borken dishes back up in the cabinet
    try
    {
        detail::read_n(pad1, m_ifs, sizeof(uint8_t));
        detail::read_n(pad2, m_ifs, sizeof(uint8_t));
    }
    catch (std::out_of_range& e) 
    {
        boost::ignore_unused_variable_warning(e);
        m_ifs.seekg(current_pos, std::ios::beg);
        return false;
    }
    catch (std::runtime_error& e)
    {
        boost::ignore_unused_variable_warning(e);
        m_ifs.seekg(current_pos, std::ios::beg);
        return false;        
    }
    LIBLAS_SWAP_BYTES(pad1);
    LIBLAS_SWAP_BYTES(pad2);
    
    // Put the stream back where we found it
    m_ifs.seekg(current_pos, std::ios::beg);
    
    // Let's check both ways in case people were 
    // careless with their swapping.  This will do no good 
    // when we go to read point data though.
    bool found = false;
    if (sgn1 == pad2 && sgn2 == pad1) found = true;
    if (sgn1 == pad1 && sgn2 == pad2) found = true;
    
    return found;
}

void Header::ReadVLRs(boost::uint16_t& riegl_extrabytes)
{
    VLRHeader vlrh = { 0 };

    if (m_ifs.eof()) {
        // if we hit the end of the file already, it's because 
        // we don't have any points.  We still want to read the VLRs 
        // in that case.
        m_ifs.clear();  
    }

    // seek to the start of the VLRs
    m_ifs.seekg(m_header->GetHeaderSize(), std::ios::beg);

    uint32_t count = m_header->GetRecordsCount();
    
    // We set the VLR records count to 0 because AddVLR 
    // will ++ it each time we add a VLR instance to the 
    // header.
    m_header->SetRecordsCount(0);
    for (uint32_t i = 0; i < count; ++i)
    {
        read_n(vlrh, m_ifs, sizeof(VLRHeader));

        uint16_t length = vlrh.recordLengthAfterHeader;

        std::vector<uint8_t> data(length);

        read_n(data.front(), m_ifs, length);
         
        VariableRecord vlr;
        vlr.SetReserved(vlrh.reserved);
        vlr.SetUserId(std::string(vlrh.userId, VariableRecord::eUserIdSize));
        vlr.SetDescription(std::string(vlrh.description, VariableRecord::eDescriptionSize));
        vlr.SetRecordLength(vlrh.recordLengthAfterHeader);
        vlr.SetRecordId(vlrh.recordId);
        vlr.SetData(data);

        m_header->AddVLR(vlr);
        
        if( 0 == strcmp("RIEGL Extra Bytes", vlrh.description) ) {
          riegl_extrabytes = vlrh.recordLengthAfterHeader;
        }        
    }

    liblas::SpatialReference srs(m_header->GetVLRs());    
    m_header->SetSRS(srs);
    
    // Go fetch the schema from the VLRs if we've got one.
    try {
        liblas::Schema schema(m_header->GetVLRs());
        m_header->SetSchema(schema);

    } catch (std::runtime_error const& e) 
    {
        // Create one from the PointFormat if we don't have
        // one in the VLRs.  Create a custom dimension on the schema 
        // That comprises the rest of the bytes after the end of the 
        // required dimensions.
        liblas::Schema schema(m_header->GetDataFormatId());
        
        // FIXME: handle custom bytes here.
        m_header->SetSchema(schema);
        boost::ignore_unused_variable_warning(e);
    }

/***
#ifdef HAVE_LASZIP
    if (m_header->Compressed())
    {
         ZipPoint zpd(m_header->GetDataFormatId());
         bool ok = zpd.ValidateVLR(m_header->GetVLRs());
         if (!ok)
         {
            throw configuration_error("LASzip compression format does not match the LAS header format.");
         }
    }
#endif
***/
}


void Header::Validate()
{
    // Check that the point count actually describes the number of points 
    // in the file.  If it doesn't, we're going to throw an error telling 
    // the user why.  It may also be a problem that the dataoffset is 
    // really what is wrong, but there's no real way to know that unless 
    // you go start mucking around in the bytes with hexdump or od
        
    // LAS 1.3 specification no longer mandates that the end of the file is the
    // end of the points. See http://trac.liblas.org/ticket/147 for more details
    // on this issue and why the seek can be trouble in the windows case.  
    // If you are having trouble properly seeking to the end of the stream on 
    // windows, use boost's iostreams or similar, which do not have an overflow 
    // problem.
    
    if (m_header->GetVersionMinor() < 3 && !m_header->Compressed() ) 
    {
        // Seek to the beginning 
        m_ifs.seekg(0, std::ios::beg);
        std::ios::pos_type beginning = m_ifs.tellg();
    
        // Seek to the end
        m_ifs.seekg(0, std::ios::end);
        std::ios::pos_type end = m_ifs.tellg();
        std::ios::off_type size = end - beginning;
        std::ios::off_type offset = static_cast<std::ios::off_type>(m_header->GetDataOffset());
        std::ios::off_type length = static_cast<std::ios::off_type>(m_header->GetDataRecordLength());
        std::ios::off_type point_bytes = end - offset;

        // Figure out how many points we have and whether or not we have 
        // extra slop in there.
        std::ios::off_type count = point_bytes / length;
        std::ios::off_type remainder = point_bytes % length;
        

        if ( m_header->GetPointRecordsCount() != static_cast<uint32_t>(count)) {
  
                std::ostringstream msg; 
                msg <<  "The number of points in the header that was set "
                        "by the software '" << m_header->GetSoftwareId() <<
                        "' does not match the actual number of points in the file "
                        "as determined by subtracting the data offset (" 
                        <<m_header->GetDataOffset() << ") from the file length (" 
                        << size <<  ") and dividing by the point record length (" 
                        << m_header->GetDataRecordLength() << ")."
                        " It also does not perfectly contain an exact number of"
                        " point data and we cannot infer a point count."
                        " Calculated number of points: " << count << 
                        " Header-specified number of points: " 
                        << m_header->GetPointRecordsCount() <<
                        " Point data remainder: " << remainder;
                throw std::runtime_error(msg.str());                

        }
    }
}
}}} // namespace liblas::detail::reader
