#ifndef LIBLAS_DETAIL_SHAREDPTR_HPP_INCLUDED
#define LIBLAS_DETAIL_SHAREDPTR_HPP_INCLUDED

// Copyright Mateusz Loskot 2007, mateusz@loskot.net
// Copyright Nicolai M. Josuttis 1999
//
// The SharedPtr class is based on implementation of CountedPtr
// from the book "The C++ Standard Library - A Tutorial and Reference"
// by Nicolai M. Josuttis, Addison-Wesley, 1999

namespace liblas { namespace detail {

template <class T>
class SharedPtr
{
public:

    // Initialize pointer with existing pointer, where pointer p
    // is required to be a return value of new operator
    explicit SharedPtr(T* p = 0)
        : m_ptr(p), m_count(new long(1))
    {}

    // Copy pointer (one more owner)
    SharedPtr(SharedPtr<T> const& other) throw()
        : m_ptr(other.m_ptr), m_count(other.m_count)
    {
            ++*m_count;
    }

    // Destructor (delete value if this was the last owner)
    ~SharedPtr() throw()
    {
        dispose();
    }

    // Assignment operator (unshare old and share new value)
    SharedPtr<T>& operator=(SharedPtr<T> const& rhs) throw()
    {
        if (&rhs != this)
        {
            dispose();
            m_ptr = rhs.m_ptr;
            m_count = rhs.m_count;
            ++*m_count;
        }
        return *this;
    }

    T& operator*() const throw()
    {
        return *m_ptr;
    }

    T* operator->() const throw()
    {
        return m_ptr;
    }

private:

    T* m_ptr;
    long* m_count; // shared number of owners

    void dispose()
    {
        if (--*m_count == 0)
        {
            delete m_count;
            delete m_ptr;
        }
    }

};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_SHAREDPTR_HPP_INCLUDED
