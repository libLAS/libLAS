// $Id$/
//
// The SharedPtr class is based on implementation of CountedPtr
// from the book "The C++ Standard Library - A Tutorial and Reference"
//
// (C) Copyright Nicolai M. Josuttis 1999
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net (modifications)
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_DETAIL_SHAREDPTR_HPP_INCLUDED
#define LIBLAS_DETAIL_SHAREDPTR_HPP_INCLUDED
#include <cassert>

namespace liblas { namespace detail {

template <class T>
class SharedPtr
{
public:

    // Initialize pointer with existing pointer, where pointer p
    // is required to be a return value of new operator
    // TODO: Relax the "0 != p requirement"
    //  m_count(new long(p ? 1 : 0))
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
        assert(0 != m_ptr);
        return *m_ptr;
    }

    T* operator->() const throw()
    {
        assert(0 != m_ptr);
        return m_ptr;
    }

    T* get() const
    {
        return m_ptr;
    }

    long use_count() const
    {
        return (*m_count);
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
