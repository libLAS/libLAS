// $Id$
//
// (C) Copyright Phil Vachon 2008, philippe@cowpig.ca
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_LASRECORDHEADER_HPP_INCLUDED
#define LIBLAS_LASRECORDHEADER_HPP_INCLUDED

#include <liblas/cstdint.hpp>
// std
#include <string>

namespace liblas {

/// Representation of variable-length record data.
class LASRecordHeader
{
public:

    /// Default constructor.
    /// Zero-initialization of record data.
    /// \exception No throw
    LASRecordHeader();

    /// Copy constructor.
    /// Construction of new record object as a copy of existing one.
    /// \exception No throw
    LASRecordHeader(LASRecordHeader const& other);

    /// Assignment operator.
    /// Construction and initializition of record object by
    /// assignment of another one.
    /// \exception No throw
    LASRecordHeader& operator=(LASRecordHeader const& rhs);

    /// Get record signature (LAS 1.0) or reserved bytes (LAS 1.1).
    /// \exception No throw
    uint16_t GetReserved() const;

    /// Get identifier of user which created the record.
    /// The character data is up to 16 bytes long.
    /// \exception No throw
    std::string const& GetUserId() const;

    /// Get identifier of record.
    /// The record ID is closely related to the user ID.
    /// \exception No throw
    uint16_t GetRecordId() const;

    /// Get record length after the header.
    /// \exception No throw
    uint16_t GeRecordLength() const;

    /// Get text description of data in the record.
    /// The character data is up to 32 bytes long.
    /// \exception No throw
    std::string const& GetDescription() const;

    /// Compare actual header object against the other.
    /// \exception No throw
    bool equal(LASRecordHeader const& other) const;

private:

    uint16_t m_reserved;
    uint16_t m_recordId;
    uint16_t m_recordLength; // after header
    std::string m_userId; // [16]
    std::string m_desc; // [32]
};

/// Equality operator.
/// Implemented in terms of LASRecordHeader::equal member function.
/// \exception No throw
inline bool operator==(LASRecordHeader const& lhs, LASRecordHeader const& rhs)
{
    return lhs.equal(rhs);
}

/// Inequality operator.
/// Implemented in terms of LASRecordHeader::equal member function.
/// \exception No throw
inline bool operator!=(LASRecordHeader const& lhs, LASRecordHeader const& rhs)
{
    return (!(lhs == rhs));
}

} // namespace liblas

#endif // LIBLAS_LASRECORDHEADER_HPP_INCLUDED
