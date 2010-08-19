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

#include <liblas/lasreader.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/lasbounds.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/detail/index/indexcell.hpp>
// boost
#include <boost/cstdint.hpp>
// std
#include <cstddef> // std::size_t
#include <cstdio>
#include <iostream>
#include <stdexcept> // std::out_of_range
#include <vector> // std::vector

namespace liblas {

std::size_t const LIBLAS_INDEX_MAXMEMDEFAULT = 10 * 1024 * 1024; // 10 megs default
int const LIBLAS_INDEX_VERSIONMAJOR = 1;
int const LIBLAS_INDEX_VERSIONMINOR = 0;

typedef std::vector<boost::uint8_t> IndexVLRData;
typedef std::vector<liblas::detail::IndexCell> IndexCellRow;
typedef std::vector<IndexCellRow>	IndexCellDataBlock;

class IndexData;

/// Point data record composed with X, Y, Z coordinates and attributes.
class Index
{
public:
	Index();
	Index(liblas::Reader *reader, std::ostream *ofs = 0, const char *tmpfilenme = 0, const char *indexauthor = 0, 
		const char *indexcomment = 0, const char *indexdate = 0, 
		double zbinht = 0.0, boost::uint32_t maxmem = LIBLAS_INDEX_MAXMEMDEFAULT, int debugoutputlevel = 0, FILE *debugger = 0);
    Index(std::istream *ifs, std::ostream *ofs = 0, const char *tmpfilenme = 0, const char *indexauthor = 0, 
		const char *indexcomment = 0, const char *indexdate = 0, 
		double zbinht = 0.0, boost::uint32_t maxmem = LIBLAS_INDEX_MAXMEMDEFAULT, int debugoutputlevel = 0, FILE *debugger = 0);
    Index(IndexData const& ParamSrc);
	~Index();
	
    // Blocked copying operations, declared but not defined.
    /// Copy constructor.
    Index(Index const& other);
    /// Assignment operator.
    Index& operator=(Index const& rhs);
    
private:

	Reader *m_reader;
	Reader *m_idxreader;
	Header m_pointheader;
	Header m_idxheader;
	Bounds<double> m_bounds;
	bool m_indexBuilt, m_tempFileStarted, m_readerCreated, m_readOnly, m_writestandaloneindex, m_forceNewIndex;
	int m_debugOutputLevel;
    boost::uint32_t m_pointRecordsCount, m_maxMemoryUsage, m_cellsX, m_cellsY, m_cellsZ, m_totalCells, 
		m_tempFileWrittenBytes, m_DataVLR_ID;
    boost::int32_t m_LowXCellCompletelyIn, m_HighXCellCompletelyIn, m_LowYCellCompletelyIn, m_HighYCellCompletelyIn,
		m_LowZCellCompletelyIn, m_HighZCellCompletelyIn;
    boost::int32_t m_LowXBorderCell, m_HighXBorderCell, m_LowYBorderCell, m_HighYBorderCell,
		m_LowZBorderCell, m_HighZBorderCell;
    double m_rangeX, m_rangeY, m_rangeZ, m_cellSizeZ, m_cellSizeX, m_cellSizeY;
	double m_filterMinXCell, m_filterMaxXCell, m_filterMinYCell, m_filterMaxYCell, m_filterMinZCell, m_filterMaxZCell,
		m_LowXBorderPartCell, m_HighXBorderPartCell, m_LowYBorderPartCell, m_HighYBorderPartCell;
	std::string m_tempFileName;	
	std::string m_indexAuthor;
	std::string m_indexComment;
	std::string m_indexDate;
	std::vector<boost::uint32_t> m_filterResult;
	std::ostream *m_ofs;
    FILE *m_tempFile, *m_outputFile;
    FILE *m_debugger;
    
