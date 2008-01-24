#ifndef LIBLAS_SAMPLE_UTILITY_HPP_INCLUDED
#define LIBLAS_SAMPLE_UTILITY_HPP_INCLUDED
// std
#include <iosfwd>
#include <utility>
// Forward declarations
namespace liblas {
    class LASPoint;
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
       << "\n";
    return os;
}

#endif // LIBLAS_SAMPLE_UTILITY_HPP_INCLUDED
