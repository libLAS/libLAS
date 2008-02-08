#if defined(_MSC_VER) && defined(USE_VLD)
#include <vld.h>
#endif
// liblas
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/laswriter.hpp>
#include <liblas/guid.hpp>
// std
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <cstdlib>
#include <cassert>
// sample
#include "utility.hpp"
using namespace std;

int main()
{
    using liblas::LASHeader;

    try
    {
        char const* name = "test.las";

        std::ofstream ofs(name, ios::out | ios::binary);

        liblas::LASHeader hdr;
        hdr.SetVersionMajor(1);
        hdr.SetVersionMinor(1);
        hdr.SetDataFormatId(LASHeader::ePointFormat1);
        hdr.SetPointRecordsCount(1000); // should be corrected automatically by writer
        liblas::LASWriter writer(ofs, hdr);

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