	void SetValues(void);
    bool IndexInit(void);
    void ClearOldIndex(void);
	bool BuildIndex(void);
	bool LoadIndexVLR(VariableRecord const& vlr);
	void SetCellFilterBounds(IndexData const& ParamSrc);
	bool FilterOneVLR(VariableRecord const& vlr, boost::uint32_t& i, IndexData const& ParamSrc);
	bool VLRInteresting(boost::int32_t MinCellX, boost::int32_t MinCellY, boost::int32_t MaxCellX, boost::int32_t MaxCellY, 
		IndexData const& ParamSrc);
	bool CellInteresting(boost::int32_t x, boost::int32_t y, IndexData const& ParamSrc);
	bool SubCellInteresting(boost::int32_t SubCellID, boost::int32_t XCellID, boost::int32_t YCellID, IndexData const& ParamSrc);
	bool ZCellInteresting(boost::int32_t ZCellID, IndexData const& ParamSrc);
	bool FilterOnePoint(boost::int32_t x, boost::int32_t y, boost::int32_t z, boost::int32_t PointID, boost::int32_t LastPointID, bool &LastPtRead,
		IndexData const& ParamSrc);
	bool IdentifyCell(Point const& CurPt, boost::uint32_t& CurCellX, boost::uint32_t& CurCellY) const;
	bool IdentifyCellZ(Point const& CurPt, boost::uint32_t& CurCellZ) const;
	bool IdentifySubCell(Point const& CurPt, boost::uint32_t x, boost::uint32_t y, boost::uint32_t& CurSubCell) const;
	bool PurgePointsToTempFile(IndexCellDataBlock& CellBlock);
	bool LoadCellFromTempFile(liblas::detail::IndexCell *CellBlock, 
		boost::uint32_t CurCellX, boost::uint32_t CurCellY);
	FILE *OpenTempFile(void);
	void CloseTempFile(void);
	bool SaveIndexInLASFile(void);
	bool SaveIndexInStandAloneFile(void);
	bool FileError(const char *Reporter);
	bool OutputFileError(const char *Reporter) const;
	bool DebugOutputError(const char *Reporter) const;
	bool PointCountError(const char *Reporter) const;
	bool PointBoundsError(const char *Reporter) const;
	bool MemoryError(const char *Reporter) const;
	bool InitError(const char *Reporter) const;
	bool InputBoundsError(const char *Reporter) const;

