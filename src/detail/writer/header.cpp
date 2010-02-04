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

#include <liblas/detail/writer/header.hpp>
#include <liblas/detail/utility.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lasspatialreference.hpp>

#include <cassert>
#include <cstdlib> // std::size_t
#include <fstream>
#include <iosfwd>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


namespace liblas { namespace detail { namespace writer {

Header::Header(std::ostream& ofs, liblas::uint32_t& count, LASHeader const& header) :
    Base(ofs, count)
{
    m_header = header;
}

void Header::write()
{

    uint8_t n1 = 0;
    uint16_t n2 = 0;
    uint32_t n4 = 0;

    // Rewrite the georeference VLR entries if they exist
    m_header.SetGeoreference();
    
    // Seek to the beginning
    GetStream().seekp(0, std::ios::beg);
    std::ios::pos_type beginning = GetStream().tellp();

    // Seek to the end
    GetStream().seekp(0, std::ios::end);
    std::ios::pos_type end = GetStream().tellp();
    
    // Figure out how many points we already have.  Each point record 
    // should be 20 bytes long, and header.GetDataOffset tells
    // us the location to start counting points from.  
    
    // This test should only be true if we were opened in both 
    // std::ios::in *and* std::ios::out, otherwise it should return false 
    // and we won't adjust the point count.
    
    if ((beginning != end) && ((uint32_t)end != 0)) {
        uint32_t count = ((uint32_t) end - m_header.GetDataOffset())/m_header.GetDataRecordLength();
        SetPointCount(count);

        // Position to the beginning of the file to start writing the header
        GetStream().seekp(0, std::ios::beg);
    }

    // 1. File Signature
    std::string const filesig(m_header.GetFileSignature());
    assert(filesig.size() == 4);
    detail::write_n(GetStream(), filesig, 4);
    
    // 2. Reserved
    n4 = m_header.GetReserved();
    detail::write_n(GetStream(), n4, sizeof(n4));

    // 3-6. GUID data
    uint32_t d1 = 0;
    uint16_t d2 = 0;
    uint16_t d3 = 0;
    uint8_t d4[8] = { 0 };
    liblas::guid g = m_header.GetProjectId();
    g.output_data(d1, d2, d3, d4);
    detail::write_n(GetStream(), d1, sizeof(d1));
    detail::write_n(GetStream(), d2, sizeof(d2));
    detail::write_n(GetStream(), d3, sizeof(d3));
    detail::write_n(GetStream(), d4, sizeof(d4));
    
    // 7. Version major
    n1 = m_header.GetVersionMajor();
    assert(1 == n1);
    detail::write_n(GetStream(), n1, sizeof(n1));
    
    // 8. Version minor
    n1 = m_header.GetVersionMinor();
    assert(0 == n1);
    detail::write_n(GetStream(), n1, sizeof(n1));

    // 9. System ID
    std::string sysid(m_header.GetSystemId(true));
    assert(sysid.size() == 32);
    detail::write_n(GetStream(), sysid, 32);
    
    // 10. Generating Software ID
    std::string softid(m_header.GetSoftwareId(true));
    assert(softid.size() == 32);
    detail::write_n(GetStream(), softid, 32);

    // 11. Flight Date Julian
    n2 = m_header.GetCreationDOY();
    detail::write_n(GetStream(), n2, sizeof(n2));

    // 12. Year
    n2 = m_header.GetCreationYear();
    detail::write_n(GetStream(), n2, sizeof(n2));

    // 13. Header Size
    n2 = m_header.GetHeaderSize();
    assert(227 <= n2);
    detail::write_n(GetStream(), n2, sizeof(n2));

    // 14. Offset to data
    // Because we are 1.0, we must also add pad bytes to the end of the 
    // m_header.  This means resetting the dataoffset +=2, but we 
    // don't want to change the header's actual offset until after we 
    // write the VLRs or else we'll be off by 2 when we write the pad
    // bytes
    n4 = m_header.GetDataOffset() + 2;
    detail::write_n(GetStream(), n4, sizeof(n4));

    // 15. Number of variable length records
    // TODO: This value must be updated after new variable length record is added.
    n4 = m_header.GetRecordsCount();
    detail::write_n(GetStream(), n4, sizeof(n4));

    // 16. Point Data Format ID
    n1 = static_cast<uint8_t>(m_header.GetDataFormatId());
    detail::write_n(GetStream(), n1, sizeof(n1));

    // 17. Point Data Record Length
    n2 = m_header.GetDataRecordLength();
    detail::write_n(GetStream(), n2, sizeof(n2));

    // 18. Number of point records
    // This value is updated if necessary, see UpdateHeader function.
    n4 = m_header.GetPointRecordsCount();
    detail::write_n(GetStream(), n4, sizeof(n4));

    // 19. Number of points by return
    std::vector<uint32_t>::size_type const srbyr = 5;
    std::vector<uint32_t> const& vpbr = m_header.GetPointRecordsByReturnCount();
    assert(vpbr.size() <= srbyr);
    uint32_t pbr[srbyr] = { 0 };
    std::copy(vpbr.begin(), vpbr.end(), pbr);
    detail::write_n(GetStream(), pbr, sizeof(pbr));

    // 20-22. Scale factors
    detail::write_n(GetStream(), m_header.GetScaleX(), sizeof(double));
    detail::write_n(GetStream(), m_header.GetScaleY(), sizeof(double));
    detail::write_n(GetStream(), m_header.GetScaleZ(), sizeof(double));

    // 23-25. Offsets
    detail::write_n(GetStream(), m_header.GetOffsetX(), sizeof(double));
    detail::write_n(GetStream(), m_header.GetOffsetY(), sizeof(double));
    detail::write_n(GetStream(), m_header.GetOffsetZ(), sizeof(double));

    // 26-27. Max/Min X
    detail::write_n(GetStream(), m_header.GetMaxX(), sizeof(double));
    detail::write_n(GetStream(), m_header.GetMinX(), sizeof(double));

    // 28-29. Max/Min Y
    detail::write_n(GetStream(), m_header.GetMaxY(), sizeof(double));
    detail::write_n(GetStream(), m_header.GetMinY(), sizeof(double));

    // 30-31. Max/Min Z
    detail::write_n(GetStream(), m_header.GetMaxZ(), sizeof(double));
    detail::write_n(GetStream(), m_header.GetMinZ(), sizeof(double));

    // If WriteVLR returns a value, it is because the header's 
    // offset is not large enough to contain the VLRs.  The value 
    // it returns is the number of bytes we must increase the header
    // by in order for it to contain the VLRs.
    
    // int32_t difference = WriteVLR(m_header);
    // if (difference < 0) {
    //     m_header.SetDataOffset(m_header.GetDataOffset() + abs(difference) );
    //     WriteVLR(header);
    // }
    
    // Write the pad bytes.
    uint8_t const sgn1 = 0xCC;
    uint8_t const sgn2 = 0xDD;
    detail::write_n(GetStream(), sgn1, sizeof(uint8_t));
    detail::write_n(GetStream(), sgn2, sizeof(uint8_t));

    // We can now reset the header's offset to +=2.  If we monkeypatched
    // the offset because we were too small to write the VLRs, this will 
    // end up being header.GetDataOffset() + difference + 2 (see above).
    m_header.SetDataOffset(m_header.GetDataOffset() + 2);

    // Make sure to rewrite the dataoffset in the header portion now that
    // we've changed it.
    std::streamsize const current_pos = GetStream().tellp();
    std::streamsize const offset_pos = 96; 
    GetStream().seekp(offset_pos, std::ios::beg);
    detail::write_n(GetStream(), m_header.GetDataOffset() , sizeof(m_header.GetDataOffset()));
    GetStream().seekp(current_pos, std::ios::beg);        


    
    // If we already have points, we're going to put it at the end of the file.  
    // If we don't have any points,  we're going to leave it where it is.
    if (GetPointCount() != 0)
        GetStream().seekp(0, std::ios::end);    
}

// Writer::Writer(std::ostream& ofs) : m_ofs(ofs), m_transform(0), m_in_ref(0), m_out_ref(0)
// {
// }
// 
// Writer::~Writer()
// {
// #ifdef HAVE_GDAL
//     if (m_transform) {
//         OCTDestroyCoordinateTransformation(m_transform);
//     }
//     if (m_in_ref) {
//         OSRDestroySpatialReference(m_in_ref);
//     }
//     if (m_out_ref) {
//         OSRDestroySpatialReference(m_out_ref);
//     }
// #endif
// }
// 
// std::ostream& Writer::GetStream() const
// {
//     return m_ofs;
// }
// 
// 
// void Writer::FillPointRecord(PointRecord& record, const LASPoint& point, const LASHeader& header) 
// {
// 
//     if (m_transform) {
//         // let's just copy the point for now.
//         LASPoint p = LASPoint(point);
//         Project(p);
//         record.x = static_cast<int32_t>((p.GetX() - header.GetOffsetX()) / header.GetScaleX());
//         record.y = static_cast<int32_t>((p.GetY() - header.GetOffsetY()) / header.GetScaleY());
//         record.z = static_cast<int32_t>((p.GetZ() - header.GetOffsetZ()) / header.GetScaleZ());
//     } else {
//         record.x = static_cast<int32_t>((point.GetX() - header.GetOffsetX()) / header.GetScaleX());
//         record.y = static_cast<int32_t>((point.GetY() - header.GetOffsetY()) / header.GetScaleY());
//         record.z = static_cast<int32_t>((point.GetZ() - header.GetOffsetZ()) / header.GetScaleZ());
//     }
// 
//     LASClassification::bitset_type clsflags(point.GetClassification());
//     record.classification = static_cast<uint8_t>(clsflags.to_ulong());
// 
//     record.intensity = point.GetIntensity();
//     record.flags = point.GetScanFlags();
//     record.scan_angle_rank = point.GetScanAngleRank();
//     record.user_data = point.GetUserData();
//     record.point_source_id = point.GetPointSourceID();
// }
// 
// uint32_t Writer::WriteVLR(LASHeader const& header) 
// {
//     // If this function returns a value, it is the size that the header's 
//     // data offset must be increased by in order for the VLRs to fit in 
//     // the header.  
//     m_ofs.seekp(header.GetHeaderSize(), std::ios::beg);
// 
//     // if the VLRs won't fit because the data offset is too 
//     // small, we need to throw an error.
//     uint32_t vlr_total_size = 0;
//         
//     // Calculate a new data offset size
//     for (uint32_t i = 0; i < header.GetRecordsCount(); ++i)
//     {
//         LASVariableRecord vlr = header.GetVLR(i);
//         vlr_total_size += vlr.GetTotalSize();
//     }
//     
//     int32_t difference = header.GetDataOffset() - (vlr_total_size + header.GetHeaderSize());
// 
//     if (difference < 0) 
//     {
//         return difference;
//     }
//     
//     for (uint32_t i = 0; i < header.GetRecordsCount(); ++i)
//     {
//         LASVariableRecord vlr = header.GetVLR(i);
// 
//         detail::write_n(m_ofs, vlr.GetReserved(), sizeof(uint16_t));
//         detail::write_n(m_ofs, vlr.GetUserId(true).c_str(), 16);
//         detail::write_n(m_ofs, vlr.GetRecordId(), sizeof(uint16_t));
//         detail::write_n(m_ofs, vlr.GetRecordLength(), sizeof(uint16_t));
//         detail::write_n(m_ofs, vlr.GetDescription(true).c_str(), 32);
//         std::vector<uint8_t> const& data = vlr.GetData();
//         std::streamsize const size = static_cast<std::streamsize>(data.size());
//         detail::write_n(m_ofs, data.front(), size);
//     }
//     
//     // if we had more room than we need for the VLRs, we need to pad that with 
//     // 0's.  We must also not forget to add the 1.0 pad bytes to the end of this
//     // but the impl should be the one doing that, not us.
//     if (difference > 0) {
//         detail::write_n(m_ofs, "\0", difference);
//     }
//     return 0;
// }
// 
// 
// void Writer::SetOutputSRS(const LASSpatialReference& srs )
// {
//     m_out_srs = srs;
//     CreateTransform();
// 
// }
// 
// void Writer::SetSRS(const LASSpatialReference& srs )
// {
//     SetOutputSRS(srs);
// }
// 
// void Writer::SetInputSRS(const LASSpatialReference& srs )
// {
//     m_in_srs = srs;
// }
// 
// void Writer::CreateTransform()
// {
// #ifdef HAVE_GDAL
//     if (m_transform)
//     {
//         OCTDestroyCoordinateTransformation(m_transform);
//     }
//     if (m_in_ref)
//     {
//         OSRDestroySpatialReference(m_in_ref);
//     }
//     if (m_out_ref)
//     {
//         OSRDestroySpatialReference(m_out_ref);
//     }
//     
//     m_in_ref = OSRNewSpatialReference(0);
//     m_out_ref = OSRNewSpatialReference(0);
// 
//     int result = OSRSetFromUserInput(m_in_ref, m_in_srs.GetWKT().c_str());
//     if (result != OGRERR_NONE) 
//     {
//         std::ostringstream msg; 
//         msg << "Could not import input spatial reference for Writer::" << CPLGetLastErrorMsg() << result;
//         std::string message(msg.str());
//         throw std::runtime_error(message);
//     }
//     
//     result = OSRSetFromUserInput(m_out_ref, m_out_srs.GetWKT().c_str());
//     if (result != OGRERR_NONE) 
//     {
//         std::ostringstream msg; 
//         msg << "Could not import output spatial reference for Writer::" << CPLGetLastErrorMsg() << result;
//         std::string message(msg.str());
//         throw std::runtime_error(message);
//     }
// 
//     m_transform = OCTNewCoordinateTransformation( m_in_ref, m_out_ref);
// #endif
// }
// 
// void Writer::Project(LASPoint& p)
// {
// #ifdef HAVE_GDAL
//     
//     int ret = 0;
//     double x = p.GetX();
//     double y = p.GetY();
//     double z = p.GetZ();
//     
//     ret = OCTTransform(m_transform, 1, &x, &y, &z);
//     
//     if (!ret) {
//         std::ostringstream msg; 
//         msg << "Could not project point for Writer::" << CPLGetLastErrorMsg() << ret;
//         std::string message(msg.str());
//         throw std::runtime_error(message);
//     }
//     
//     p.SetX(x);
//     p.SetY(y);
//     p.SetZ(z);
// #else
//     detail::ignore_unused_variable_warning(p);
// #endif
// }
// 
// Writer* WriterFactory::Create(std::ostream& ofs, LASHeader const& header)
// {
//     if (!ofs)
//     {
//         throw std::runtime_error("output stream state is invalid");
//     }
// 
//     // Select writer implementation based on requested LAS version.
//     uint8_t major = header.GetVersionMajor();
//     uint8_t minor = header.GetVersionMinor();
//     
//     if (1 == major && 0 == minor)
//     {
//         return new v10::WriterImpl(ofs);
//     }
//     if (1 == major && 1 == minor)
//     {
//         return new v11::WriterImpl(ofs);
//     }
//     if (1 == major && 2 == minor)
//     {
//         return new v12::WriterImpl(ofs);
//     }
//     else if (2 == major && 0 == minor)
//     {
//         // TODO: LAS 2.0 read/write support
//         throw std::runtime_error("LAS 2.0 file detected but unsupported");
//     }
// 
//     throw std::runtime_error("LAS file of unknown version");
// }
// 
// void WriterFactory::Destroy(Writer* p) 
// {
//     delete p;
//     p = 0;
// }

}}} // namespace liblas::detail::writer
