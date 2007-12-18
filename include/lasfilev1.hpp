/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: Declaration of the hidden LASFileV1 class for reading LAS V1.x
 *          format data.
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#ifndef LASFILEV1_HPP_INCLUDED
#define LASFILEV1_HPP_INCLUDED

#include <liblas.hpp>
#include <lastypes.hpp>

namespace liblas {

namespace details {

    class LASFileV1 : public LASFile {
    public:
        LASFileV1(const std::string& sFilename);
        LASFileV1(const std::string& sFilename, OpenMode eCreateFlags);
        ~LASFileV1();

        const LASRecordHeader &GetVariableHeaderRecord(uint16_t nEntryID) const;
        const LASPoint &GetPointByID(uint64_t nPointID) const;
        const LASPoint &GetNextPoint() const;
        void SetPoint(const LASPoint &roPoint, uint64_t nPointID);
        void AddPoint(const LASPoint &roPoint);
    private:

    };

}; /* end namespace liblas::details */

}; /* end namespace liblas */

#endif /* ndef LASFILEV1_HPP_INCLUDED*/
