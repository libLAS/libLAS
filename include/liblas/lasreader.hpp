/******************************************************************************
* $Id$
*
* Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
* Purpose:  LAS reader class 
* Author:   Mateusz Loskot, mateusz@loskot.net
*
******************************************************************************
* Copyright (c) 2008, Mateusz Loskot
* Copyright (c) 2008, Phil Vachon
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

#ifndef LIBLAS_LASREADER_HPP_INCLUDED
#define LIBLAS_LASREADER_HPP_INCLUDED

#include <liblas/detail/fwd.hpp>
#include <liblas/liblas.hpp>

#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/lasspatialreference.hpp>


// std
#include <iosfwd>
#include <string>
#include <memory>
#include <vector>
#include <cstddef>

namespace liblas {

/// Defines public interface to LAS reader implementation.
class Reader
{
public:

    /// Consructor initializes reader with input stream as source of LAS records.
    /// @param ifs - stream used as source of LAS records.
    /// @excepion std::runtime_error - on failure state of the input stream.
    Reader(std::istream& ifs);

    Reader(ReaderI* reader);
    
    /// User-defined consructor initializes reader with input stream and
    /// a header to override the values in the file
    /// @excepion std::runtime_error - on failure state of the input stream.
    Reader(std::istream& ifs, Header& header);
    
    /// Destructor.
    /// @excepion nothrow
    ~Reader();
    
    /// Provides read-only access to header of LAS file being read.
    /// @excepion nothrow
    Header const& GetHeader() const;

    /// Provides read-only access to current point record.
    /// @excepion nothrow
    Point const& GetPoint() const;

    /// Provides read-only access to collection of variable-length records.
    /// @excepion nothrow
    std::vector<VariableRecord> const& GetVLRs() const;

    /// Allow fetching of the stream attached to the reader.
    /// @excepion nothrow
    std::istream& GetStream() const;

    /// Checks if end-of-file has been reached.
    bool IsEOF() const;

    /// Fetches next point record in file.
    /// @excepion may throw std::exception
    bool ReadNextPoint();

    /// Fetches n-th point record from file.
    /// @excepion may throw std::exception
    bool ReadPointAt(std::size_t n);

    /// Reinitializes state of the reader.
    /// @excepion may throw std::exception
    void Reset();

    /// Reproject data as they are written if the Reader's reference is
    /// different than the Header's.
    /// @excepion may throw std::exception
    bool SetSRS(const SpatialReference& ref);
    
    /// Override the spatial reference of the Reader's Header for 
    /// writing purposes.
    /// @excepion may throw std::exception
    bool SetInputSRS(const SpatialReference& ref);

    /// Override the spatial reference of the Reader's Header for 
    /// writing purposes.
    /// @excepion may throw std::exception
    bool SetOutputSRS(const SpatialReference& ref);

    /// Provides index-based access to point records.
    /// The operator is implemented in terms of ReadPointAt method
    /// and is not const-qualified because it updates file stream position.
    /// @excepion may throw std::exception
    Point const& operator[](std::size_t n);

private:

    // Blocked copying operations, declared but not defined.
    Reader(Reader const& other);
    Reader& operator=(Reader const& rhs);

    void Init(); // throws on error

    const std::auto_ptr<ReaderI> m_pimpl;
    Header m_header;
    Point* m_point;
    Point* m_empty_point;
    
    // Set if the user provides a header to override the header as 
    // read from the istream
    bool bCustomHeader;

    std::istream& m_ifs;
    
};



} // namespace liblas

#endif // ndef LIBLAS_LASREADER_HPP_INCLUDED
