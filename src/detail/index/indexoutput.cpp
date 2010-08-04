/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  index output implementation for C++ libLAS 
 * Author:   Gary Huber, gary@garyhuberart.com
 *
 ******************************************************************************
 *
 * (C) Copyright Gary Huber 2010, gary@garyhuberart.com
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

#include <liblas/detail/index/indexoutput.hpp>
#include <limits>

using namespace std;

namespace liblas { namespace detail {

IndexOutput::IndexOutput(liblas::Index *indexsource) :
	m_index(indexsource), 
	m_VLRCommonDataSize(5 * sizeof(uint32_t)),
	m_VLRDataSizeLocation(4 * sizeof(uint32_t)),
	m_FirstCellLocation(0),
	m_LastCellLocation(sizeof(uint32_t) * 2)
{
} // IndexOutput::IndexOutput

bool IndexOutput::InitiateOutput(void)
{

	uint8_t VersionMajor = LIBLAS_INDEX_VERSIONMAJOR, VersionMinor = LIBLAS_INDEX_VERSIONMINOR;
	char DestStr[512];
	uint16_t StringLen;
	uint16_t WritePos = 0;
	
	try {
		// write a header in standard VLR format
		m_indexVLRHeaderData.resize(16000);
		m_indexVLRHeaderRecord.SetUserId("liblas");
		m_indexVLRHeaderRecord.SetRecordId(42);
		m_indexVLRHeaderRecord.SetDescription("LASLIB Index Header");
		// set the header data into the header data string
		// Index file version
		memcpy(&m_indexVLRHeaderData[WritePos], &VersionMajor, sizeof(uint8_t));
		WritePos += sizeof(uint8_t);
		memcpy(&m_indexVLRHeaderData[WritePos], &VersionMinor, sizeof(uint8_t));
		WritePos += sizeof(uint8_t);
		// creator		
		strcpy(DestStr, m_index->GetIndexAuthorStr());
		StringLen = static_cast<uint16_t>(strlen(DestStr) + 1);
		memcpy(&m_indexVLRHeaderData[WritePos], &StringLen, sizeof(uint16_t));
		WritePos += sizeof(uint16_t);
		memcpy(&m_indexVLRHeaderData[WritePos], DestStr, StringLen);
		WritePos = WritePos + StringLen;
		// comment
		strcpy(DestStr, m_index->GetIndexCommentStr());
		StringLen = static_cast<uint16_t>(strlen(DestStr) + 1);
		memcpy(&m_indexVLRHeaderData[WritePos], &StringLen, sizeof(uint16_t));
		WritePos += sizeof(uint16_t);
		memcpy(&m_indexVLRHeaderData[WritePos], DestStr, StringLen);
		WritePos = WritePos + StringLen;
		// date	
		strcpy(DestStr, m_index->GetIndexDateStr());
		StringLen = static_cast<uint16_t>(strlen(DestStr) + 1);
		memcpy(&m_indexVLRHeaderData[WritePos], &StringLen, sizeof(uint16_t));
		WritePos += sizeof(uint16_t);
		memcpy(&m_indexVLRHeaderData[WritePos], DestStr, StringLen);
		WritePos = WritePos + StringLen;

		// file index extents
		double TempData = m_index->GetMinX();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempData, sizeof(double));
		WritePos += sizeof(double);
		TempData = m_index->GetMaxX();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempData, sizeof(double));
		WritePos += sizeof(double);
		TempData = m_index->GetMinY();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempData, sizeof(double));
		WritePos += sizeof(double);
		TempData = m_index->GetMaxY();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempData, sizeof(double));
		WritePos += sizeof(double);
		TempData = m_index->GetMinZ();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempData, sizeof(double));
		WritePos += sizeof(double);
		TempData = m_index->GetMaxZ();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempData, sizeof(double));
		WritePos += sizeof(double);

		// ID number of associated data VLR's - normally 43 but may use heigher numbers
		// in order to store more than one index in a file
		uint32_t TempLong = m_index->GetDataVLR_ID();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempLong, sizeof(uint32_t));
		WritePos += sizeof(uint32_t);
		
		// index cell matrix and number of points
		memcpy(&m_indexVLRHeaderData[WritePos], &TempLong, sizeof(uint32_t));
		WritePos += sizeof(uint32_t);
		TempLong = m_index->GetCellsX();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempLong, sizeof(uint32_t));
		WritePos += sizeof(uint32_t);
		TempLong = m_index->GetCellsY();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempLong, sizeof(uint32_t));
		WritePos += sizeof(uint32_t);
		TempLong = m_index->GetCellsZ();
		memcpy(&m_indexVLRHeaderData[WritePos], &TempLong, sizeof(uint32_t));
		WritePos += sizeof(uint32_t);
		
		// record length		
		m_indexVLRHeaderRecord.SetRecordLength(WritePos);
		m_indexVLRHeaderData.resize(WritePos);
		m_indexVLRHeaderRecord.SetData(m_indexVLRHeaderData);
		m_index->GetHeader()->AddVLR(m_indexVLRHeaderRecord);

		// initialize VLR data members
		m_FirstCellInVLR = true;
		m_indexVLRCellRecord.SetUserId("liblas");
		m_indexVLRCellRecord.SetRecordId(43);
		m_indexVLRCellRecord.SetDescription("LASLIB Index Data");
		
		return true;
	}
	catch (std::bad_alloc) {
		return false;
	}

} // IndexOutput::InitiateOutput

