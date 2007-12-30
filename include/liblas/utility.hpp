#ifndef LIBLAS_UTILITY_HPP_INCLUDED
#define LIBLAS_UTILITY_HPP_INCLUDED

namespace liblas {

template <typename T>
struct Point
{
    Point() : x(T()), y(T()), z(T()) {}
    T x;
    T y;
    T z;
};

template <typename T>
struct Extents
{
    typename Point<T> min;
    typename Point<T> max;
};

} // namespace liblas

#endif // LIBLAS_UTILITY_HPP_INCLUDED