	// debugging
	bool OutputCellStats(IndexCellDataBlock& CellBlock)  const;
	bool OutputCellGraph(std::vector<boost::uint32_t> CellPopulation, boost::uint32_t MaxPointsPerCell)  const;
	
public:
    bool IndexFailed(void)  const {return (! m_indexBuilt);};
    bool IndexReady(void)  const {return (m_indexBuilt);};
    const std::vector<boost::uint32_t>& Filter(IndexData const& ParamSrc);
	double GetMinX(void) const	{return m_bounds.min(0);};
	double GetMaxX(void) const	{return m_bounds.max(0);};
	double GetMinY(void) const	{return m_bounds.min(1);};
	double GetMaxY(void) const	{return m_bounds.max(1);};
	double GetMinZ(void) const	{return m_bounds.min(2);};
	double GetMaxZ(void) const	{return m_bounds.max(2);};
	Bounds<double> const& GetBounds(void) const	{return m_bounds;};
	boost::uint32_t GetPointRecordsCount(void) const	{return m_pointRecordsCount;};
	boost::uint32_t GetCellsX(void) const	{return m_cellsX;};
	boost::uint32_t GetCellsY(void) const	{return m_cellsY;};
	boost::uint32_t GetCellsZ(void) const	{return m_cellsZ;};
	boost::uint32_t GetDataVLR_ID(void) const	{return m_DataVLR_ID;};
	double GetCellSizeZ(void) const	{return m_cellSizeZ;};
	FILE *GetDebugger(void) const	{return m_debugger;};
	bool GetReadOnly(void) const	{return m_readOnly;};
	bool GetStandaloneIndex(void) const	{return m_writestandaloneindex;};
	bool GetForceNewIndex(void) const	{return m_forceNewIndex;};
	boost::uint32_t GetMaxMemoryUsage(void) const	{return m_maxMemoryUsage;};
	Header *GetPointHeader(void) {return &m_pointheader;};
	Header *GetIndexHeader(void) {return &m_idxheader;};
	Reader *GetReader(void) const {return m_reader;};
	Reader *GetIndexReader(void) const {return m_idxreader;};
	int GetDebugOutputLevel(void) const {return m_debugOutputLevel;};
	const char *GetTempFileName(void) const {return m_tempFileName.c_str();};
	const char *GetIndexAuthorStr(void)  const;
	const char *GetIndexCommentStr(void)  const;
	const char *GetIndexDateStr(void)  const;
	void SetDataVLR_ID(boost::uint32_t DataVLR_ID)	{m_DataVLR_ID = DataVLR_ID;};
	void SetIndexAuthorStr(const char *ias)	{m_indexAuthor = ias;};
	void SetIndexCommentStr(const char *ics)	{m_indexComment = ics;};
	void SetIndexDateStr(const char *ids)	{m_indexDate = ids;};
	void SetMinX(double minX)	{m_bounds.min(0, minX);};
	void SetMaxX(double maxX)	{m_bounds.max(0, maxX);};
	void SetMinY(double minY)	{m_bounds.min(1, minY);};
	void SetMaxY(double maxY)	{m_bounds.max(1, maxY);};
	void SetMinZ(double minZ)	{m_bounds.min(2, minZ);};
	void SetMaxZ(double maxZ)	{m_bounds.max(2, maxZ);};
	void CalcRangeX(void)	{m_rangeX = m_bounds.max(0) - m_bounds.min(0);};
	void CalcRangeY(void)	{m_rangeY = m_bounds.max(1) - m_bounds.min(1);};
	void CalcRangeZ(void)	{m_rangeZ = m_bounds.max(2) - m_bounds.min(2);};
	void SetPointRecordsCount(boost::uint32_t prc)	{m_pointRecordsCount = prc;};
	void SetCellsX(boost::uint32_t cellsX)	{m_cellsX = cellsX;};
	void SetCellsY(boost::uint32_t cellsY)	{m_cellsY = cellsY;};
	void SetCellsZ(boost::uint32_t cellsZ)	{m_cellsZ = cellsZ;};
}; 

class IndexData
{
friend class Index;

public:
	IndexData(void);
 	IndexData(Index const& index);
	bool SetInitialValues(std::istream *ifs = 0, Reader *reader = 0, std::ostream *ofs = 0, Reader *idxreader = 0, const char *tmpfilenme = 0, const char *indexauthor = 0, 
		const char *indexcomment = 0, const char *indexdate = 0, double zbinht = 0.0, 
		boost::uint32_t maxmem = LIBLAS_INDEX_MAXMEMDEFAULT, int debugoutputlevel = 0, bool readonly = 0, bool writestandaloneindex = 0,
		bool forcenewindex = 0, FILE *debugger = 0);
	bool SetFilterValues(double LowFilterX, double HighFilterX, double LowFilterY, double HighFilterY, double LowFilterZ, double HighFilterZ);
	bool SetFilterValues(Bounds<double> const& src);

