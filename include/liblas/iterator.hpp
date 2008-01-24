#ifndef LIBLAS_ITERATOR_HPP_INCLUDED
#define LIBLAS_ITERATOR_HPP_INCLUDED

#include <liblas/lasreader.hpp>
#include <iterator>
#include <cassert>

namespace liblas {

template <typename T>
class reader_iterator
{
public:

    typedef std::input_iterator_tag iterator_category;
    typedef T value_type;
    typedef T const* pointer;
    typedef T const& reference;
    typedef ptrdiff_t difference_type;

    reader_iterator()
        : m_reader(0)
    {}

    reader_iterator(liblas::LASReader& reader)
        : m_reader(&reader)
    {
        assert(0 != m_reader);
        getval();
    }

    reference operator*() const
    {
        assert(0 != m_reader);
        return m_reader->GetPoint();
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    reader_iterator& operator++()
    {
        assert(0 != m_reader);
        getval();
        return (*this);
    }

    reader_iterator operator++(int)
    {
        reader_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    bool equal(reader_iterator const& rhs) const
    {
        return m_reader == rhs.m_reader;
    }

private:

    void getval()
    {
        if (0 != m_reader && !(m_reader->ReadNextPoint()))
        {
            m_reader = 0;
        }
    }

    liblas::LASReader* m_reader;
};

typedef reader_iterator<LASPoint> lasreader_iterator;

template <typename T>
bool operator==(reader_iterator<T> const& lhs, reader_iterator<T> const& rhs)
{
    return lhs.equal(rhs);
}

template <typename T>
bool operator!=(reader_iterator<T> const& lhs, reader_iterator<T> const& rhs)
{
    return (!(lhs == rhs));
}

} // namespace liblas

#endif // LIBLAS_ITERATOR_HPP_INCLUDED
