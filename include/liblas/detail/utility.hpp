#ifndef LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
#define LIBLAS_DETAIL_UTILITY_HPP_INCLUDED

#include <iosfwd>

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

template <typename T>
void read_n(T& dest, std::istream& input, std::streamsize const& num)
{
    // TODO: Review and redesign errors handling logic if necessary

    if (!input)
        throw std::runtime_error("input stream is not readable");

    // Read bytes into buffer
    input.read(detail::bytes_of(dest), num);
    
    // Test stream state bits
    if (input.eof())
        throw std::runtime_error("end of file encountered");
    else if (input.fail())
        throw std::runtime_error("non-fatal I/O error occured");
    else if (input.bad())
        throw std::runtime_error("fatal I/O error occured");

    // Poor man test of data consistency
    std::streamsize const rn = input.gcount();
    if (num != rn)
    {
        throw std::runtime_error("read only " + rn + std::string("bytes of " + num));
    }
}

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
