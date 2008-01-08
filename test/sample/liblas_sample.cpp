#include <vld.h>
#include <liblas/cstdint.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/detail/timer.hpp>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <utility>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <cassert>
using namespace std;


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


int main()
{
    try
    {
        liblas::LASPoint p1;
        p1.SetCoordinates(1, 2, 3);
        liblas::LASPoint p2(p1);
        cout << "p1 - " << p1 << '\n';
        cout << "p2 - " << p2 << '\n';
        p2.SetCoordinates(4, 5, 6);
        p1 = p2;
        cout << "p1 - " << p1 << '\n';

        return 0;


        char* const name = "d:\\data\\lidar\\LDR030828_213450_0.LAS"; 
        liblas::LASReader reader(name);
        
        cout << "File: " << name << '\n';
        cout << "Version: " << reader.GetVersion() << '\n';

        cout << "Signature: " << reader.GetHeader().GetFileSignature() << '\n';
        cout << "Points count: " << reader.GetHeader().GetPointRecordsCount() << '\n';

        liblas::detail::Timer t;
        t.start();
        
        typedef std::pair<double, double> minmax_t;
        minmax_t mx;
        minmax_t my;
        minmax_t mz;
        liblas::uint32_t i = 0;
        while (reader.ReadPoint())
        {
            liblas::LASPoint const& p = reader.GetPoint();

            mx.first = std::min<double>(mx.first, p.GetX());
            mx.second = std::max<double>(mx.second, p.GetX());
            my.first = std::min<double>(my.first, p.GetY());
            my.second = std::max<double>(my.second, p.GetY());
            mz.first = std::min<double>(mz.first, p.GetZ());
            mz.second = std::max<double>(mz.second, p.GetZ());
            
            // Warning: Printing zillion of points may take looong time
            //cout << i << ". " << p << '\n';
            i++;
        }
        double const d = t.stop();

        if (reader.GetHeader().GetPointRecordsCount() != i)
            throw std::runtime_error("read incorrect number of point records");

        cout << "Read points: " << i << " (" << d << ")\n"
            << std::fixed << std::setprecision(6)
            << "\nX: " << mx
            << "\nY: " << my
            << "\nZ: " << mz    
            << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}