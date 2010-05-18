/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS include file
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ******************************************************************************
 * Copyright (c) 2008, Mateusz Loskot
 * Copyright (c) 2008, Phil Vachon
 * Copyright (c) 2008, Howard Butler
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


#ifndef LIBLAS_HPP_INCLUDED
#define LIBLAS_HPP_INCLUDED

#include <liblas/cstdint.hpp>
#include <liblas/detail/fwd.hpp>
#include <fstream>
#include <string>

/// Namespace grouping all elements of libLAS public interface.
/// \note
/// User's may notice there is namespace \em detail nested
/// in the \em liblas namespace. The \em detail should be considered as private
/// namespace dedicated for implementation details, so libLAS users are not
/// supposed to access it directly, nor included headers from the \em detail
/// subdirectory of \em liblas include folder.
namespace liblas
{

/// Open file to read in binary mode.
/// The input file is also attached to input stream.
/// \param ifs - reference to input file stream to
/// which opened file is attached
/// \param filename - name of file to open
/// \return true if file has been opened with success, false otherwise
/// \exception No throw
///
inline bool Open(std::ifstream& ifs, std::string const& filename) // throw()
{
    ifs.open(filename.c_str(), std::ios::in | std::ios::binary);
    return ifs.is_open();
}

/// Create file and open to write in binary mode.
/// The output file is also attached to output stream.
/// \param ofs - reference to output file stream to
/// which created file is attached
/// \param filename - name of file to open
/// \return true if file has been create with success, false otherwise
/// \exception No throw
///
inline bool Create(std::ofstream& ofs, std::string const& filename) // throw()
{
    ofs.open(filename.c_str(), std::ios::out | std::ios::binary);
    return ofs.is_open();
}

/// Check if GDAL support has been built in to libLAS.
inline bool IsGDALEnabled()
{
#ifdef HAVE_GDAL
    return true;
#else
    return false;
#endif
}

/// Check if GeoTIFF support has been built in to libLAS.
inline bool IsLibGeoTIFFEnabled()
{
#ifdef HAVE_LIBGEOTIFF
    return true;
#else
    return false;
#endif
}

/// Check if libspatialindex support has been built in to libLAS.
inline bool IsLibSpatialIndexEnabled()
{
#ifdef HAVE_SPATIALINDEX
    return true;
#else
    return false;
#endif
}

/// Range of allowed ASPRS LAS file format versions.
enum FormatVersion
{
    eVersionMajorMin = 1, ///< Minimum of major component
    eVersionMajorMax = 1, ///< Maximum of major component
    eVersionMinorMin = 0, ///< Minimum of minor component
    eVersionMinorMax = 2 ///< Maximum of minor component
};

/// Versions of point record format.
enum PointFormatName
{
    ePointFormat0 = 0, ///< Point Data Format \e 0
    ePointFormat1 = 1, ///< Point Data Format \e 1
    ePointFormat2 = 2, ///< Point Data Format \e 2
    ePointFormat3 = 3,  ///< Point Data Format \e 3
    ePointFormatUnknown = -99 ///< Point Data Format is unknown
};

/// Number of bytes of point record storage in particular format.
enum PointSize
{
    ePointSize0 = 20, ///< Size of point record in data format \e 0
    ePointSize1 = 28, ///< Size of point record in data format \e 1
    ePointSize2 = 26, ///< Size of point record in data format \e 2
    ePointSize3 = 34  ///< Size of point record in data format \e 3

};


class ReaderI
{
public:

    virtual Header const& ReadHeader() = 0;
    virtual Point const& ReadNextPoint(const Header& header) = 0;
    virtual Point const& ReadPointAt(std::size_t n, const Header& header) = 0;

    virtual void Reset(const Header& header) = 0;
    virtual void SetInputSRS(const SpatialReference& srs) = 0;
    virtual void SetOutputSRS(const SpatialReference& srs, const Header& header) = 0;
    
    virtual std::istream& GetStream() const = 0;
    
    virtual ~ReaderI() {};    
};

class WriterI
{
public:

    virtual Header const& WriteHeader(const Header& header) = 0;
    virtual void UpdateHeader(const Header& header) = 0;
    virtual void WritePoint(const Point& point, const Header& header) = 0;

    virtual void SetInputSRS(const SpatialReference& srs) = 0;
    virtual void SetOutputSRS(const SpatialReference& srs, const Header& header) = 0;

    virtual std::ostream& GetStream() const = 0;

    virtual ~WriterI() {};    

};

/// Defines public interface to LAS filter implementation.
class FilterI
{
public:
    
    virtual bool filter(const Point& point) = 0;
    virtual void SetKeep(bool bKeep) = 0;
    virtual ~FilterI() {};

};



} // namespace liblas

#endif // LIBLAS_HPP_INCLUDED
