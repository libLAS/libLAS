// $Id$
//
// (C) Copyright Phil Vachon 2008, philippe@cowpig.ca
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/lasrecordheader.hpp>
#include <liblas/cstdint.hpp>
// std
#include <string>

namespace liblas {

LASRecordHeader::LASRecordHeader() :
    m_reserved(0), m_recordId(0), m_recordLength(0)
{    
}

LASRecordHeader::LASRecordHeader(LASRecordHeader const& other) :
    m_reserved(other.m_reserved),
    m_recordId(other.m_recordId),
    m_recordLength(other.m_recordLength),
    m_userId(other.m_userId),
    m_desc(other.m_desc)
{
}

LASRecordHeader& LASRecordHeader::operator=(LASRecordHeader const& rhs)
{
    if (this != &rhs)
    {
        m_reserved = rhs.m_reserved;
        m_recordId = rhs.m_recordId;
        m_recordLength = rhs.m_recordLength;
        m_userId = rhs.m_userId;
        m_desc = rhs.m_desc;
    }
    return (*this);
}

uint16_t LASRecordHeader::GetReserved() const
{
    return m_reserved;
}

std::string const& LASRecordHeader::GetUserId() const
{
    return m_userId;
}

uint16_t LASRecordHeader::GetRecordId() const
{
    return m_recordId;
}

uint16_t LASRecordHeader::GeRecordLength() const
{
    return m_recordLength;
}

std::string const& LASRecordHeader::GetDescription() const
{
    return m_desc;
}

bool LASRecordHeader::equal(LASRecordHeader const& other) const
{
    return (m_recordId == other.m_recordId
            && m_userId == other.m_userId 
            && m_desc == other.m_desc
            && m_reserved == other.m_reserved
            && m_recordLength == other.m_recordLength);
}

} // namespace liblas
