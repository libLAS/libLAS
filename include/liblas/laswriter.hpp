/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS writer class 
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ******************************************************************************
 * Copyright (c) 2008, Mateusz Loskot
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

#ifndef LIBLAS_LASWRITER_HPP_INCLUDED
#define LIBLAS_LASWRITER_HPP_INCLUDED

#include <liblas/lasversion.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/detail/fwd.hpp>

// std
#include <iosfwd> // std::ostream
#include <string>
#include <memory>
#include <cstdlib> // std::size_t

namespace liblas
{

/// Defines public interface to LAS writer implementation.
/// This class 
class LASWriter
{
public:

    /// Consructor initializes reader with specified output stream and header specification.
    /// @param ofs - stream used as destination for LAS records.
    /// @param header - specifies obligatory properties of LAS file.
    /// @excepion std::runtime_error - on failure state of the input stream.
    LASWriter(std::ostream& ofs, LASHeader const& header);

    /// Destructor does not close file attached to the output stream
    /// Header may be updated after writing operation completed, if necessary
    /// in order to maintain data consistency.
    ~LASWriter();
    
    /// Provides access to header structure.
    LASHeader const& GetHeader() const;

    /// \todo TODO: Move point record composition deep into writer implementation.
    /// \todo TODO: How to handle point_source_id in portable way, for LAS 1.0 and 1.1
    bool WritePoint(LASPoint const& point);

    /// Allow fetching of the stream
    std::ostream& GetStream() const;
    
    /// Allow in-place writing of header
    void WriteHeader(LASHeader& header);

    /// Reproject data as they are written if the LASWriter's reference is
    /// different than the LASHeader's
    bool SetSRS(const LASSpatialReference& ref);
    bool SetInputSRS(const LASSpatialReference& ref);
    bool SetOutputSRS(const LASSpatialReference& ref);
    
private:
    
    // Blocked copying operations, declared but not defined.
    LASWriter(LASWriter const& other);
    LASWriter& operator=(LASWriter const& rhs);

    const std::auto_ptr<detail::WriterImpl> m_pimpl;

    LASHeader m_header;
    detail::PointRecord m_record;
};

} // namespace liblas

#endif // ndef LIBLAS_LASWRITER_HPP_INCLUDED
