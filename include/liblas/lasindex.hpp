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

#ifndef LIBLAS_LASINDEX_HPP_INCLUDED
#define LIBLAS_LASINDEX_HPP_INCLUDED

#include <boost/cstdint.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/detail/index/indexcell.hpp>

// std
#include <stdexcept> // std::out_of_range
#include <cstdio>	// file io
#include <iostream>	// file io
#include <cstdlib> // std::size_t
#include <vector> // std::vector

namespace liblas {

#define LIBLAS_INDEX_MAXMEMDEFAULT	10000000	// 10 megs default
#define LIBLAS_INDEX_VERSIONMAJOR	1
#define LIBLAS_INDEX_VERSIONMINOR	0

typedef std::vector<uint8_t> IndexVLRData;
typedef std::vector<liblas::detail::IndexCell> IndexCellRow;
typedef std::vector<IndexCellRow>	IndexCellDataBlock;

class liblas::detail::IndexCell;
class IndexData;

/// Point data record composed with X, Y, Z coordinates and attributes.
class Index
{
public:
	Index();
	Index(liblas::Reader *reader, std::ostream *ofs = 0, const char *tmpfilenme = 0, const char *outfilenme = 0, const char *indexauthor = 0, 
		const char *indexcomment = 0, const char *indexdate = 0, 
		double zbinht = 0.0, uint32_t maxmem = LIBLAS_INDEX_MAXMEMDEFAULT, int debugoutputlevel = 0, FILE *debugger = 0);
    Index(std::istream *ifs, std::ostream *ofs = 0, const char *tmpfilenme = 0, const char *outfilenme = 0, const char *indexauthor = 0, 
		const char *indexcomment = 0, const char *indexdate = 0, 
		double zbinht = 0.0, uint32_t maxmem = LIBLAS_INDEX_MAXMEMDEFAULT, int debugoutputlevel = 0, FILE *debugger = 0);
    Index(IndexData const& ParamSrc);
	~Index();
	
    // Blocked copying operations, declared but not defined.
    /// Copy constructor.
    Index(Index const& other);
    /// Assignment operator.
    Index& operator=(Index const& rhs);
    
private:

	Reader *m_reader;
	Header m_header;
	bool m_indexBuilt, m_tempFileStarted, m_readerCreated, m_readOnly, m_forceNewIndex;
	int m_debugOutputLevel;
    uint32_t m_pointRecordsCount, m_maxMemoryUsage, m_cellsX, m_cellsY, m_cellsZ, m_totalCells, 
		m_tempFileWrittenBytes, m_DataVLR_ID;
    int32_t m_LowXCellCompletelyIn, m_HighXCellCompletelyIn, m_LowYCellCompletelyIn, m_HighYCellCompletelyIn,
		m_LowZCellCompletelyIn, m_HighZCellCompletelyIn;
    int32_t m_LowXBorderCell, m_HighXBorderCell, m_LowYBorderCell, m_HighYBorderCell,
		m_LowZBorderCell, m_HighZBorderCell;
    double m_minX, m_maxX, m_minY, m_maxY, m_minZ, m_maxZ, m_scaleX, m_scaleY, m_scaleZ,
		m_offsetX, m_offsetY, m_offsetZ, m_rangeX, m_rangeY, m_rangeZ, m_cellSizeZ, m_cellSizeX, m_cellSizeY;
	double m_filterMinXCell, m_filterMaxXCell, m_filterMinYCell, m_filterMaxYCell, m_filterMinZCell, m_filterMaxZCell;
	std::string m_tempFileName;	
	std::string m_outFileName;
	std::string m_indexAuthor;
	std::string m_indexComment;
	std::string m_indexDate;
	std::vector<uint32_t> m_filterResult;
	std::ostream *m_ofs;
    FILE *m_tempFile, *m_outputFile;
    FILE *m_debugger;
    
