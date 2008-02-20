// $Id: read.cpp 376 2008-02-10 16:24:31Z mloskot $
//
// (C) Copyright Howard Butler 2008
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#if defined(_MSC_VER) && defined(USE_VLD)
#include <vld.h>
#endif
#include <liblas/lasreader.hpp>
#include <liblas/laserror.hpp>
#include <liblas/laswriter.hpp>
#include <liblas/lasfile.hpp>
#include <liblas/guid.hpp>

#include <string>
#include <stack>
#include <map>
//#include <cstdio>
#include <exception>

#include <iostream>
#include <fstream>

int main()
{
    using namespace liblas;

    try
    {
        char const* name = "TO_core_last_clip.las";

        std::ios::openmode const write_mode = std::ios::out | std::ios::binary | std::ios::ate;
        std::ios::openmode const read_mode = std::ios::in | std::ios::binary;

        std::ifstream *ifs = new std::ifstream(name, read_mode);
        LASReader *reader = new LASReader(*ifs);

        LASHeader hdr = reader->GetHeader();


        hdr.SetVersionMajor(1);
        hdr.SetVersionMinor(0);
        hdr.SetSoftwareId("hobu");
        hdr.SetDataFormatId(LASHeader::ePointFormat0);
        hdr.SetPointRecordsCount(1000); // should be corrected automatically by writer

        ifs->close();

        std::ofstream *ofs = new std::ofstream(name, write_mode);

        liblas::LASWriter writer(*ofs, hdr);

        liblas::LASPoint p;
        p.SetCoordinates(10, 20, 30);

        writer.WritePoint(p);
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error\n";
    }

    return 0;
}
