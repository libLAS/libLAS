/******************************************************************************
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  swig/C# bindings for liblas
 * Author:   Michael P. Gerlek (mpg@flaxen.com)
 *
 ******************************************************************************
 * Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
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
 
%module liblas_swig_cpp

%{
#include "liblas/liblas.hpp"
#include "liblas/version.hpp"
#include "liblas/reader.hpp"
#include "liblas/writer.hpp"
#include "liblas/classification.hpp"
#include "liblas/header.hpp"
#include "liblas/point.hpp"
#include "liblas/color.hpp"
#include "liblas/guid.hpp"
#include "liblas/factory.hpp"
%}

%include "typemaps.i"

// C# support for std::string
%include "std_string.i"

// C# support for std::vector<T>
%include "std_vector.i"
namespace std {
   %template(VectorU8) vector<unsigned char>;
   %template(VectorU32) vector<unsigned int>;
   %template(VectorVariableRecord) vector<liblas::VariableRecord>;
};
 

// fix up some missing types
namespace std
{
	typedef unsigned int size_t;
};

namespace boost
{
	typedef unsigned char uint8_t;
	typedef signed char int8_t;
	typedef unsigned short uint16_t;
	typedef signed short int16_t;
	typedef unsigned int uint32_t;
	typedef signed int int32_t;
};

// BUG: how do I do a rename such that "SWIGTYPE_p_std__istream" can
// become something like "IStreamHandle"?

namespace liblas
{
typedef Header HeaderPtr;

//---------------------------------------------------------------------------
	
enum PointFormatName
{
    ePointFormat0 = 0,  ///< Point Data Format \e 0
    ePointFormat1 = 1,  ///< Point Data Format \e 1
    ePointFormat2 = 2,  ///< Point Data Format \e 2
    ePointFormat3 = 3,  ///< Point Data Format \e 3
    ePointFormat4 = 4,  ///< Point Data Format \e 3
    ePointFormat5 = 5,  ///< Point Data Format \e 3
    ePointFormatUnknown = -99 ///< Point Data Format is unknown
};


//---------------------------------------------------------------------------

class guid
{
public:
    guid(char const* const str);
    ~guid();
    std::string to_string() const;
};


//---------------------------------------------------------------------------

class Color
{
public:
    typedef boost::uint16_t value_type;

    value_type GetRed() const;
    value_type GetBlue() const;
    value_type GetGreen() const;
};


//---------------------------------------------------------------------------

class Classification
{
public:
    std::string GetClassName() const;
    bool IsSynthetic() const;
    bool IsKeyPoint() const;
    bool IsWithheld() const;
};


//---------------------------------------------------------------------------

class Point
{
public:
	enum DataMemberFlag
    {
        eReturnNumber = 1,
        eNumberOfReturns = 2,
        eScanDirection = 4,
        eFlightLineEdge = 8,
        eClassification = 16,
        eScanAngleRank = 32,
        eTime = 64
    };

	enum ClassificationType
    {
        eCreated = 0,
        eUnclassified,
        eGround,
        eLowVegetation,
        eMediumVegetation,
        eHighVegetation,
        eBuilding,
        eLowPoint,
        eModelKeyPoint,
        eWater = 9,
        // = 10 // reserved for ASPRS Definition
        // = 11 // reserved for ASPRS Definition
        eOverlapPoints = 12
        // = 13-31 // reserved for ASPRS Definition
    };

    Point();
    Point(HeaderPtr header);
    Point(Point const& other);

    double GetX() const;
    double GetY() const;
    double GetZ() const;
    void SetCoordinates(double const& x, double const& y, double const& z);
    void SetX(double const& value);
    void SetY(double const& value);
    void SetZ(double const& value);
	
	boost::uint16_t GetIntensity() const;
    void SetIntensity(boost::uint16_t const& intensity);

    boost::uint8_t GetScanFlags() const;
    void SetScanFlags(boost::uint8_t const& flags);

    boost::uint16_t GetReturnNumber() const;
    void SetReturnNumber(boost::uint16_t const& num);

    boost::uint16_t GetNumberOfReturns() const;
    void SetNumberOfReturns(boost::uint16_t const& num);

    boost::uint16_t GetScanDirection() const;
    void SetScanDirection(boost::uint16_t const& dir);
    
    boost::uint16_t GetFlightLineEdge() const;
    void SetFlightLineEdge(boost::uint16_t const& edge);

    Classification GetClassification() const;
    void SetClassification(Classification const& cls);

    Color GetColor() const;
    void SetColor(Color const& value);

    double GetTime() const;
    void SetTime(double const& time);
};


//---------------------------------------------------------------------------

class VariableRecord
{
public:
    VariableRecord(); 
    VariableRecord(VariableRecord const& other);
    ~VariableRecord();
	
	std::string GetUserId(bool pad) const;
    boost::uint16_t GetRecordId() const;
    boost::uint16_t GetRecordLength() const;
    std::string GetDescription(bool pad) const;
    std::vector<boost::uint8_t> const& GetData() const;
};


//---------------------------------------------------------------------------

class SpatialReference
{
public:
    enum WKTModeFlag
    {
        eHorizontalOnly = 1,
        eCompoundOK = 2
    };

    enum GeoVLRType
    {
        eGeoTIFF = 1,
        eOGRWKT = 2
    };

    SpatialReference();
    ~SpatialReference();
    SpatialReference(std::vector<VariableRecord> const& vlrs);
    SpatialReference(SpatialReference const& other);

    std::string GetWKT(WKTModeFlag mode_flag, bool pretty) const;
    std::string GetProj4() const;
    std::vector<VariableRecord> GetVLRs() const;
};


//---------------------------------------------------------------------------

class Header
{
public:
    Header();
    Header(Header const& other);

    std::string GetFileSignature() const;
    boost::uint16_t GetFileSourceId() const;
    void SetFileSourceId(boost::uint16_t v);
    guid GetProjectId() const;
    void SetProjectId(guid const& v);
    boost::uint8_t GetVersionMajor() const;
    void SetVersionMajor(boost::uint8_t v);
    boost::uint8_t GetVersionMinor() const;
    void SetVersionMinor(boost::uint8_t v);
    std::string GetSystemId(bool pad = false) const;
    void SetSystemId(std::string const& v);
    std::string GetSoftwareId(bool pad = false) const;
    void SetSoftwareId(std::string const& v);
    boost::uint16_t GetCreationDOY() const;
    void SetCreationDOY(boost::uint16_t v);
    boost::uint16_t GetCreationYear() const;
    void SetCreationYear(boost::uint16_t v);

    PointFormatName GetDataFormatId() const;
    void SetDataFormatId(PointFormatName v);

    boost::uint32_t GetRecordsCount() const;
    boost::uint32_t GetPointRecordsCount() const;

    double GetScaleX() const;
    double GetScaleY() const;
    double GetScaleZ() const;
    double GetOffsetX() const;
    double GetOffsetY() const;
    double GetOffsetZ() const;
    double GetMaxX() const;
    double GetMinX() const;
    double GetMaxY() const;
    double GetMinY() const;
    double GetMaxZ() const;
    double GetMinZ() const;
    void SetScale(double x, double y, double z);
    void SetOffset(double x, double y, double z);
    void SetMax(double x, double y, double z);
    void SetMin(double x, double y, double z);

    const std::vector<VariableRecord>& GetVLRs() const;

    SpatialReference GetSRS() const;

    bool Compressed() const;
    void SetCompressed(bool b);
};


//---------------------------------------------------------------------------

class Reader
{
public:
    Reader(std::istream& ifs);
    Reader(Reader const& other);
    ~Reader();

	// BUG: GetHeader and GetPoint make new proxy objects at every call.
	// For perf reasons, it'd be nice to avoid that.
	// BUG: GetHeader and GetPoint will cause GC issues if the Reader goes
	// away while you're still using the Point or Header (see 18.6.1 of the
	// swig manual)
    Header const& GetHeader() const;
    Point const& GetPoint() const;

    bool ReadNextPoint();
    bool ReadPointAt(std::size_t n);
    void Reset();
    bool Seek(std::size_t n);
};


//---------------------------------------------------------------------------

class Writer
{
public:
    Writer(std::ostream& ofs, Header const& header);
    Writer(Writer const& other);
    ~Writer();

	// BUG: see Reader::GetHeader() above
    Header const& GetHeader() const;

    bool WritePoint(Point const& point);

    //void WriteHeader(Header& header);
    //void SetFilters(std::vector<liblas::FilterPtr> const& filters);
    //void SetTransforms(std::vector<liblas::TransformPtr> const& transforms);
};


//---------------------------------------------------------------------------

class ReaderFactory
{
public:
    ReaderFactory();
    ~ReaderFactory();
    Reader CreateCached(std::istream& stream, boost::uint32_t cache_size);
    Reader CreateWithStream(std::istream& stream);
    static std::istream* FileOpen(std::string const& filename);
    static void FileClose(std::istream*);
};


//---------------------------------------------------------------------------

class WriterFactory
{
public:
    WriterFactory();
    ~WriterFactory();
    static std::ostream* FileCreate(std::string const& filename);
    static void FileClose(std::ostream*);
};

}; // namespace