	void SetValues(void);
    bool IndexInit(void);
    void ClearOldIndex(void);
	bool BuildIndex(void);
	bool LoadIndexVLR(VariableRecord const& vlr);
	void SetCellFilterBounds(IndexData const& ParamSrc);
	bool FilterOneVLR(VariableRecord const& vlr, uint32_t& i, IndexData const& ParamSrc);
	bool FilterOnePoint(int32_t x, int32_t y, int32_t z, int32_t PointID, int32_t LastPointID, bool &LastPtRead,
		IndexData const& ParamSrc);
	bool IdentifyCell(Point const& CurPt, uint32_t& CurCellX, uint32_t& CurCellY) const;
	bool IdentifyCellZ(Point const& CurPt, uint32_t& CurCellZ) const;
	bool IdentifySubCell(Point const& CurPt, uint32_t x, uint32_t y, uint32_t& CurSubCell) const;
	bool PurgePointsToTempFile(IndexCellDataBlock& CellBlock);
	bool LoadCellFromTempFile(liblas::detail::IndexCell *CellBlock, 
		uint32_t CurCellX, uint32_t CurCellY);
	FILE *OpenTempFile(void);
	void CloseTempFile(void);
	bool SaveIndexInLASFile(void);
	bool SaveIndexInStandAloneFile(void);
	FILE *OpenOutputFile(void);
	void CloseOutputFile(void);
	bool FileError(const char *Reporter);
	bool OutputFileError(const char *Reporter) const;
	bool DebugOutputError(const char *Reporter) const;
	bool PointCountError(const char *Reporter) const;
	bool PointBoundsError(const char *Reporter) const;
	bool MemoryError(const char *Reporter) const;
	bool InitError(const char *Reporter) const;

	// debugging
	bool OutputCellStats(IndexCellDataBlock& CellBlock)  const;
	bool OutputCellGraph(std::vector<uint32_t> CellPopulation, uint32_t MaxPointsPerCell)  const;
	
public:
    bool IndexFailed(void)  const {return (! m_indexBuilt);};
    bool IndexReady(void)  const {return (m_indexBuilt);};
    const std::vector<uint32_t>& Filter(IndexData const& ParamSrc);
	double GetMinX(void) const	{return m_minX;};
	double GetMaxX(void) const	{return m_maxX;};
	double GetMinY(void) const	{return m_minY;};
	double GetMaxY(void) const	{return m_maxY;};
	double GetMinZ(void) const	{return m_minZ;};
	double GetMaxZ(void) const	{return m_maxZ;};
	uint32_t GetPointRecordsCount(void) const	{return m_pointRecordsCount;};
	uint32_t GetCellsX(void) const	{return m_cellsX;};
	uint32_t GetCellsY(void) const	{return m_cellsY;};
	uint32_t GetCellsZ(void) const	{return m_cellsZ;};
	uint32_t GetDataVLR_ID(void) const	{return m_DataVLR_ID;};
	double GetCellSizeZ(void) const	{return m_cellSizeZ;};
	FILE *GetDebugger(void) const	{return m_debugger;};
	bool GetReadOnly(void) const	{return m_readOnly;};
	bool GetForceNewIndex(void) const	{return m_forceNewIndex;};
	uint32_t GetMaxMemoryUsage(void) const	{return m_maxMemoryUsage;};
	Header *GetHeader(void) {return &m_header;};
	Reader *GetReader(void) const {return m_reader;};
	int GetDebugOutputLevel(void) const {return m_debugOutputLevel;};
	const char *GetTempFileName(void) const {return m_tempFileName.c_str();};
	const char *GetOutFileName(void) const {return m_outFileName.c_str();};
	const char *GetIndexAuthorStr(void)  const;
	const char *GetIndexCommentStr(void)  const;
	const char *GetIndexDateStr(void)  const;
	void SetDataVLR_ID(uint32_t DataVLR_ID)	{m_DataVLR_ID = DataVLR_ID;};
	void SetIndexAuthorStr(const char *ias)	{m_indexAuthor = ias;};
	void SetIndexCommentStr(const char *ics)	{m_indexComment = ics;};
	void SetIndexDateStr(const char *ids)	{m_indexDate = ids;};
	void SetMinX(double minX)	{m_minX = minX;};
	void SetMaxX(double maxX)	{m_maxX = maxX;};
	void SetMinY(double minY)	{m_minY = minY;};
	void SetMaxY(double maxY)	{m_maxY = maxY;};
	void SetMinZ(double minZ)	{m_minZ = minZ;};
	void SetMaxZ(double maxZ)	{m_maxZ = maxZ;};
	void CalcRangeX(void)	{m_rangeX = m_maxX - m_minX;};
	void CalcRangeY(void)	{m_rangeY = m_maxY - m_minY;};
	void CalcRangeZ(void)	{m_rangeZ = m_maxZ - m_minZ;};
	void SetPointRecordsCount(uint32_t prc)	{m_pointRecordsCount = prc;};
	void SetCellsX(uint32_t cellsX)	{m_cellsX = cellsX;};
	void SetCellsY(uint32_t cellsY)	{m_cellsY = cellsY;};
	void SetCellsZ(uint32_t cellsZ)	{m_cellsZ = cellsZ;};
}; 

class IndexData
{
friend class Index;

public:
	IndexData(void);
 	IndexData(Index const& index);
	bool SetInitialValues(std::istream *ifs = 0, Reader *reader = 0, std::ostream *ofs = 0, const char *tmpfilenme = 0, const char *outfilenme = 0, const char *indexauthor = 0, 
		const char *indexcomment = 0, const char *indexdate = 0, double zbinht = 0.0, 
		uint32_t maxmem = LIBLAS_INDEX_MAXMEMDEFAULT, int debugoutputlevel = 0, bool readonly = 0, bool forcenewindex = 0, FILE *debugger = 0);
	bool SetFilterValues(double LowFilterX, double HighFilterX, double LowFilterY, double HighFilterY, double LowFilterZ, double HighFilterZ);

