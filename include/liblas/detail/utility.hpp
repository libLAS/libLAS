#ifndef LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
#define LIBLAS_DETAIL_UTILITY_HPP_INCLUDED

#include <iosfwd>
#include <sstream> // std::ostringstream
#include <stdexcept>

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
void read_n(T& dest, std::istream& src, std::streamsize const& num)
{
    // TODO: Review and redesign errors handling logic if necessary

    if (!src)
        throw std::runtime_error("input stream is not readable");

    // Read bytes into buffer
    src.read(detail::bytes_of(dest), num);
    
    // Test stream state bits
    if (src.eof())
        throw std::runtime_error("end of file encountered");
    else if (src.fail())
        throw std::runtime_error("non-fatal I/O error occured");
    else if (src.bad())
        throw std::runtime_error("fatal I/O error occured");

    // Poor man test of data consistency
    std::streamsize const rn = src.gcount();
    if (num != rn)
    {
        std::ostringstream os;
        os << "read only " << rn << " bytes of " << num;
        throw std::runtime_error(os.str());
    }
}

template <typename T>
void write_n(std::ostream& dest, T const& src, std::streamsize const& num)
{
    if (!dest)
        throw std::runtime_error("output stream is not writable");

    dest.write(detail::bytes_of(src), num);

    // Test stream state bits
    if (dest.eof())
        throw std::runtime_error("end of file encountered");
    else if (dest.fail())
        throw std::runtime_error("non-fatal I/O error occured");
    else if (dest.bad())
        throw std::runtime_error("fatal I/O error occured");
}


}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_UTILITY_HPP_INCLUDED