bool IndexOutput::OutputCell(liblas::detail::IndexCell *CellBlock, uint32_t x, uint32_t y)
{

	if (m_FirstCellInVLR)
	{
		if (! InitializeVLRData(x, y))
			return false;
		m_indexVLRTempData.resize(numeric_limits<unsigned short>::max());
		m_TempWritePos = 0;
	} // if
	// data output for one cell may result in a partial VLR or more than one VLR depending on the number
	// of point records for the cell.
	// Partially filled VLR's are only flushed when the next cell would overflow the VLR size limit of USHRT_MAX
	// or on the last cell in the index cell block
	uint32_t SubCellsXY, SubCellsZ, NumPts, PtRecords;

	if (NumPts = CellBlock->GetNumPoints())
	{
		// current cell, x, y
		memcpy(&m_indexVLRTempData[m_TempWritePos], &x, sizeof(uint32_t));
		m_TempWritePos += sizeof(uint32_t);
		memcpy(&m_indexVLRTempData[m_TempWritePos], &y, sizeof(uint32_t));
		m_TempWritePos += sizeof(uint32_t);
		// min and max Z
		ElevExtrema CellZ = CellBlock->GetMinZ();
		memcpy(&m_indexVLRTempData[m_TempWritePos], &CellZ, sizeof(ElevExtrema));
		m_TempWritePos += sizeof(ElevExtrema);
		CellZ = CellBlock->GetMaxZ();
		memcpy(&m_indexVLRTempData[m_TempWritePos], &CellZ, sizeof(ElevExtrema));
		m_TempWritePos += sizeof(ElevExtrema);
		
		// number of subcells in this cell in both XY and Z
		PtRecords = CellBlock->GetNumRecords();
		SubCellsXY = CellBlock->GetNumSubCellRecords();
		SubCellsZ = CellBlock->GetNumZCellRecords();
		memcpy(&m_indexVLRTempData[m_TempWritePos], &PtRecords, sizeof(uint32_t));
		m_TempWritePos += sizeof(uint32_t);
		memcpy(&m_indexVLRTempData[m_TempWritePos], &SubCellsXY, sizeof(uint32_t));
		m_TempWritePos += sizeof(uint32_t);
		memcpy(&m_indexVLRTempData[m_TempWritePos], &SubCellsZ, sizeof(uint32_t));
		m_TempWritePos += sizeof(uint32_t);

		// <<<>>> prevent array overruns
		// compile data into one long vector m_indexVLRTempData
		if (SubCellsZ)
		{
			for (liblas::detail::IndexSubCellData::iterator MyCellIt = CellBlock->GetFirstZCellRecord();
				MyCellIt != CellBlock->GetEndZCell(); ++MyCellIt)
			{
				// subcell number
				uint32_t ZCellID = MyCellIt->first;
				memcpy(&m_indexVLRTempData[m_TempWritePos], &ZCellID, sizeof(uint32_t));
				m_TempWritePos += sizeof(uint32_t);
				// number of point records in subcell
				uint32_t ZCellNumPts = static_cast<uint32_t>(MyCellIt->second.size());
				memcpy(&m_indexVLRTempData[m_TempWritePos], &ZCellNumPts, sizeof(uint32_t));
				m_TempWritePos += sizeof(uint32_t);
				for (liblas::detail::IndexCellData::iterator MyPointIt = MyCellIt->second.begin();
					MyPointIt != MyCellIt->second.end(); ++MyPointIt)
				{
					uint32_t PointID = MyPointIt->first;
					memcpy(&m_indexVLRTempData[m_TempWritePos], &PointID, sizeof(uint32_t));
					m_TempWritePos += sizeof(uint32_t);
					uint8_t ConsecutivePts = MyPointIt->second;
					memcpy(&m_indexVLRTempData[m_TempWritePos], &ConsecutivePts, sizeof(uint8_t));
					m_TempWritePos += sizeof(uint8_t);
				} // for
			} // for
		} // if
		if (SubCellsXY)
		{
			for (liblas::detail::IndexSubCellData::iterator MyCellIt = CellBlock->GetFirstSubCellRecord();
				MyCellIt != CellBlock->GetEndSubCell(); ++MyCellIt)
			{
				// subcell number
				uint32_t SubCellID = MyCellIt->first;
				memcpy(&m_indexVLRTempData[m_TempWritePos], &SubCellID, sizeof(uint32_t));
				m_TempWritePos += sizeof(uint32_t);
				// number of point records in subcell
				uint32_t SubCellNumPts = static_cast<uint32_t>(MyCellIt->second.size());
				memcpy(&m_indexVLRTempData[m_TempWritePos], &SubCellNumPts, sizeof(uint32_t));
				m_TempWritePos += sizeof(uint32_t);
				for (liblas::detail::IndexCellData::iterator MyPointIt = MyCellIt->second.begin();
					MyPointIt != MyCellIt->second.end(); ++MyPointIt)
				{
					uint32_t PointID = MyPointIt->first;
					memcpy(&m_indexVLRTempData[m_TempWritePos], &PointID, sizeof(uint32_t));
					m_TempWritePos += sizeof(uint32_t);
					uint8_t ConsecutivePts = MyPointIt->second;
					memcpy(&m_indexVLRTempData[m_TempWritePos], &ConsecutivePts, sizeof(uint8_t));
					m_TempWritePos += sizeof(uint8_t);
				} // for
			} // for
		} // if
		if (! (SubCellsZ || SubCellsXY))
		{
			for (liblas::detail::IndexCellData::iterator MyPointIt = CellBlock->GetFirstRecord();
				MyPointIt != CellBlock->GetEnd(); ++MyPointIt)
			{
				uint32_t PointID = MyPointIt->first;
				memcpy(&m_indexVLRTempData[m_TempWritePos], &PointID, sizeof(uint32_t));
				m_TempWritePos += sizeof(uint32_t);
				uint8_t ConsecutivePts = MyPointIt->second;
				memcpy(&m_indexVLRTempData[m_TempWritePos], &ConsecutivePts, sizeof(uint8_t));
				m_TempWritePos += sizeof(uint8_t);
			} // for
		} // if

		// copy data to VLR
		// if new cell data causes VLR data to exceed limit add VLR to header VLR list and start new VLR
		if (m_SomeDataReadyToWrite && (m_TempWritePos + m_DataRecordSize > numeric_limits<unsigned short>::max()))
		{
			m_indexVLRCellPointData.resize(m_DataRecordSize);
			m_indexVLRCellRecord.SetRecordLength(static_cast<uint16_t>(m_DataRecordSize));
			m_indexVLRCellRecord.SetData(m_indexVLRCellPointData);
			m_index->GetHeader()->AddVLR(m_indexVLRCellRecord);
			// get set up for next VLR
			if (! InitializeVLRData(x, y))
				return false;
		} // if
		// if size allows, add to VLR cell record data
		if (m_TempWritePos + m_DataRecordSize <= numeric_limits<unsigned short>::max())
		{
			uint32_t WritePos = m_DataRecordSize;
			// update last cell in VLR, x, y
			memcpy(&m_indexVLRCellPointData[m_LastCellLocation], &x, sizeof(uint32_t));
			memcpy(&m_indexVLRCellPointData[m_LastCellLocation + sizeof(uint32_t)], &y, sizeof(uint32_t));
			// update data record size
			m_DataRecordSize += m_TempWritePos;
			memcpy(&m_indexVLRCellPointData[m_VLRDataSizeLocation], &m_DataRecordSize, sizeof(uint32_t));
			memcpy(&m_indexVLRCellPointData[WritePos], &m_indexVLRTempData[0], m_TempWritePos);
			m_SomeDataReadyToWrite = true;
			// rewind counter to start new cell data
			m_TempWritePos = 0;
		} // if
		else
		{
			// unusual situation where one cell's data exceeds the size allowed for a VLR (USHRT_MAX)
			// make as many USHRT_MAX size VLR's until all cell is represented.
			// distinguish the continuation style VLR with the first byte being the total size of the record.
			// When reading a VLR, if the size field is larger than USHRT_MAX you know you have more than one 
			// record to read and concatenate. 
			// WritePos should be at the end of the small VLR common data section
			uint32_t WritePos = m_DataRecordSize;
			// TempWritePos is the size of the data block
			uint32_t UnwrittenBytes = m_TempWritePos + m_DataRecordSize;
			// Make DataRecordSize equal to the total size of the data + common data 
			m_DataRecordSize += m_TempWritePos;
			// write the total size in the common data section
			// resizing shouldn't be necessary
			if (m_indexVLRCellPointData.size() != numeric_limits<unsigned short>::max())
				m_indexVLRCellPointData.resize(numeric_limits<unsigned short>::max());
			memcpy(&m_indexVLRCellPointData[m_VLRDataSizeLocation], &m_DataRecordSize, sizeof(uint32_t));
			
			// write out the part that fits in this VLR (excluding the common data)
			uint32_t WrittenBytes = numeric_limits<unsigned short>::max() - WritePos;
			memcpy(&m_indexVLRCellPointData[WritePos], &m_indexVLRTempData[0], WrittenBytes);
			// add this VLR
			m_indexVLRCellRecord.SetRecordLength(static_cast<uint16_t>(numeric_limits<unsigned short>::max()));
			m_indexVLRCellRecord.SetData(m_indexVLRCellPointData);
			m_index->GetHeader()->AddVLR(m_indexVLRCellRecord);
			
			// subtract the part written
			UnwrittenBytes -= WrittenBytes;
			
			while (UnwrittenBytes > 0)
			{
				uint32_t NewWrittenBytes = (UnwrittenBytes < numeric_limits<unsigned short>::max() ? UnwrittenBytes: numeric_limits<unsigned short>::max());
				memcpy(&m_indexVLRCellPointData[0], &m_indexVLRTempData[WrittenBytes], NewWrittenBytes);
				WrittenBytes += NewWrittenBytes;
				UnwrittenBytes -= NewWrittenBytes;
				m_indexVLRCellRecord.SetRecordLength(static_cast<uint16_t>(NewWrittenBytes));
				m_indexVLRCellRecord.SetData(m_indexVLRCellPointData);
				m_index->GetHeader()->AddVLR(m_indexVLRCellRecord);
			} // while
			// wipe the slate clean for the next cell
			m_SomeDataReadyToWrite = false;
			m_TempWritePos = 0;
			m_FirstCellInVLR = true;
		} // else
	} // if
	
	return true;

} // IndexOutput::OutputCell

