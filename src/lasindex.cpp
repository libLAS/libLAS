/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  LAS index class 
 * Author:   Gary Huber, gary@garyhuberart.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Gary Huber, gary@garyhuberart.com
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

#include <liblas/lasindex.hpp>
#include <liblas/laswriter.hpp>
#include <liblas/detail/index/indexoutput.hpp>
#include <liblas/detail/index/indexcell.hpp>

namespace liblas
{

Index::Index()
{
	SetValues();
	m_indexBuilt = IndexInit();
} // Index::Index

Index::Index(liblas::Reader *reader, std::ostream *ofs, const char *tmpfilenme, const char *outfilenme, 
	const char *indexauthor, const char *indexcomment, const char *indexdate, 
	double zbinht, uint32_t maxmem,	int debugOutput, FILE *debugger)
{
	SetValues();
	m_reader = reader;
	m_ofs = ofs;
    m_debugOutputLevel = debugOutput;
	m_tempFileName = tmpfilenme ? tmpfilenme: "";
	m_outFileName = outfilenme ? outfilenme: "";
	m_indexAuthor = indexauthor ? indexauthor: "";
	m_indexComment = indexcomment ? indexcomment: "";
	m_indexDate = indexdate ? indexdate: "";
	m_cellSizeZ = zbinht;
	m_debugger = debugger ? debugger: stderr;
	if (maxmem > 0)
		m_maxMemoryUsage = maxmem;
	else
		m_maxMemoryUsage = LIBLAS_INDEX_MAXMEMDEFAULT;
	m_indexBuilt = IndexInit();
} // Index::Index

Index::Index(std::istream *ifs, std::ostream *ofs, const char *tmpfilenme, const char *outfilenme, 
	const char *indexauthor, const char *indexcomment, const char *indexdate, 
	double zbinht, uint32_t maxmem,	int debugOutput, FILE *debugger)
{
	SetValues();
	m_reader = new liblas::Reader(*ifs);
	m_readerCreated = true;
	m_ofs = ofs;
    m_debugOutputLevel = debugOutput;
	m_tempFileName = tmpfilenme ? tmpfilenme: "";
	m_outFileName = outfilenme ? outfilenme: "";
	m_indexAuthor = indexauthor ? indexauthor: "";
	m_indexComment = indexcomment ? indexcomment: "";
	m_indexDate = indexdate ? indexdate: "";
	m_cellSizeZ = zbinht;
	m_debugger = debugger ? debugger: stderr;
	if (maxmem > 0)
		m_maxMemoryUsage = maxmem;	// 10 megs default
	else
		m_maxMemoryUsage = LIBLAS_INDEX_MAXMEMDEFAULT;
	m_indexBuilt = IndexInit();
} // Index::Index

Index::Index(IndexData const& ParamSrc)
{
	SetValues();
	m_reader = ParamSrc.m_reader;
 	m_readerCreated = false;
	if (! m_reader)
	{
		m_reader = new liblas::Reader(*ParamSrc.m_ifs);
 		m_readerCreated = true;
	} // if
	m_ofs = ParamSrc.m_ofs;
    m_debugOutputLevel = ParamSrc.m_debugOutputLevel;
	m_tempFileName = ParamSrc.m_tempFileName ? ParamSrc.m_tempFileName: "";
	m_outFileName = ParamSrc.m_outFileName ? ParamSrc.m_outFileName: "";
	m_indexAuthor = ParamSrc.m_indexAuthor ? ParamSrc.m_indexAuthor: "";
	m_indexComment = ParamSrc.m_indexComment ? ParamSrc.m_indexComment: "";
	m_indexDate = ParamSrc.m_indexDate ? ParamSrc.m_indexDate: "";
	m_cellSizeZ = ParamSrc.m_cellSizeZ;
	m_debugger = ParamSrc.m_debugger ? ParamSrc.m_debugger: stderr;
	m_readOnly = ParamSrc.m_readOnly;
	m_forceNewIndex = ParamSrc.m_forceNewIndex;
	if (ParamSrc.m_maxMemoryUsage > 0)
		m_maxMemoryUsage = ParamSrc.m_maxMemoryUsage;
	else
		m_maxMemoryUsage = LIBLAS_INDEX_MAXMEMDEFAULT;
	m_indexBuilt = IndexInit();
} // Index::Index

void Index::SetValues(void)
{
    m_reader = 0;
    m_ofs = 0;
  	m_readerCreated = false;
	m_tempFile = 0;
	m_outputFile = 0;
    m_debugOutputLevel = 0;
    m_tempFileName = "";
    m_outFileName = "";
	m_indexAuthor = "";
	m_indexComment = "";
	m_indexDate = "";
	m_cellSizeZ = 0.0;
	m_debugger = stderr;
	m_readOnly = false;
	m_forceNewIndex = false;
	m_DataVLR_ID = 43;
	m_maxMemoryUsage = LIBLAS_INDEX_MAXMEMDEFAULT;
    m_minX = m_maxX = m_minY = m_maxY = m_minZ = m_maxZ = m_scaleX = m_scaleY = m_scaleZ = 
		m_offsetX = m_offsetY = m_offsetZ = m_rangeX = m_rangeY = m_rangeZ = m_cellSizeZ = m_cellSizeX = m_cellSizeY = 
		m_filterMinXCell = m_filterMaxXCell = m_filterMinYCell = m_filterMaxYCell = m_filterMinZCell = m_filterMaxZCell = 0.0;
    m_pointRecordsCount = m_maxMemoryUsage = m_cellsX = m_cellsY = m_cellsZ = m_totalCells = m_tempFileWrittenBytes = 0;
    m_LowXCellCompletelyIn = m_HighXCellCompletelyIn = m_LowYCellCompletelyIn = m_HighYCellCompletelyIn = 
		m_LowZCellCompletelyIn = m_HighZCellCompletelyIn =
		m_LowXBorderCell = m_HighXBorderCell = m_LowYBorderCell = m_HighYBorderCell = 
		m_LowZBorderCell = m_HighZBorderCell = 0;
	m_indexBuilt = m_tempFileStarted = m_readerCreated = false;
} // Index::SetValues

Index::~Index(void)
{
	if (m_readerCreated)
		delete m_reader;
} // Index::~Index

bool Index::IndexInit(void)
{
	bool Success = false;
	bool IndexFound = false;

	if (m_reader)
	{
		m_header = m_reader->GetHeader();
		uint32_t initialVLRs = m_header.GetRecordsCount();
		for (uint32_t i = 0; i < initialVLRs; ++i)
		{
			VariableRecord const& vlr = m_header.GetVLR(i);
			// a combination of "liblas" and 42 denotes that this is a liblas spatial index id
			if (std::string(vlr.GetUserId(false)) == std::string("liblas"))
			{
				if (vlr.GetRecordId() == 42)
				{
					LoadIndexVLR(vlr);
					IndexFound = true;
					break;
				}
			}
		}
		if (IndexFound)
		{
			if (m_forceNewIndex)
			{
				ClearOldIndex();
				IndexFound = false;
				if (m_debugOutputLevel > 1)
					fprintf(m_debugger, "Old index removed.\n");
			} // if
			else
				return true;
		} // if
		if (! IndexFound)
		{
			if (! m_readOnly)
			{
				Success = BuildIndex();
				uint32_t test = m_header.GetRecordsCount() - initialVLRs;
				if (m_debugOutputLevel > 1)
					fprintf(m_debugger, "VLRs created %d\n", test);
			} // if
			else if (m_debugOutputLevel > 1)
				fprintf(m_debugger, "Index not found nor created per user instructions.\n");
		} // if
		return Success;
	} // m_reader
	return (InitError("IndexInit"));	
} // Index::IndexInit

void Index::ClearOldIndex(void)
{
	uint32_t initialVLRs = m_header.GetRecordsCount();
	for (uint32_t i = 0; i < initialVLRs; ++i)
	{
		VariableRecord const& vlr = m_header.GetVLR(i);
		// a combination of "liblas" and 42 denotes that this is a liblas spatial index id
		if (std::string(vlr.GetUserId(false)) == std::string("liblas"))
		{
			if (vlr.GetRecordId() == 42 || vlr.GetRecordId() == 43)
			{
				m_header.DeleteVLR(i);
			} // if
		} // if
	} // for
} // Index::ClearOldIndex

const std::vector<uint32_t>& Index::Filter(IndexData const& ParamSrc)
{

	// if there is already a list, get rid of it
	m_filterResult.resize(0);
	if (m_reader)
	{
		for (uint32_t i = 0; i < m_header.GetRecordsCount(); ++i)
		{
			VariableRecord const& vlr = m_header.GetVLR(i);
			// a combination of "liblas" and 42 denotes that this is a liblas spatial index id
			if (std::string(vlr.GetUserId(false)) == std::string("liblas"))
			{
				uint16_t RecordID = vlr.GetRecordId();
				if (RecordID == 42)
				{
					if (! LoadIndexVLR(vlr))
						break;
					// Beyond the first record would be more VLR's with the actual index data
					// some cells will fall completely inside, some outside and some straddling the filter bounds	
					SetCellFilterBounds(ParamSrc);
				} // if 42
				else if (RecordID == m_DataVLR_ID)
				{
					// some of our data is in this record
					if (! FilterOneVLR(vlr, i, ParamSrc))
						break;
				} // else if ID matches ID stored in index header
			}
		}
	} // m_reader
	return (m_filterResult);

} // Index::Filter

void Index::SetCellFilterBounds(IndexData const& ParamSrc)
{
	double LowXCell, HighXCell, LowYCell, HighYCell, LowZCell, HighZCell;
	// convert filter bounds into cell numbers
	m_filterMinXCell = m_cellsX * (ParamSrc.m_lowFilterX - m_minX) / m_rangeX;
	m_filterMaxXCell = m_cellsX * (ParamSrc.m_highFilterX - m_minX) / m_rangeX;
	m_filterMinYCell = m_cellsY * (ParamSrc.m_lowFilterY - m_minY) / m_rangeY;
	m_filterMaxYCell = m_cellsY * (ParamSrc.m_highFilterY - m_minY) / m_rangeY;
	m_filterMinZCell = m_cellsZ * (ParamSrc.m_lowFilterZ - m_minZ) / m_rangeZ;
	m_filterMaxZCell = m_cellsZ * (ParamSrc.m_highFilterZ - m_minZ) / m_rangeZ;
	LowXCell = ceil(m_filterMinXCell);
	HighXCell = floor(m_filterMaxXCell) - 1.0;
	LowYCell = ceil(m_filterMinYCell);
	HighYCell = floor(m_filterMaxYCell) - 1.0;
	LowZCell = ceil(m_filterMinZCell);
	HighZCell = floor(m_filterMaxZCell) - 1.0;
	m_LowXCellCompletelyIn = (int32_t)LowXCell;
	m_HighXCellCompletelyIn = (int32_t)HighXCell;
	m_LowYCellCompletelyIn = (int32_t)LowYCell;
	m_HighYCellCompletelyIn = (int32_t)HighYCell;
	m_LowZCellCompletelyIn = (int32_t)LowZCell;
	m_HighZCellCompletelyIn = (int32_t)HighZCell;

    LowXCell = floor(m_filterMinXCell);
    HighXCell = ceil(m_filterMaxXCell) - 1.0;
    LowYCell = floor(m_filterMinYCell);
    HighYCell = ceil(m_filterMaxYCell) - 1.0;
    LowZCell = floor(m_filterMinZCell);
    HighZCell = ceil(m_filterMaxZCell) - 1.0;
    m_LowXBorderCell = (int32_t)LowXCell;
    m_HighXBorderCell = (int32_t)HighXCell;
    m_LowYBorderCell = (int32_t)LowYCell;
    m_HighYBorderCell = (int32_t)HighYCell;
    m_LowZBorderCell = (int32_t)LowZCell;
    m_HighZBorderCell = (int32_t)HighZCell;
	
} // Index::SetCellFilterBounds

bool Index::LoadIndexVLR(VariableRecord const& vlr)
{
	uint8_t VersionMajor, VersionMinor;
	char DestStr[512];
	uint16_t StringLen;
	uint16_t ReadPos = 0;

	try {
		//uint16_t VLRIndexRecLen = vlr.GetRecordLength();
		// GetData returns a vector of liblas::uint8_t
		// std::vector<uint8_t>
		// read the first record of our index data
		std::vector<uint8_t> const& VLRIndexData = vlr.GetData();
		// parse the index header data
		memcpy(&VersionMajor, &VLRIndexData[ReadPos], sizeof(uint8_t));
		ReadPos += sizeof(uint8_t);
		memcpy(&VersionMinor, &VLRIndexData[ReadPos], sizeof(uint8_t));
		ReadPos += sizeof(uint8_t);
		// creator		
		memcpy(&StringLen, &VLRIndexData[ReadPos], sizeof(uint16_t));
		ReadPos += sizeof(uint16_t);
		strncpy(DestStr, (const char *)&VLRIndexData[ReadPos], StringLen);	// includes NULL terminator
		SetIndexAuthorStr(DestStr);
		ReadPos = ReadPos + StringLen;
		
		// comment
		memcpy(&StringLen, &VLRIndexData[ReadPos], sizeof(uint16_t));
		ReadPos += sizeof(uint16_t);
		strncpy(DestStr, (const char *)&VLRIndexData[ReadPos], StringLen);	// includes NULL terminator
		SetIndexCommentStr(DestStr);
		ReadPos = ReadPos + StringLen;

		// date	
		memcpy(&StringLen, &VLRIndexData[ReadPos], sizeof(uint16_t));
		ReadPos += sizeof(uint16_t);
		strncpy(DestStr, (const char *)&VLRIndexData[ReadPos], StringLen);	// includes NULL terminator
		SetIndexDateStr(DestStr);
		ReadPos = ReadPos + StringLen;

		// file index extents
		double TempData;
		memcpy(&TempData, &VLRIndexData[ReadPos], sizeof(double));
		SetMinX(TempData);
		ReadPos += sizeof(double);
		memcpy(&TempData, &VLRIndexData[ReadPos], sizeof(double));
		SetMaxX(TempData);
		ReadPos += sizeof(double);
		memcpy(&TempData, &VLRIndexData[ReadPos], sizeof(double));
		SetMinY(TempData);
		ReadPos += sizeof(double);
		memcpy(&TempData, &VLRIndexData[ReadPos], sizeof(double));
		SetMaxY(TempData);
		ReadPos += sizeof(double);
		memcpy(&TempData, &VLRIndexData[ReadPos], sizeof(double));
		SetMinZ(TempData);
		ReadPos += sizeof(double);
		memcpy(&TempData, &VLRIndexData[ReadPos], sizeof(double));
		SetMaxZ(TempData);
		ReadPos += sizeof(double);
		
		// ID number of associated data VLR's - normally 43 but may use heigher numbers
		// in order to store more than one index in a file
		uint32_t TempLong;
		memcpy(&TempLong, &VLRIndexData[ReadPos], sizeof(uint32_t));
		SetDataVLR_ID(TempLong);
		ReadPos += sizeof(uint32_t);
		
		// number of points indexed and cells in the index
		memcpy(&TempLong, &VLRIndexData[ReadPos], sizeof(uint32_t));
		SetPointRecordsCount(TempLong);
		ReadPos += sizeof(uint32_t);
		memcpy(&TempLong, &VLRIndexData[ReadPos], sizeof(uint32_t));
		SetCellsX(TempLong);
		ReadPos += sizeof(uint32_t);
		memcpy(&TempLong, &VLRIndexData[ReadPos], sizeof(uint32_t));
		SetCellsY(TempLong);
		ReadPos += sizeof(uint32_t);
		memcpy(&TempLong, &VLRIndexData[ReadPos], sizeof(uint32_t));
		SetCellsZ(TempLong);
		ReadPos += sizeof(uint32_t);
		
		CalcRangeX();
		CalcRangeY(); 
		CalcRangeZ();
	} // try
	catch (std::out_of_range) {
		return (false);
	} // catch
	return true;

} // Index::LoadIndexVLR

bool Index::FilterOneVLR(VariableRecord const& vlr, uint32_t& i, IndexData const& ParamSrc)
{

	uint16_t ReadPos = 0;
	uint32_t MinCellX, MinCellY, MaxCellX, MaxCellY, DataRecordSize = 0;
	std::vector<uint8_t> CompositeData;
	
	try {
		std::vector<uint8_t> const& VLRIndexRecordData = vlr.GetData();
		uint16_t VLRIndexRecLen = vlr.GetRecordLength();
		CompositeData.resize(VLRIndexRecLen);
		memcpy(&CompositeData[0], &VLRIndexRecordData[0], VLRIndexRecLen);
			
		memcpy(&MinCellX, &CompositeData[ReadPos], sizeof(uint32_t));
		ReadPos += sizeof(uint32_t);
		memcpy(&MinCellY, &CompositeData[ReadPos], sizeof(uint32_t));
		ReadPos += sizeof(uint32_t);
		// last cell in VLR, x, y
		memcpy(&MaxCellX, &CompositeData[ReadPos], sizeof(uint32_t));
		ReadPos += sizeof(uint32_t);
		memcpy(&MaxCellY, &CompositeData[ReadPos], sizeof(uint32_t));
		ReadPos += sizeof(uint32_t);
		// data record size
		memcpy(&DataRecordSize, &CompositeData[ReadPos], sizeof(uint32_t));
		ReadPos += sizeof(uint32_t);
				
		if (DataRecordSize > VLRIndexRecLen)
		{
			CompositeData.resize(DataRecordSize);
			// read more records and concatenate data
			uint32_t ReadData = VLRIndexRecLen;
			uint32_t UnreadData = DataRecordSize - ReadData;
			while (UnreadData)
			{
				++i;
				VariableRecord const& vlr2 = m_header.GetVLR(i);
				std::vector<uint8_t> const& TempData = vlr2.GetData();
				uint16_t TempRecLen = vlr2.GetRecordLength();
				memcpy(&CompositeData[ReadData], &TempData[0], TempRecLen);
				ReadData += TempRecLen;
				if (UnreadData >= TempRecLen)
					UnreadData -= TempRecLen;
				else
					// this is an error if we get here
					UnreadData = 0;
			} // while
		} // if
			
		// translate the data for this VLR
		while (ReadPos < DataRecordSize)
		{
			// current cell, x, y
			uint32_t x, y, PtRecords, SubCellsXY, SubCellsZ;
			memcpy(&x, &CompositeData[ReadPos], sizeof(uint32_t));
			ReadPos += sizeof(uint32_t);
			memcpy(&y, &CompositeData[ReadPos], sizeof(uint32_t));
			ReadPos += sizeof(uint32_t);
			// min and max Z
			liblas::detail::ElevExtrema CellMinZ, CellMaxZ;
			memcpy(&CellMinZ, &CompositeData[ReadPos], sizeof(liblas::detail::ElevExtrema));
			ReadPos += sizeof(liblas::detail::ElevExtrema);
			memcpy(&CellMaxZ, &CompositeData[ReadPos], sizeof(liblas::detail::ElevExtrema));
			ReadPos += sizeof(liblas::detail::ElevExtrema);
			// number of subcells in this cell in both XY and Z
			memcpy(&PtRecords, &CompositeData[ReadPos], sizeof(uint32_t));
			ReadPos += sizeof(uint32_t);
			memcpy(&SubCellsXY, &CompositeData[ReadPos], sizeof(uint32_t));
			ReadPos += sizeof(uint32_t);
			memcpy(&SubCellsZ, &CompositeData[ReadPos], sizeof(uint32_t));
			ReadPos += sizeof(uint32_t);
			
			// read the data stored in Z cells
			for (uint32_t SubCellZ = 0; SubCellZ < SubCellsZ; ++SubCellZ)
			{
				uint32_t ZCellID;
				memcpy(&ZCellID, &CompositeData[ReadPos], sizeof(uint32_t));
				ReadPos += sizeof(uint32_t);
				// number of point records in subcell
				uint32_t ZCellNumRecords;
				memcpy(&ZCellNumRecords, &CompositeData[ReadPos], sizeof(uint32_t));
				ReadPos += sizeof(uint32_t);
				for (uint32_t SubCellZPt = 0; SubCellZPt < ZCellNumRecords; ++SubCellZPt)
				{
					uint32_t PointID, LastPointID = static_cast<uint32_t>(~0);
					bool LastPtRead = 0;
					memcpy(&PointID, &CompositeData[ReadPos], sizeof(uint32_t));
					ReadPos += sizeof(uint32_t);
					liblas::detail::ConsecPtAccumulator ConsecutivePts;
					memcpy(&ConsecutivePts, &CompositeData[ReadPos], sizeof(liblas::detail::ConsecPtAccumulator));
					ReadPos += sizeof(liblas::detail::ConsecPtAccumulator);
					for (uint32_t PtCt = 0; PtCt < ConsecutivePts; ++PointID, ++PtCt)
					{
						if (FilterOnePoint(x, y, ZCellID, PointID, LastPointID, LastPtRead, ParamSrc))
							m_filterResult.push_back(PointID);
						LastPointID = PointID;
					} // for
				} // for
			} // for
			// read the data stored in XY quadtree cells
			for (uint32_t SubCellXY = 0; SubCellXY < SubCellsXY; ++SubCellXY)
			{
				uint32_t SubCellID;
				memcpy(&SubCellID, &CompositeData[ReadPos], sizeof(uint32_t));
				ReadPos += sizeof(uint32_t);
				// number of point records in subcell
				uint32_t SubCellNumRecords;
				memcpy(&SubCellNumRecords, &CompositeData[ReadPos], sizeof(uint32_t));
				ReadPos += sizeof(uint32_t);
				for (uint32_t SubCellPt = 0; SubCellPt < SubCellNumRecords; ++SubCellPt)
				{
					uint32_t PointID, LastPointID = static_cast<uint32_t>(~0);
					bool LastPtRead = 0;
					memcpy(&PointID, &CompositeData[ReadPos], sizeof(uint32_t));
					ReadPos += sizeof(uint32_t);
					liblas::detail::ConsecPtAccumulator ConsecutivePts;
					memcpy(&ConsecutivePts, &CompositeData[ReadPos], sizeof(liblas::detail::ConsecPtAccumulator));
					ReadPos += sizeof(liblas::detail::ConsecPtAccumulator);
					for (uint32_t PtCt = 0; PtCt < ConsecutivePts; ++PointID, ++PtCt)
					{
						if (FilterOnePoint(x, y, 0, PointID, LastPointID, LastPtRead, ParamSrc))
							m_filterResult.push_back(PointID);
						LastPointID = PointID;
					} // for
				} // for
			} // for
			// read data in unsubdivided cells
			if (! (SubCellsZ || SubCellsXY))
			{
				for (uint32_t CurPt = 0; CurPt < PtRecords; ++CurPt)
				{
					uint32_t PointID, LastPointID = static_cast<uint32_t>(~0);
					bool LastPtRead = 0;
					memcpy(&PointID, &CompositeData[ReadPos], sizeof(uint32_t));
					ReadPos += sizeof(uint32_t);
					liblas::detail::ConsecPtAccumulator ConsecutivePts;
					memcpy(&ConsecutivePts, &CompositeData[ReadPos], sizeof(liblas::detail::ConsecPtAccumulator));
					ReadPos += sizeof(liblas::detail::ConsecPtAccumulator);
					for (uint32_t PtCt = 0; PtCt < ConsecutivePts; ++PointID, ++PtCt)
					{
						if (FilterOnePoint(x, y, 0, PointID, LastPointID, LastPtRead, ParamSrc))
							m_filterResult.push_back(PointID);
						LastPointID = PointID;
					} // for
				} // for
			} // if
		} // while
	} // try
	catch (std::out_of_range) {
		return (false);
	} // catch
	return true;
	
} // Index::FilterOneVLR

bool Index::FilterOnePoint(int32_t x, int32_t y, int32_t z, int32_t PointID, int32_t LastPointID, bool &LastPtRead, 
	IndexData const& ParamSrc)
{
	bool XGood = false, YGood = false, ZGood = false, PtRead = false;
	double PtX, PtY = 0.0, PtZ = 0.0;
	
	if (ParamSrc.m_noFilterX)
		XGood = true;
	else if (x >= m_LowXCellCompletelyIn && x <= m_HighXCellCompletelyIn)
		XGood = true;
	else if (x == m_LowXBorderCell || x == m_HighXBorderCell)
	{
		if ((((PointID == LastPointID + 1) && LastPtRead) && m_reader->ReadNextPoint())	
			|| m_reader->ReadPointAt(PointID))
		{
			PtRead = LastPtRead = true;
			Point TestPt = m_reader->GetPoint();
			PtX = TestPt.GetX();
			PtY = TestPt.GetY();
			PtZ = TestPt.GetZ();
			if (PtX >= ParamSrc.m_lowFilterX && PtX <= ParamSrc.m_highFilterX)
				XGood = true;
		} // if
	} // else
	if (XGood)
	{
		if (ParamSrc.m_noFilterY)
			YGood = true;
		else if (y >= m_LowYCellCompletelyIn && y <= m_HighYCellCompletelyIn)
			YGood = true;
		else if (y == m_LowYBorderCell || y == m_HighYBorderCell)
		{
			if (PtRead)
			{
				if (PtY >= ParamSrc.m_lowFilterY && PtY <= ParamSrc.m_highFilterY)
					YGood = true;
			}
			else if ((((PointID == LastPointID + 1) && LastPtRead) && m_reader->ReadNextPoint()) 
				|| m_reader->ReadPointAt(PointID))
			{
				PtRead = LastPtRead = true;
				Point TestPt = m_reader->GetPoint();
				PtY = TestPt.GetY();
				PtZ = TestPt.GetZ();
				if (PtY >= ParamSrc.m_lowFilterY && PtY <= ParamSrc.m_highFilterY)
					YGood = true;
			} // if
		} // else if
	} // if
	if (XGood && YGood)
	{
		if (ParamSrc.m_noFilterZ)
			ZGood = true;
		else if (z >= m_LowZCellCompletelyIn && z <= m_HighZCellCompletelyIn)
			ZGood = true;
		else if (z == m_LowZBorderCell || z == m_HighZBorderCell)
		{
			if (PtRead)
			{
				if (PtZ >= ParamSrc.m_lowFilterZ && PtZ <= ParamSrc.m_highFilterZ)
					ZGood = true;
			}
			else if ((((PointID == LastPointID + 1) && LastPtRead) && m_reader->ReadNextPoint()) 
				|| m_reader->ReadPointAt(PointID))
			{
				PtRead = LastPtRead = true;
				Point TestPt = m_reader->GetPoint();
				PtZ = TestPt.GetZ();
				if (PtZ >= ParamSrc.m_lowFilterZ && PtZ <= ParamSrc.m_highFilterZ)
					ZGood = true;
			} // if
		} // else if
	} // if
	return (XGood && YGood && ZGood);
	
} // Index::FilterOnePoint

bool Index::BuildIndex(void)
{
	// Build an array of two dimensions. Sort data points into
	uint32_t MaximumCells = 250000;
	
	// reset to beginning of point data records in case points had been examined before index is built
	m_reader->Reset();

	// need the header to get number of point records
    m_pointRecordsCount = m_header.GetPointRecordsCount();
    // get the bounds of the data and scale factors in case they are needed for point translation
    m_minX = m_header.GetMinX();
    m_maxX = m_header.GetMaxX();
    m_minY = m_header.GetMinY();
    m_maxY = m_header.GetMaxY();
    m_minZ = m_header.GetMinZ();
    m_maxZ = m_header.GetMaxZ();
    m_scaleX = m_header.GetScaleX();
    m_scaleY = m_header.GetScaleY();
    m_scaleZ = m_header.GetScaleZ();
    m_offsetX = m_header.GetOffsetX();
    m_offsetY = m_header.GetOffsetY();
    m_offsetZ = m_header.GetOffsetZ();
	CalcRangeX();
	CalcRangeY(); 
	CalcRangeZ();

	if (m_cellSizeZ > 0.0)
		m_cellsZ = static_cast<uint32_t>(ceil(m_rangeZ / m_cellSizeZ));
	else
		m_cellsZ = 1;
		
	// under the conditions of one dimension being 0 or negative in size, no index is possible
	if (m_maxX <= m_minX || m_maxY <= m_minY)
	{
		return (PointBoundsError("BuildIndex"));
	} // if
			
	// fix a cell size and number of cells in X and Y to begin the process of indexing
	double XRatio = m_rangeX >= m_rangeY ? 1.0: m_rangeX / m_rangeY;
	double YRatio = m_rangeY >= m_rangeX ? 1.0: m_rangeY / m_rangeX;
	
	m_totalCells = m_pointRecordsCount / 100;
	m_totalCells = static_cast<uint32_t>(sqrt((double)m_totalCells));
	if (m_totalCells < 10)
		m_totalCells = 10;	// let's set a minimum number of cells to make the effort worthwhile
	m_cellsX = static_cast<uint32_t>(XRatio * m_totalCells);
	m_cellsY = static_cast<uint32_t>(YRatio * m_totalCells);
	if (m_cellsX < 1)
		m_cellsX = 1;
	if (m_cellsY < 1)
		m_cellsY = 1;
	m_totalCells = m_cellsX * m_cellsY;
	if (m_totalCells > MaximumCells)
	{
		double CellReductionRatio = (double)MaximumCells / (double)m_totalCells;
		CellReductionRatio = sqrt(CellReductionRatio);
		m_cellsX = static_cast<uint32_t>(m_cellsX * CellReductionRatio);
		m_cellsY = static_cast<uint32_t>(m_cellsY * CellReductionRatio);
		m_totalCells = m_cellsX * m_cellsY;
	} // if
	m_cellSizeX = m_rangeX / m_cellsX;
	m_cellSizeY = m_rangeY / m_cellsY;

	// print some statistics to the console
	if (m_debugOutputLevel > 1)
	{
		fprintf(m_debugger, "Points in file %d, Cell matrix x %d, y %d, z %d\n", m_pointRecordsCount, m_cellsX, m_cellsY,
			m_cellsZ);
		fprintf(m_debugger, "Point ranges x %.2f-%.2f, y %.2f-%.2f, z %.2f-%.2f, z range %.2f\n", m_minX, m_maxX, m_minY, m_maxY, 
			m_minZ, m_maxZ, m_rangeZ);
	} // if
	
	// now we know how large our index array is going to be
	// we'll create a vector of that many entities
	
	try {
		// a one dimensional array to represent cell matrix
		IndexCellRow IndexCellsY(m_cellsY);
		// a two dimensional array
		IndexCellDataBlock IndexCellBlock(m_cellsX, IndexCellsY);
		liblas::detail::IndexOutput IndexOut(this);
		
		// for Z bounds debugging
		uint32_t ZRangeSum = 0;
		uint32_t PointSum = 0;
		// read each point in the file
		// figure out what cell in X and Y
		// test to see if it is the same as the last cell
		uint32_t LastCellX = static_cast<uint32_t>(~0), LastCellY = static_cast<uint32_t>(~0);
		liblas::detail::ElevRange ZRange;
		uint32_t PointID = 0;
		uint32_t LastPointID = 0;
		uint32_t PtsIndexed = 0;
		uint32_t PointsInMemory = 0, MaxPointsInMemory;
		MaxPointsInMemory = m_maxMemoryUsage / sizeof(liblas::detail::IndexCell);
		// ReadNextPoint() throws a std::out_of_range error when it hits end of range so don't 
		// get excited when you see it in the debug output
		while (m_reader->ReadNextPoint())
		{
			uint32_t CurCellX, CurCellY;
			// analyze the point to determine its cell ID
			Point const& CurPt = m_reader->GetPoint();
			if (IdentifyCell(CurPt, CurCellX, CurCellY))
			{
				// if same cell as last point, attempt to increment the count of consecutive points for the cell
				// otherwise add a new point, first checking to see if the memory allocated to this process is full
				if (! (CurCellX == LastCellX && CurCellY == LastCellY &&
					IndexCellBlock[CurCellX][CurCellY].IncrementPointRecord(LastPointID)))
				{
					// if memory allocated to this process is full, write all the point data to a temp file
					if (m_tempFileName.size() && PointsInMemory >= MaxPointsInMemory)
					{
						if (! PurgePointsToTempFile(IndexCellBlock))
							return (FileError("BuildIndex"));
						PointsInMemory = 0;
					} // if
					IndexCellBlock[CurCellX][CurCellY].AddPointRecord(PointID);
					LastPointID = PointID;
					LastCellX = CurCellX;
					LastCellY = CurCellY;
					++PointsInMemory;
				} // else
			// update Z cell bounds
			IndexCellBlock[CurCellX][CurCellY].UpdateZBounds(CurPt.GetZ());
			} // if
		++PointID;
		} // while
		// write remaining points to temp file
		if (m_tempFileName.size())
		{
			if (! PurgePointsToTempFile(IndexCellBlock))
				return (FileError("BuildIndex"));
		} // if using temp file

		// print some statistics to the console
		if (m_debugOutputLevel > 2)
		{
			if (! OutputCellStats(IndexCellBlock))
			{
				return (DebugOutputError("BuildIndex"));
			} // if
		} // if

		// Here's where it gets fun
		// Read the binned data from the temp file, one cell at a time
		// Store the data in Variable records section of the LAS file
		// If a cell contains too many points, subdivide the cell and save sub-cells within the cell structure
		// If Z-binning is desired, define the bounds of each Z zone and subdivide sort each cell's points into Z bins
		// Save Z bins within the cell structure.
		// reset Reader to beginning of point data records in case points had been examined before index is built
		
		if (IndexOut.InitiateOutput())
		{
			m_reader->Reset();
			for (uint32_t x = 0; x < m_cellsX; ++x)
			{
				for (uint32_t y = 0; y < m_cellsY; ++y)
				{
					if (m_debugOutputLevel > 3)
						fprintf(m_debugger, "reloading %d %d\n", x, y);
					if (LoadCellFromTempFile(&IndexCellBlock[x][y], x, y))
					{
						ZRange = IndexCellBlock[x][y].GetZRange();
						// if Z-binning is specified, create Z sub-cells first
						if ((m_cellsZ > 1 && ZRange > m_cellSizeZ) || IndexCellBlock[x][y].GetNumPoints() > 1000)
						{
							// walk the points in this cell and divvy them up into Z - cells or quadtree cells
							// create an iterator for the map
							// walk the map entities
							liblas::detail::IndexCellData::iterator MapIt = IndexCellBlock[x][y].GetFirstRecord();
							for (; MapIt != IndexCellBlock[x][y].GetEnd(); ++MapIt)
							{
								// get the actual point from the las file
								if (m_reader->ReadPointAt(MapIt->first))
								{
									uint32_t FirstPt = 0, LastCellZ = static_cast<uint32_t>(~0);
									uint32_t LastSubCell = static_cast<uint32_t>(~0);
									for (liblas::detail::ConsecPtAccumulator PtsTested = 0; PtsTested < MapIt->second; )
									{
										Point const& CurPt = m_reader->GetPoint();
										if (m_cellsZ > 1 && ZRange > m_cellSizeZ)
										{
											// for the number of consecutive points, identify the Z cell
											uint32_t CurCellZ;
											if (IdentifyCellZ(CurPt, CurCellZ))
											{
												// add a record to the z cell chain or increment existing record
												if (! (CurCellZ == LastCellZ && IndexCellBlock[x][y].IncrementZCell(CurCellZ, FirstPt)))
												{
													FirstPt = MapIt->first + PtsTested;
													IndexCellBlock[x][y].AddZCell(CurCellZ, FirstPt);
													LastCellZ = CurCellZ;
												} // else
											} // if
										} // if
										else
										{
											uint32_t CurSubCell;
											// for the number of consecutive points, identify the sub cell in a 2x2 matrix
											// 0 is lower left, 1 is lower right
											if (IdentifySubCell(CurPt, x, y, CurSubCell))
											{
												// add a record to the sub cell chain or increment existing record
												if (! (CurSubCell == LastSubCell && IndexCellBlock[x][y].IncrementSubCell(CurSubCell, FirstPt)))
												{
													FirstPt = MapIt->first + PtsTested;
													IndexCellBlock[x][y].AddSubCell(CurSubCell, FirstPt);
													LastSubCell = CurSubCell;
												} // else
											} // if
										} // else
										++PtsTested;
										if (PtsTested < MapIt->second)
										{
											if (! m_reader->ReadNextPoint())
												return (FileError("BuildIndex"));
										} // if
									} // for
								} // if
								else
									return (FileError("BuildIndex"));
							} // for
							IndexCellBlock[x][y].RemoveMainRecords();
						} // if
						// write the cell out to permanent file VLR
						PtsIndexed += IndexCellBlock[x][y].GetNumPoints();

						if (! IndexOut.OutputCell(&IndexCellBlock[x][y], x, y))
							return (FileError("BuildIndex"));

						// some statistical stuff for z bounds debugging
						ZRangeSum += ZRange;
						++PointSum;
						
						// purge the memory for this cell
						IndexCellBlock[x][y].RemoveAllRecords();
					} // if
					else
					{
						return (FileError("BuildIndex"));
					}  // else
				} // for y
			} // for x
			if (! IndexOut.FinalizeOutput())
				return (FileError("BuildIndex"));
			if (m_debugOutputLevel)
			{
				if (PtsIndexed < m_pointRecordsCount)
				{
				fprintf(m_debugger, "%d of %d points in las file were indexed.\n", PtsIndexed, m_pointRecordsCount);
				} // if
			} // if
			if (m_debugOutputLevel > 2 && PointSum)
			{
				ZRangeSum /= PointSum;
				fprintf(m_debugger, "Z range average per cell %d\n", ZRangeSum);
			} // if
			if (m_ofs)
			{
				// resave the entire file with new index VLR's
				if (! SaveIndexInLASFile())
					return false;
			} // if
			else
			{
				// save a standalone index file, whatever that should look like
				if (! SaveIndexInStandAloneFile())
					return false;
			} // else
		} // if
	} // try
	catch (std::bad_alloc) {
		CloseTempFile();
		return (MemoryError("BuildIndex"));
	} // catch
	
	return true;

} // Index::BuildIndex

bool Index::IdentifyCell(Point const& CurPt, uint32_t& CurCellX, uint32_t& CurCellY) const
{
	double OffsetX, OffsetY;

	OffsetX = (CurPt.GetX() - m_minX) / m_rangeX;
	if (OffsetX >= 0 && OffsetX < 1.0)
		CurCellX = static_cast<uint32_t>(OffsetX * m_cellsX);
	else if (OffsetX == 1.0)
		CurCellX = m_cellsX - 1;
	else
	{
		return (PointBoundsError("IdentifyCell"));
	} // else
	
	OffsetY = (CurPt.GetY() - m_minY) / m_rangeY;
	if (OffsetY >= 0 && OffsetY < 1.0)
		CurCellY = static_cast<uint32_t>(OffsetY * m_cellsY);
	else if (OffsetY == 1.0)
		CurCellY = m_cellsY - 1;
	else
	{
		return (PointBoundsError("IdentifyCell"));
	} // else
	
	return true;

} // Index::IdentifyCell

bool Index::IdentifyCellZ(Point const& CurPt, uint32_t& CurCellZ) const
{
	double OffsetZ;

	OffsetZ = (CurPt.GetZ() - m_minZ) / m_rangeZ;
	if (OffsetZ >= 0 && OffsetZ < 1.0)
		CurCellZ = static_cast<uint32_t>(OffsetZ * m_cellsZ);
	else if (OffsetZ == 1.0)
		CurCellZ = m_cellsZ - 1;
	else
	{
		return (PointBoundsError("IdentifyCellZ"));
	} // else

	return true;

} // Index::IdentifyCellZ

bool Index::IdentifySubCell(Point const& CurPt, uint32_t x, uint32_t y, uint32_t& CurSubCell) const
{
	double Offset, CellMinX, CellMinY;

	CellMinX = x * m_cellSizeX + m_minX;
	CellMinY = y * m_cellSizeY + m_minY;
	// find point position in X
	Offset = (CurPt.GetX() - CellMinX) / m_cellSizeX;
	if (Offset > .5)	//upper half X
	{
		// find point position in Y
		Offset = (CurPt.GetY() - CellMinY) / m_cellSizeY;
		if (Offset > .5)
			CurSubCell = 3;	// upper half of Y, NE
		else
			CurSubCell = 1;	// lower half of Y, SE
	} // if
	else	// lower half X
	{
		// find point position in Y
		Offset = (CurPt.GetY() - CellMinY) / m_cellSizeY;
		if (Offset > .5)
			CurSubCell = 2;	// upper half of Y, NW
		else
			CurSubCell = 0;	// lower half of Y, SW
	} // else

	return true;

} // Index::IdentifyCellZ

bool Index::PurgePointsToTempFile(IndexCellDataBlock& CellBlock)
{
	if (m_tempFile || OpenTempFile())
	{
		uint32_t EmptyOffset = 0;	// this might not be large enough
		
		if (! m_tempFileStarted)
		{
			// there is some setup of the temp file to be done first
			// write out a block of file offsets the size of the number of cells
			for (uint32_t i = 0; i < m_totalCells; ++i)
			{
				if (fwrite(&EmptyOffset, sizeof(uint32_t), 1, m_tempFile) < 1)
				{
					return (FileError("PurgePointsToTempFile"));
				} // if error
			} // for
			m_tempFileWrittenBytes = m_totalCells * sizeof(uint32_t);
			m_tempFileStarted = true;
		} // if
		for (uint32_t x = 0; x < m_cellsX; ++x)
		{
			for (uint32_t y = 0; y < m_cellsY; ++y)
			{
				uint32_t RecordsToWrite = CellBlock[x][y].GetNumRecords();
				if (RecordsToWrite)
				{
					// write the current file location in the cell block header
					// if cell block header is 0 write the current file location in the file header
					// otherwise write the current file location at the file location specified in the 
					// cell block header
					uint32_t LastWriteLocation = CellBlock[x][y].GetFileOffset();
					if (LastWriteLocation == 0)
						LastWriteLocation = (x * m_cellsY + y) * sizeof(uint32_t);
					fseek(m_tempFile, LastWriteLocation, SEEK_SET);
					if (fwrite(&m_tempFileWrittenBytes, sizeof(uint32_t), 1, m_tempFile) < 1)
						return (FileError("PurgePointsToTempFile"));
					CellBlock[x][y].SetFileOffset(m_tempFileWrittenBytes);

					fseek(m_tempFile, 0, SEEK_END);
					uint32_t FilePos = ftell(m_tempFile);
					if (FilePos < 19600)
						printf("file position error");
					// write a blank space for later placement of next file block for this cell
					if (fwrite(&EmptyOffset, sizeof(uint32_t), 1, m_tempFile) < 1)
						return (FileError("PurgePointsToTempFile"));
					m_tempFileWrittenBytes += sizeof(uint32_t);
					// write the number of records stored in this section
					if (fwrite(&RecordsToWrite, sizeof(uint32_t), 1, m_tempFile) < 1)
						return (FileError("PurgePointsToTempFile"));
					m_tempFileWrittenBytes += sizeof(uint32_t);

					liblas::detail::IndexCellData::iterator MapIt = CellBlock[x][y].GetFirstRecord();
					for (uint32_t RecordNum = 0; RecordNum < RecordsToWrite && MapIt != CellBlock[x][y].GetEnd(); ++RecordNum, ++MapIt)
					{
						// write the point ID
						uint32_t PointID = MapIt->first;
						// write the number of consecutive points
						liblas::detail::ConsecPtAccumulator ConsecutivePoints = MapIt->second;
						if (fwrite(&PointID, sizeof(uint32_t), 1, m_tempFile) < 1)
							return (FileError("PurgePointsToTempFile"));
						if (fwrite(&ConsecutivePoints, sizeof(liblas::detail::ConsecPtAccumulator), 1, m_tempFile) < 1)
							return (FileError("PurgePointsToTempFile"));
						m_tempFileWrittenBytes += sizeof(uint32_t);
						m_tempFileWrittenBytes += sizeof(liblas::detail::ConsecPtAccumulator);
					} // for
					// purge the records for this cell from active memory
					CellBlock[x][y].RemoveMainRecords();
				} // if
			} // for y
		} // for x
		// necessary for subsequent reads in case fseek isn't called first
		fflush(m_tempFile);
		return true;
	} // if file

	return (FileError("PurgePointsToTempFile"));

} // Index::PurgePointsToTempFile

bool Index::LoadCellFromTempFile(liblas::detail::IndexCell *CellBlock, 
	uint32_t CurCellX, uint32_t CurCellY)
{

	uint32_t FileOffset, RecordsToRead, FormerNumPts, NewNumPts = 0;
	
	FormerNumPts = CellBlock->GetNumPoints();
	CellBlock->SetNumPoints(0);
	
	// load the cell as it was written
	// read the first offset for this cell
	if (fseek(m_tempFile, (CurCellX * m_cellsY + CurCellY) * sizeof (uint32_t), SEEK_SET))
		return (FileError("LoadCellFromTempFile"));
	if (fread(&FileOffset, sizeof (uint32_t), 1, m_tempFile) < 1)
		return (FileError("LoadCellFromTempFile"));
	while (FileOffset > 0)
	{
		// jump to the first block for this cell, read the next offset
		if (fseek(m_tempFile, FileOffset, SEEK_SET))
			return (FileError("LoadCellFromTempFile"));
		if (fread(&FileOffset, sizeof (uint32_t), 1, m_tempFile) < 1)
			return (FileError("LoadCellFromTempFile"));
		// read the data for the cell in this block
		// first is the number of items to read now
		if (fread(&RecordsToRead, sizeof (uint32_t), 1, m_tempFile) < 1)
			return (FileError("LoadCellFromTempFile"));
		for (uint32_t RecordNum = 0; RecordNum < RecordsToRead; ++RecordNum)
		{
			uint32_t PointID;
			liblas::detail::ConsecPtAccumulator ConsecutivePoints;
			// read the point ID
			if (fread(&PointID, sizeof(uint32_t), 1, m_tempFile) < 1)
				return (FileError("LoadCellFromTempFile"));
			// read the number of consecutive points
			if (fread(&ConsecutivePoints, sizeof(liblas::detail::ConsecPtAccumulator), 1, m_tempFile) < 1)
				return (FileError("LoadCellFromTempFile"));
			CellBlock->AddPointRecord(PointID, ConsecutivePoints);
		} // for
	} // while
	// check to see that we got the number of points back that we started with
	NewNumPts = CellBlock->GetNumPoints();
	if (NewNumPts != FormerNumPts)
	{
		CloseTempFile();
		return (PointCountError("LoadCellFromTempFile"));
	} // if
	return (true);

} // Index::LoadCellFromTempFile

FILE *Index::OpenTempFile(void)
{

	m_tempFileStarted = 0;
	m_tempFileWrittenBytes = 0;
	return (m_tempFile = fopen(m_tempFileName.c_str(), "wb+"));
    
} // Index::OpenTempFile

void Index::CloseTempFile(void)
{

	if (m_tempFile)
		fclose(m_tempFile);
	m_tempFile = 0;
	m_tempFileWrittenBytes = 0;
    
} // Index::CloseTempFile

bool Index::SaveIndexInLASFile(void)
{
	try {
		Writer writer(*m_ofs, m_header);
		m_reader->Reset();
		while (m_reader->ReadNextPoint())
		{
			Point CurPt = m_reader->GetPoint();
			if (! writer.WritePoint(CurPt))
				return (OutputFileError("SaveIndexInLASFile"));
		} // while
	} // try
	catch (std::runtime_error) {
		return (OutputFileError("SaveIndexInLASFile"));
	} // catch
	return true;
} // Index::SaveIndexInLASFile

bool Index::SaveIndexInStandAloneFile(void)
{
	try {
		if (OpenOutputFile())
		{
			CloseOutputFile();
		} // if
		else
			return (OutputFileError("SaveIndexInLASFile"));
	} // try
	catch (std::runtime_error) {
		return (OutputFileError("SaveIndexInLASFile"));
	} // catch
	return true;
} // Index::SaveIndexInStandAloneFile

bool Index::FileError(const char *Reporter)
{

	CloseTempFile();
	if (m_debugOutputLevel)
		fprintf(m_debugger, "File i/o error, %s\n", Reporter);
	return false;

} // Index::FileError

bool Index::OutputFileError(const char *Reporter) const
{

	if (m_debugOutputLevel)
		fprintf(m_debugger, "Output file i/o error, %s\n", Reporter);
	return false;

} // Index::OutputFileError

bool Index::DebugOutputError(const char *Reporter) const
{

	if (m_debugOutputLevel)
		fprintf(m_debugger, "Debug output error, %s\n", Reporter);
	return false;

} // Index::DebugOutputError

bool Index::PointCountError(const char *Reporter) const
{

	if (m_debugOutputLevel)
		fprintf(m_debugger, "Point checksum error, %s\n", Reporter);
	return false;

} // Index::PointCountError

bool Index::PointBoundsError(const char *Reporter) const
{

	if (m_debugOutputLevel)
		fprintf(m_debugger, "Point out of bounds error, %s\n", Reporter);
	return false;

} // Index::PointBoundsError

bool Index::MemoryError(const char *Reporter) const
{

	if (m_debugOutputLevel)
		fprintf(m_debugger, "Memory error, %s\n", Reporter);
	return false;

} // Index::MemoryError

bool Index::InitError(const char *Reporter) const
{

	if (m_debugOutputLevel)
		fprintf(m_debugger, "Index creation failure, %s\n", Reporter);
	return false;

} // Index::InitError

#define LIBLAS_INDEX_DEBUGCELLBINS 20

bool Index::OutputCellStats(IndexCellDataBlock& CellBlock) const
{
	uint32_t MaxPointsPerCell = 0;

	for (uint32_t x = 0; x < m_cellsX; ++x)
	{
		for (uint32_t y = 0; y < m_cellsY; ++y)
		{
			uint32_t PointsThisCell = CellBlock[x][y].GetNumPoints();
			if (PointsThisCell > MaxPointsPerCell)
				MaxPointsPerCell = PointsThisCell;
		} // for
	} // for
	
	std::vector<uint32_t> CellPopulation(LIBLAS_INDEX_DEBUGCELLBINS);

	for (uint32_t x = 0; x < m_cellsX; ++x)
	{
		for (uint32_t y = 0; y < m_cellsY; ++y)
		{
			uint32_t PointsThisCell = CellBlock[x][y].GetNumPoints();
			uint32_t BinThisCell = (uint32_t )(LIBLAS_INDEX_DEBUGCELLBINS * (double)PointsThisCell / (double)MaxPointsPerCell);
			if (BinThisCell >= LIBLAS_INDEX_DEBUGCELLBINS)
				BinThisCell = LIBLAS_INDEX_DEBUGCELLBINS - 1;
			++CellPopulation[BinThisCell];
		} // for
	} // for
	
	fprintf(m_debugger, "Max points per cell %d\n", MaxPointsPerCell);
	OutputCellGraph(CellPopulation, MaxPointsPerCell);
	// no way for this to fail at this time but allow for future modifications by having a return value
	return true;
	
} // Index::OutputCellStats

bool Index::OutputCellGraph(std::vector<uint32_t> CellPopulation, uint32_t MaxPointsPerCell) const
{
	
	for (uint32_t i = 0; i < CellPopulation.size(); ++i)
	{
		fprintf(m_debugger,"Bin %2d (%4d-%4d)... Cells in point range bin %d\n", i, (i * MaxPointsPerCell / LIBLAS_INDEX_DEBUGCELLBINS),
			((i + 1)* MaxPointsPerCell / LIBLAS_INDEX_DEBUGCELLBINS), CellPopulation[i]);
	} // for
	// no way for this to fail at this time but allow for future modifications by having a return value
	return true;
	
} // Index::OutputCellGraph

FILE *Index::OpenOutputFile(void)
{
	return (m_outputFile = fopen(m_outFileName.c_str(), "wb+"));
} // Index::OpenOutputFile

void Index::CloseOutputFile(void)
{
	fclose(m_outputFile);
} // Index::CloseOutputFile

const char *Index::GetIndexAuthorStr(void) const
{
	return (m_indexAuthor.c_str());
} // Index::GetIndexAuthorStr

const char *Index::GetIndexCommentStr(void) const
{
	return (m_indexComment.c_str());
} // Index::GetIndexAuthorStr

const char *Index::GetIndexDateStr(void) const
{
	return (m_indexDate.c_str());
} // Index::GetIndexAuthorStr

IndexData::IndexData(void)
{
	SetValues();
} // IndexData::IndexData

IndexData::IndexData(Index const& index)
{
	SetValues();
	m_reader = index.GetReader();
    m_debugOutputLevel = index.GetDebugOutputLevel();
	m_tempFileName = index.GetTempFileName() ? index.GetTempFileName(): "";
	m_outFileName = index.GetOutFileName() ? index.GetOutFileName(): "";
	m_indexAuthor = index.GetIndexAuthorStr() ? index.GetIndexAuthorStr(): "";
	m_indexComment = index.GetIndexCommentStr() ? index.GetIndexCommentStr(): "";
	m_indexDate = index.GetIndexDateStr() ? index.GetIndexDateStr(): "";
	m_cellSizeZ = index.GetCellSizeZ();
	m_debugger = index.GetDebugger() ? index.GetDebugger(): stderr;
	m_readOnly = index.GetReadOnly();
	m_forceNewIndex = index.GetForceNewIndex();
	if (index.GetMaxMemoryUsage() > 0)
		m_maxMemoryUsage = index.GetMaxMemoryUsage();
	else
		m_maxMemoryUsage = LIBLAS_INDEX_MAXMEMDEFAULT;
	m_indexValid = index.IndexReady();
} // Index::Index

void IndexData::SetValues(void)
{
	m_reader = 0;
	m_ifs = 0;
	m_ofs = 0;
	m_tempFileName = 0;
	m_outFileName = 0;
	m_indexAuthor = 0;
	m_indexComment = 0;
	m_indexDate = 0;
	m_cellSizeZ = 0.0;
	m_maxMemoryUsage = 0;
	m_debugOutputLevel = 0;
	m_readOnly = false;
	m_forceNewIndex = false;
	m_debugger = 0;
	m_indexValid = false;
	m_noFilterX = m_noFilterY = m_noFilterZ = false;
	m_lowFilterX = m_highFilterX = m_lowFilterY = m_highFilterY = m_lowFilterZ = m_highFilterZ = 0.0;
} // IndexData::SetValues

bool IndexData::SetInitialValues(std::istream *ifs, Reader *reader, std::ostream *ofs,
	const char *tmpfilenme, const char *outfilenme, const char *indexauthor, 
	const char *indexcomment, const char *indexdate, double zbinht, 
	uint32_t maxmem, int debugoutputlevel, bool readonly, bool forcenewindex, FILE *debugger)
{

	m_ifs = ifs;
	m_ofs = ofs;
	m_reader = reader;
	m_tempFileName = tmpfilenme;
	m_outFileName = outfilenme;
	m_indexAuthor = indexauthor;
	m_indexComment = indexcomment;
	m_indexDate = indexdate;
	m_cellSizeZ = zbinht;
	m_maxMemoryUsage = maxmem;
	m_debugOutputLevel = debugoutputlevel;
	m_readOnly = readonly;
	m_forceNewIndex = forcenewindex;
	m_debugger = debugger;
	m_indexValid = false;
	return (m_reader || m_ifs);
	
} // IndexData::SetInitialValues

bool IndexData::SetFilterValues(double LowFilterX, double HighFilterX, double LowFilterY, double HighFilterY, 
	double LowFilterZ, double HighFilterZ)
{
	m_lowFilterX = LowFilterX;
	m_highFilterX = HighFilterX;
	m_lowFilterY = LowFilterY;
	m_highFilterY = HighFilterY;
	m_lowFilterZ = LowFilterZ;
	m_highFilterZ = HighFilterZ;
	if (m_lowFilterX > HighFilterX)
		std::swap(m_lowFilterX, HighFilterX);
	else if (m_lowFilterX == HighFilterX)
		m_noFilterX = true;
	if (m_lowFilterY > HighFilterY)
		std::swap(m_lowFilterY, HighFilterY);
	else if (m_lowFilterY == HighFilterY)
		m_noFilterY = true;
	if (m_lowFilterZ > HighFilterZ)
		std::swap(m_lowFilterZ, HighFilterZ);
	else if (m_lowFilterZ == HighFilterZ)
		m_noFilterZ = true;
	return (! (m_noFilterX && m_noFilterY && m_noFilterZ));
	
} // IndexData::SetFilterValues

} // namespace liblas