    // Blocked copying operations, declared but not defined.
    /// Copy constructor.
    IndexData(IndexData const& other);
    /// Assignment operator.
    IndexData& operator=(IndexData const& rhs);

private:
	void SetValues(void);
	
protected:
	Reader *m_reader;
	std::istream *m_ifs;
	std::ostream *m_ofs;
	const char *m_tempFileName;
	const char *m_outFileName;
	const char *m_indexAuthor;
	const char *m_indexComment;
	const char *m_indexDate;
	double m_cellSizeZ;
	uint32_t m_maxMemoryUsage;
	int m_debugOutputLevel;
	bool m_noFilterX, m_noFilterY, m_noFilterZ, m_readOnly, m_forceNewIndex, m_indexValid;
	FILE *m_debugger;
	double m_lowFilterX, m_highFilterX, m_lowFilterY, m_highFilterY, m_lowFilterZ, m_highFilterZ;
	
public:
	double GetCellSizeZ(void) const	{return m_cellSizeZ;};
	FILE *GetDebugger(void) const	{return m_debugger;};
	bool GetReadOnly(void) const	{return m_readOnly;};
	bool GetForceNewIndex(void) const	{return m_forceNewIndex;};
	uint32_t GetMaxMemoryUsage(void) const	{return m_maxMemoryUsage;};
	Reader *GetReader(void) const {return m_reader;};
	int GetDebugOutputLevel(void) const {return m_debugOutputLevel;};
	const char *GetTempFileName(void) const {return m_tempFileName;};
	const char *GetOutFileName(void) const {return m_outFileName;};
	const char *GetIndexAuthorStr(void)  const;
	const char *GetIndexCommentStr(void)  const;
	const char *GetIndexDateStr(void)  const;
	void SetReader(Reader *reader)	{m_reader = reader;};
	void SetIStream(std::istream *ifs)	{m_ifs = ifs;};
	void SetOStream(std::ostream *ofs)	{m_ofs = ofs;};
	void SetTmpFileName(const char *tmpfilenme)	{m_tempFileName = tmpfilenme;};
	void SetOutFileName(const char *outfilenme)	{m_outFileName = outfilenme;};
	void SetIndexAuthor(const char *indexauthor)	{m_indexAuthor = indexauthor;};
	void SetIndexComment(const char *indexcomment)	{m_indexComment = indexcomment;};
	void SetIndexDate(const char *indexdate)	{m_indexDate = indexdate;};
	void SetCellSizeZ(double cellsizez)	{m_cellSizeZ = cellsizez;};
	void SetMaxMem(uint32_t maxmem)	{m_maxMemoryUsage = maxmem;};
	void SetDebugOutputLevel(int debugoutputlevel)	{m_debugOutputLevel = debugoutputlevel;};
	void SetReadOnly(bool readonly)	{m_readOnly = readonly;};
	void SetDebugger(FILE *debugger)	{m_debugger = debugger;};
};

} // namespace liblas

#endif // LIBLAS_LASINDEX_HPP_INCLUDED
