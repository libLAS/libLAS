/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: C++ interface to libLAS, definition of LASFile base class 
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LIBLAS_HPP_INCLUDED
#define LIBLAS_HPP_INCLUDED

#include <exception>

#include <lastypes.hpp>
#include <laspoint.hpp>
#include <lasheader.hpp>

namespace liblas {

enum OpenMode {
    eRead = 0,
    eReadWrite,
    eReadWriteUpdate
};

class LASFile;
class LASFileV1;

LASFile *Open(const std::string& sFilename);
LASFile *Open(const std::string& sFilename, OpenMode eCreate);

class LASFile {
public:


    LASFile(const std::string& sFilename);
    LASFile(const std::string& sFilename, OpenMode eCreateFlags);
    virtual ~LASFile();

    virtual const LASRecordHeader &GetVariableHeaderRecord (uint16_t nEntryID) 
        const;
   
    /* Get a specific point */
    virtual const LASPoint &GetPointByID (uint64_t nPointID) const;

    /* for iterating over the entire file, one point at a time */
    virtual const LASPoint &GetNextPoint() const;
    /* return to first record */
    void Rewind() { m_lNextPoint = 0; } 
    /* Skip to record nRecord */
    void SkipTo(uint64_t nRecord);
    /* Get the number of the next record to be read */
    uint64_t GetNextPointID() const { return m_lNextPoint; }

    /* for writing point records */
    virtual void SetPoint(const LASPoint &roPoint, uint64_t nPointID);
    virtual void AddPoint(const LASPoint &roPoint);

protected:
    FILE *m_fp;
    uint64_t m_lNextPoint;
    uint64_t m_lPointCount;
    std::string& m_sFilename;
    /* invariably, we will need more members here, for example,
     * offsets, scaling factors, etc...
     */
     
private:

    // Make the type noncopyable
    LASFile(const LASFile& other);
    LASFile& operator=(const LASFile& other);
};

}; /* end namespace liblas */

#endif /* ndef LIBLAS_HPP_INCLUDED */
