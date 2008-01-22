#include <vld.h>
// liblas
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/laswriter.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/guid.hpp>
// std
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <cstdlib>
#include <cassert>
using namespace std;

template <typename T>
inline std::ostream& operator<<(std::ostream& os, typename std::pair<T, T> const& p);
inline std::ostream& operator<<(std::ostream& os, liblas::LASPoint const& p);

int main()
{
    using liblas::LASHeader;

    try
    {
        char const* name = "test.las";

        {
            std::ofstream ofs(name, ios::out | ios::binary);

            liblas::LASHeader hdr;
            hdr.SetDataFormatId(LASHeader::ePointFormat1);
            hdr.SetPointRecordsCount(1000); // should be corrected automatically by writer
            liblas::LASWriter writer(ofs, hdr);

            liblas::LASPoint p;
            p.SetCoordinates(10, 20, 30);

            writer.WritePoint(p);
        }
        return 0;

        {
            std::ifstream ifs(name, ios::out | ios::binary);
            liblas::LASReader reader(ifs);
            reader.ReadNextPoint();
        }
    }
    catch (std::exception const& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown error\n";
    }

    return 0;
}


template <typename T>
inline std::ostream& operator<<(std::ostream& os, typename std::pair<T, T> const& p)
{
    os << p.first << "   " << p.second;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, liblas::LASPoint const& p)
{
    os << std::fixed << std::setprecision(6)
       << "\nx: " << p.GetX()
       << "\ny: " << p.GetY()
       << "\nz: " << p.GetZ()
       << "\nint: " << p.GetIntensity()
       << "\nrn: " << p.GetReturnNumber()
       << "\nnor: " << p.GetNumberOfReturns()
       << "\nsd: " << p.GetScanDirection()
       << "\neofl: " << p.GetFlightLineEdge()
       << "\ncls: " << p.GetClassification()
       << std::endl;
    return os;
}
