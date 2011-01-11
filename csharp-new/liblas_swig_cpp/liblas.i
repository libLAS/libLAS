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
 
 %module WrapperCpp

%{
#include "liblas/liblas.hpp"
#include "liblas/version.hpp"
#include "liblas/reader.hpp"
#include "liblas/classification.hpp"
#include "liblas/header.hpp"
#include "liblas/point.hpp"
#include "liblas/color.hpp"
#include "liblas/guid.hpp"
#include "liblas/factory.hpp"
%}


%include "typemaps.i"

%include "std_string.i"

%include "std_vector.i"
namespace std {
   %template(VectorU8) vector<unsigned char>;
   %template(VectorU32) vector<unsigned int>;

   %template(VectorVariableRecord) vector<liblas::VariableRecord>;
};
 
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


namespace liblas
{


class guid
{
public:
    std::string to_string() const;
    //void output_data(boost::uint32_t& d1, boost::uint16_t& d2, boost::uint16_t& d3, boost::uint8_t (&d4)[8]) const;
};


class Color
{
public:
    typedef boost::uint16_t value_type;

    value_type GetRed() const;
    value_type GetBlue() const;
    value_type GetGreen() const;
};
	
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

class Classification
{
public:
    typedef std::bitset<8> bitset_type;
    enum BitPosition
    {
        eClassBit     = 0, ///< First bit position of 0:4 range.
        eSyntheticBit = 5, ///< Synthetic flag.
        eKeyPointBit  = 6, ///< Key-point flag.
        eWithheldBit  = 7  ///< Withheld flag.
    };
    std::string GetClassName() const;
    boost::uint8_t GetClass() const;
    bool IsSynthetic() const;
    /// Tests if this point is considered to be a model keypoint.
    bool IsKeyPoint() const;
    bool IsWithheld() const;
};


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

    enum ScanAngleRankRange
    {
        eScanAngleRankMin = -90,
        eScanAngleRankMax = 90
    };

    double GetX() const;
    double GetY() const;
    double GetZ() const;
      
    boost::uint16_t GetIntensity() const;

    boost::uint8_t GetScanFlags() const;

    boost::uint16_t GetReturnNumber() const;

    boost::uint16_t GetNumberOfReturns() const;

    boost::uint16_t GetScanDirection() const;
    
    boost::uint16_t GetFlightLineEdge() const;
    Classification GetClassification() const;
    boost::int8_t GetScanAngleRank() const;
    boost::uint8_t GetUserData() const;
    boost::uint16_t GetPointSourceID() const;

    Color GetColor() const;
    double GetTime() const;

    std::vector<boost::uint8_t> const& GetData() const {return m_data; }
};


class VariableRecord
{
public:
    boost::uint16_t GetReserved() const;
    std::string GetUserId(bool pad ) const;
    boost::uint16_t GetRecordId() const;
    boost::uint16_t GetRecordLength() const;
    std::string GetDescription(bool pad ) const;
    std::vector<boost::uint8_t> const& GetData() const;
    std::size_t GetTotalSize() const;
};

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

    const GTIF* GetGTIF();
    std::string GetWKT(WKTModeFlag mode_flag = eHorizontalOnly) const;
    std::string GetWKT(WKTModeFlag mode_flag, bool pretty) const;
    std::string GetProj4() const;
    std::vector<VariableRecord> GetVLRs() const;
};



class Header
{
public:
    typedef std::vector<boost::uint32_t> RecordsByReturnArray;

    std::string GetFileSignature() const;
    boost::uint16_t GetFileSourceId() const;
    boost::uint16_t GetReserved() const;
    guid GetProjectId() const;
    boost::uint8_t GetVersionMajor() const;
    boost::uint8_t GetVersionMinor() const;
    std::string GetSystemId(bool pad = false) const;
    std::string GetSoftwareId(bool pad = false) const;
    boost::uint16_t GetCreationDOY() const;
    boost::uint16_t GetCreationYear() const;
    boost::uint16_t GetHeaderSize() const;
    boost::uint32_t GetDataOffset() const;
    boost::uint32_t GetRecordsCount() const;
    PointFormatName GetDataFormatId() const;
    boost::uint16_t GetDataRecordLength() const;
    boost::uint32_t GetPointRecordsCount() const;
    RecordsByReturnArray const& GetPointRecordsByReturnCount() const;

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
    const std::vector<VariableRecord>& GetVLRs() const;

    SpatialReference GetSRS() const;

    bool Compressed() const;
};

class Reader
{
public:
    Reader(std::istream& ifs);
    ~Reader();

    Header const& GetHeader() const;
    Point const& GetPoint() const;
    bool ReadNextPoint();
    bool ReadPointAt(std::size_t n);
    void Reset();
    bool Seek(std::size_t n);
};



class ReaderFactory
{
public:
    Reader CreateCached(std::istream& stream, boost::uint32_t cache_size);
    Reader CreateWithStream(std::istream& stream);
    static std::istream* FileOpen(std::string const& filename);
};

class WriterFactory
{
public:
    enum FileType
    {
        FileType_Unknown,
        FileType_LAS,
        FileType_LAZ
    };

    //static WriterIPtr CreateWithStream(std::ostream& stream, Header const& header); 
    static FileType InferFileTypeFromExtension(const std::string&);
    static std::ostream* WriterFactory::FileCreate(std::string const& filename);
};

};
