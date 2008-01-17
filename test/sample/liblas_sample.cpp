#include <vld.h>
// liblas
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
// std
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
inline std::ostream& operator<<(std::ostream& os, typename std::pair<T, T> const& p);
inline std::ostream& operator<<(std::ostream& os, liblas::LASPoint const& p);

int main()
{
    try
    {
    }
    catch (std::exception const& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
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