    // Blocked copying operations, declared but not defined.
    /// Copy constructor.
    IndexData(IndexData const& other);
    /// Assignment operator.
    IndexData& operator=(IndexData const& rhs);

private:
	void SetValues(void);
	bool CalcFilterEnablers(void);
	
protected:
	Reader *m_reader;
	Reader *m_idxreader;
	Bounds<double> m_filter;
	std::istream *m_ifs;
	std::ostream *m_ofs;
	const char *m_tempFileName;
	const char *m_indexAuthor;
	const char *m_indexComment;
	const char *m_indexDate;
	double m_cellSizeZ;
	boost::uint32_t m_maxMemoryUsage;
	int m_debugOutputLevel;
	bool m_noFilterX, m_noFilterY, m_noFilterZ, m_readOnly, m_writestandaloneindex, m_forceNewIndex, m_indexValid;
	FILE *m_debugger;
	
public:
	double GetCellSizeZ(void) const	{return m_cellSizeZ;};
	FILE *GetDebugger(void) const	{return m_debugger;};
	bool GetReadOnly(void) const	{return m_readOnly;};
	bool GetStandaloneIndex(void) const	{return m_writestandaloneindex;};
	bool GetForceNewIndex(void) const	{return m_forceNewIndex;};
	boost::uint32_t GetMaxMemoryUsage(void) const	{return m_maxMemoryUsage;};
	Reader *GetReader(void) const {return m_reader;};
	int GetDebugOutputLevel(void) const {return m_debugOutputLevel;};
	const char *GetTempFileName(void) const {return m_tempFileName;};
	const char *GetIndexAuthorStr(void)  const;
	const char *GetIndexCommentStr(void)  const;
	const char *GetIndexDateStr(void)  const;
	double GetMinFilterX(void) const	{return m_filter.min(0);};
	double GetMaxFilterX(void) const	{return m_filter.max(0);};
	double GetMinFilterY(void) const	{return m_filter.min(1);};
	double GetMaxFilterY(void) const	{return m_filter.max(1);};
	double GetMinFilterZ(void) const	{return m_filter.min(2);};
	double GetMaxFilterZ(void) const	{return m_filter.max(2);};
	void SetReader(Reader *reader)	{m_reader = reader;};
	void SetIStream(std::istream *ifs)	{m_ifs = ifs;};
	void SetOStream(std::ostream *ofs)	{m_ofs = ofs;};
	void SetTmpFileName(const char *tmpfilenme)	{m_tempFileName = tmpfilenme;};
	void SetIndexAuthor(const char *indexauthor)	{m_indexAuthor = indexauthor;};
	void SetIndexComment(const char *indexcomment)	{m_indexComment = indexcomment;};
	void SetIndexDate(const char *indexdate)	{m_indexDate = indexdate;};
	void SetCellSizeZ(double cellsizez)	{m_cellSizeZ = cellsizez;};
	void SetMaxMem(boost::uint32_t maxmem)	{m_maxMemoryUsage = maxmem;};
	void SetDebugOutputLevel(int debugoutputlevel)	{m_debugOutputLevel = debugoutputlevel;};
	void SetReadOnly(bool readonly)	{m_readOnly = readonly;};
	void SetStandaloneIndex(bool writestandaloneindex)	{m_writestandaloneindex = writestandaloneindex;};
	void SetDebugger(FILE *debugger)	{m_debugger = debugger;};
};

template <typename T, typename Q>
inline void ReadVLRData_n(T& dest, IndexVLRData const& src, Q& pos)
{
    // error if reading past array end
    if (static_cast<size_t>(pos) + sizeof(T) > src.size())
		throw std::out_of_range("liblas::detail::ReadVLRData_n: array index out of range");
	// copy sizeof(T) bytes to destination
    memcpy(&dest, &src[pos], sizeof(T));
    // Fix little-endian
    LIBLAS_SWAP_BYTES_N(dest, sizeof(T));
    // increment the write position to end of written data
    pos = pos + static_cast<Q>(sizeof(T));
}

template <typename T, typename Q>
inline void ReadVLRDataNoInc_n(T& dest, IndexVLRData const& src, Q const& pos)
{
    // error if reading past array end
    if (static_cast<size_t>(pos) + sizeof(T) > src.size())
		throw std::out_of_range("liblas::detail::ReadVLRDataNoInc_n: array index out of range");
	// copy sizeof(T) bytes to destination
    memcpy(&dest, &src[pos], sizeof(T));
    // Fix little-endian
    LIBLAS_SWAP_BYTES_N(dest, sizeof(T));
}

template <typename T, typename Q>
inline void ReadeVLRData_str(char * dest, IndexVLRData const& src, T const srclen, Q& pos)
{
    // error if reading past array end
    if (static_cast<size_t>(pos) + static_cast<size_t>(srclen) > src.size())
		throw std::out_of_range("liblas::detail::ReadeVLRData_str: array index out of range");
 	// copy srclen bytes to destination
	memcpy(dest, &src[pos], srclen);
    // increment the write position to end of written data
    pos = pos + static_cast<Q>(srclen);
}

template <typename T, typename Q>
inline void ReadVLRDataNoInc_str(char * dest, IndexVLRData const& src, T const srclen, Q pos)
{
    // error if reading past array end
    if (static_cast<size_t>(pos) + static_cast<size_t>(srclen) > src.size())
		throw std::out_of_range("liblas::detail::ReadVLRDataNoInc_str: array index out of range");
 	// copy srclen bytes to destination
	memcpy(dest, &src[pos], srclen);
}

} // namespace liblas

#endif // LIBLAS_LASINDEX_HPP_INCLUDED
