#ifndef LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
#define LIBLAS_DETAIL_UTILITY_HPP_INCLUDED

namespace liblas { namespace detail {

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
    typename detail::Point < T > min;
    typename detail::Point < T > max;
};

template<typename T>
inline char* bytes_of(T& data)
{
    return static_cast<char*>(static_cast<void*>(&data));
}

template<typename T>
inline char* bytes_of(T* data)
{
    return static_cast<char*>(static_cast<void*>(data));
}

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
