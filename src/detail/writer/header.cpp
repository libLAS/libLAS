/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS header writer implementation for C++ libLAS 
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
#include <liblas/point.hpp>
#include <liblas/spatialreference.hpp>
#include <liblas/detail/writer/header.hpp>
#include <liblas/detail/private_utility.hpp>
#include <liblas/detail/zippoint.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <cassert>
#include <cstdlib> // std::size_t
#include <algorithm>
#include <fstream>
#include <iosfwd>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace boost;
using namespace std;

namespace liblas { namespace detail { namespace writer {

Header::Header(std::ostream& ofs, boost::uint32_t& count, liblas::Header const& header)
    : m_ofs(ofs)
    , m_header(header)
    , m_pointCount(count)
{
}

void Header::write()
{

    uint8_t n1 = 0;
    uint16_t n2 = 0;
    uint32_t n4 = 0;
    
    // Figure out how many points we already have.  
    // Figure out if we're in append mode.  If we are, we can't rewrite 
    // any of the VLRs including the Schema and SpatialReference ones.
    bool bAppendMode = false;

    // This test should only be true if we were opened in both 
    // std::ios::in *and* std::ios::out

    // Seek to the beginning
    m_ofs.seekp(0, ios::beg);
    ios::pos_type begin = m_ofs.tellp();

    // Seek to the end
    m_ofs.seekp(0, ios::end);
    ios::pos_type end = m_ofs.tellp();
    if ((begin != end) && (end != static_cast<ios::pos_type>(0))) {
        bAppendMode = true;
    }

    // If we are in append mode, we are not touching *any* VLRs. 
    if (bAppendMode) 
    {
        // Believe the header
        m_pointCount = m_header.GetPointRecordsCount();

        // Position to the beginning of the file to start writing the header
        m_ofs.seekp(0, ios::beg);
    } 
    else 
    {
        // Rewrite the georeference VLR entries if they exist
        m_header.DeleteVLRs("liblas", 2112);
        m_header.SetGeoreference();

        // If we have a custom schema, add the VLR and write it into the 
        // file.  
        if (m_header.GetSchema().IsCustom()) {
            
            // Wipe any schema-related VLRs we might have, as this is now out of date.
            m_header.DeleteVLRs("liblas", 7);
        
            VariableRecord v = m_header.GetSchema().GetVLR();
            std::cout <<  m_header.GetSchema()<< std::endl;
            m_header.AddVLR(v);
        }
    
        // add the laszip VLR, if needed
        if (m_header.Compressed())
        {
#ifdef HAVE_LASZIP
            m_header.DeleteVLRs("laszip encoded", 22204);
            ZipPoint zpd(m_header.GetDataFormatId(), m_header.GetVLRs());
            VariableRecord v;
            zpd.ConstructVLR(v);
            m_header.AddVLR(v);
#else
            throw configuration_error("LASzip compression support not enabled in this libLAS configuration.");
#endif
        }
        else
        {
            m_header.DeleteVLRs("laszip encoded", 22204);
        }
        
        int32_t existing_padding = m_header.GetDataOffset() - 
                                  (m_header.GetVLRBlockSize() + 
                                   m_header.GetHeaderSize());

        if (existing_padding < 0) 
        {
            int32_t d = abs(existing_padding);
            
            // If our required VLR space is larger than we have 
            // room for, we have no padding.  AddVLRs will take care 
            // of incrementing up the space it needs.
            if (static_cast<boost::int32_t>(m_header.GetVLRBlockSize()) > d)
            {
                m_header.SetHeaderPadding(0);
            } else {
                m_header.SetHeaderPadding(d - m_header.GetVLRBlockSize());
            }
        } else {
            // cast is safe, we've already checked for < 0
            if (static_cast<uint32_t>(existing_padding) >= m_header.GetHeaderPadding())
            {
                m_header.SetHeaderPadding(existing_padding);
            }
            else {
                m_header.SetHeaderPadding(m_header.GetHeaderPadding() + existing_padding);
            }

        }

        m_header.SetDataOffset( m_header.GetHeaderSize() + 
                                m_header.GetVLRBlockSize() + 
                                m_header.GetHeaderPadding());
    }

    
    // 1. File Signature
    std::string const filesig(m_header.GetFileSignature());
    assert(filesig.size() == 4);
    detail::write_n(m_ofs, filesig, 4);
    
    
    // 2. File SourceId / Reserved
    if (m_header.GetVersionMinor()  ==  0) {
        n4 = m_header.GetReserved();
        detail::write_n(m_ofs, n4, sizeof(n4));         
    } else if (m_header.GetVersionMinor()  >  0) {
        n2 = m_header.GetFileSourceId();
        detail::write_n(m_ofs, n2, sizeof(n2));                
        n2 = m_header.GetReserved();
        detail::write_n(m_ofs, n2, sizeof(n2));        
    } 

    // 3-6. GUID data
    boost::uint8_t d[16];
    boost::uuids::uuid u = m_header.GetProjectId();

    d[0] = u.data[3];
    d[1] = u.data[2];
    d[2] = u.data[1];
    d[3] = u.data[0];
    d[4] = u.data[5];
    d[5] = u.data[4];
    d[6] = u.data[7];
    d[7] = u.data[6];
    for (int i=8; i<16; i++)
        d[i] = u.data[i];
    
    detail::write_n(m_ofs, d, 16);
    
    // 7. Version major
    n1 = m_header.GetVersionMajor();
    assert(1 == n1);
    detail::write_n(m_ofs, n1, sizeof(n1));
    
    // 8. Version minor
    n1 = m_header.GetVersionMinor();
    detail::write_n(m_ofs, n1, sizeof(n1));

    // 9. System ID
    std::string sysid(m_header.GetSystemId(true));
    assert(sysid.size() == 32);
    detail::write_n(m_ofs, sysid, 32);
    
    // 10. Generating Software ID
    std::string softid(m_header.GetSoftwareId(true));
    assert(softid.size() == 32);
    detail::write_n(m_ofs, softid, 32);

    // 11. Flight Date Julian
    n2 = m_header.GetCreationDOY();
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 12. Year
    n2 = m_header.GetCreationYear();
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 13. Header Size
    n2 = m_header.GetHeaderSize();
    assert(227 <= n2);
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 14. Offset to data
    n4 = m_header.GetDataOffset();
    detail::write_n(m_ofs, n4, sizeof(n4));

    // 15. Number of variable length records
    n4 = m_header.GetRecordsCount();
    detail::write_n(m_ofs, n4, sizeof(n4));

    // 16. Point Data Format ID
    n1 = static_cast<uint8_t>(m_header.GetDataFormatId());
    uint8_t n1tmp = n1;
    if (m_header.Compressed()) // high bit set indicates laszip compression
        n1tmp |= 0x80;
    detail::write_n(m_ofs, n1tmp, sizeof(n1tmp));

    // 17. Point Data Record Length
    n2 = m_header.GetDataRecordLength();
    detail::write_n(m_ofs, n2, sizeof(n2));

    // 18. Number of point records
    // This value is updated if necessary, see UpdateHeader function.
    n4 = m_header.GetPointRecordsCount();
    detail::write_n(m_ofs, n4, sizeof(n4));

    // 19. Number of points by return
    std::vector<uint32_t>::size_type const srbyr = 5;
    std::vector<uint32_t> const& vpbr = m_header.GetPointRecordsByReturnCount();
    // TODO: fix this for 1.3, which has srbyr = 7;  See detail/reader/header.cpp for more details
    // assert(vpbr.size() <= srbyr);
    uint32_t pbr[srbyr] = { 0 };
    std::copy(vpbr.begin(), vpbr.begin() + srbyr, pbr); // FIXME: currently, copies only 5 records, to be improved
    detail::write_n(m_ofs, pbr, sizeof(pbr));

    // 20-22. Scale factors
    detail::write_n(m_ofs, m_header.GetScaleX(), sizeof(double));
    detail::write_n(m_ofs, m_header.GetScaleY(), sizeof(double));
    detail::write_n(m_ofs, m_header.GetScaleZ(), sizeof(double));

    // 23-25. Offsets
    detail::write_n(m_ofs, m_header.GetOffsetX(), sizeof(double));
    detail::write_n(m_ofs, m_header.GetOffsetY(), sizeof(double));
    detail::write_n(m_ofs, m_header.GetOffsetZ(), sizeof(double));

    // 26-27. Max/Min X
    detail::write_n(m_ofs, m_header.GetMaxX(), sizeof(double));
    detail::write_n(m_ofs, m_header.GetMinX(), sizeof(double));

    // 28-29. Max/Min Y
    detail::write_n(m_ofs, m_header.GetMaxY(), sizeof(double));
    detail::write_n(m_ofs, m_header.GetMinY(), sizeof(double));

    // 30-31. Max/Min Z
    detail::write_n(m_ofs, m_header.GetMaxZ(), sizeof(double));
    detail::write_n(m_ofs, m_header.GetMinZ(), sizeof(double));

    if (!bAppendMode) 
    {
        WriteVLRs();

        // if we have padding, we should write it
        if (m_header.GetHeaderPadding() > 0) {
            m_ofs.seekp(m_header.GetHeaderSize() + m_header.GetVLRBlockSize(), std::ios::end);
            detail::write_n(m_ofs, "\0", m_header.GetHeaderPadding());
        }

        // Write the 1.0 pad signature if we need to.
        WriteLAS10PadSignature(); 
    }

    // If we already have points, we're going to put it at the end of the file.  
    // If we don't have any points,  we're going to leave it where it is.
    if (m_pointCount != 0)
    {
        m_ofs.seekp(0, std::ios::end);
    }
    else
    {
        m_ofs.seekp(m_header.GetDataOffset(), std::ios::beg);
    }
    
}

void Header::WriteVLRs() 
{

    // Seek to the end of the public header block (beginning of the VLRs)
    // to start writing
    m_ofs.seekp(m_header.GetHeaderSize(), std::ios::beg);

    int32_t diff = m_header.GetDataOffset() - GetRequiredHeaderSize();
    
    if (diff < 0) {

        m_header.SetDataOffset(GetRequiredHeaderSize());
        // Seek to the location of the data offset in the header and write a new one.
        m_ofs.seekp(96, std::ios::beg);
        detail::write_n(m_ofs, m_header.GetDataOffset(), sizeof(m_header.GetDataOffset()));
        m_ofs.seekp(m_header.GetHeaderSize(), std::ios::beg);
        
        // std::ostringstream oss;
        // oss << "Header is not large enough to contain VLRs.  Data offset is ";
        // oss << m_header.GetDataOffset() << " while the required total size ";
        // oss << "for the VLRs is " << GetRequiredHeaderSize();
        // throw std::runtime_error(oss.str());
    }

    for (uint32_t i = 0; i < m_header.GetRecordsCount(); ++i)
    {
        VariableRecord const &vlr = m_header.GetVLR(i);

        detail::write_n(m_ofs, vlr.GetReserved(), sizeof(uint16_t));
        detail::write_n(m_ofs, vlr.GetUserId(true).c_str(), 16);
        detail::write_n(m_ofs, vlr.GetRecordId(), sizeof(uint16_t));
        detail::write_n(m_ofs, vlr.GetRecordLength(), sizeof(uint16_t));
        detail::write_n(m_ofs, vlr.GetDescription(true).c_str(), 32);
        std::vector<uint8_t> const& data = vlr.GetData();
        std::streamsize const size = static_cast<std::streamsize>(data.size());
        detail::write_n(m_ofs, data.front(), size);
    }



}

boost::int32_t Header::GetRequiredHeaderSize() const
{
    return m_header.GetVLRBlockSize() + m_header.GetHeaderSize();
}

void Header::WriteLAS10PadSignature()
{
    // Only write pad signature bytes for LAS 1.0 files.  Any other files 
    // will not get the pad bytes and we are *not* allowing anyone to 
    // override this either - hobu
    
    if (m_header.GetVersionMinor() > 0) {
        return;
    }

    int32_t diff = m_header.GetDataOffset() - GetRequiredHeaderSize();

    if (diff < 2) {
        
        m_header.SetDataOffset(m_header.GetDataOffset() + 2);
        // Seek to the location of the data offset in the header and write a new one.
        m_ofs.seekp(96, std::ios::beg);
        detail::write_n(m_ofs, m_header.GetDataOffset(), sizeof(m_header.GetDataOffset()));
    }    
    
    
    // step back two bytes to write the pad bytes.  We should have already
    // determined by this point if a) they will fit b) they won't overwrite 
    // exiting real data 
    m_ofs.seekp(m_header.GetDataOffset() - 2, std::ios::beg);
    
    // Write the pad bytes.
    uint8_t const sgn1 = 0xCC;
    uint8_t const sgn2 = 0xDD;
    detail::write_n(m_ofs, sgn1, sizeof(uint8_t));
    detail::write_n(m_ofs, sgn2, sizeof(uint8_t));
}


}}} // namespace liblas::detail::writer