bool IndexOutput::InitializeVLRData(uint32_t CurCellX, uint32_t CurCellY)
{

	try {
		m_indexVLRCellPointData.resize(numeric_limits<unsigned short>::max());

		m_DataRecordSize = m_VLRCommonDataSize;
		// 1st cell in VLR, x, y
		memcpy(&m_indexVLRCellPointData[m_FirstCellLocation], &CurCellX, sizeof(uint32_t));
		memcpy(&m_indexVLRCellPointData[m_FirstCellLocation + sizeof(uint32_t)], &CurCellY, sizeof(uint32_t));
		// last cell in VLR, x, y
		memcpy(&m_indexVLRCellPointData[m_LastCellLocation], &CurCellX, sizeof(uint32_t));
		memcpy(&m_indexVLRCellPointData[m_LastCellLocation + sizeof(uint32_t)], &CurCellY, sizeof(uint32_t));
		// data record size
		memcpy(&m_indexVLRCellPointData[m_VLRDataSizeLocation], &m_DataRecordSize, sizeof(uint32_t));
		m_FirstCellInVLR = false;
		m_SomeDataReadyToWrite = false;

		return true;
	}
	catch (std::bad_alloc) {
		return false;
	}
} // IndexOutput::InitializeVLRData

bool IndexOutput::FinalizeOutput(void)
{

	try {
		// copy data to VLR
		// if new cell data causes VLR data to exceed limit add VLR to header VLR list and start new VLR
		if (m_SomeDataReadyToWrite)
		{
			m_indexVLRCellPointData.resize(m_DataRecordSize);
			m_indexVLRCellRecord.SetRecordLength(static_cast<uint16_t>(m_DataRecordSize));
			m_indexVLRCellRecord.SetData(m_indexVLRCellPointData);
			m_index->GetHeader()->AddVLR(m_indexVLRCellRecord);
		} // if
		return true;
	}
	catch (std::bad_alloc) {
		return false;
	}

} // IndexOutput::FinalizeOutput

}} // namespace liblas detail
