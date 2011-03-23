/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Allocator to allow run-time allocation strategy
 * Author:   Andrew Bell, andrew.bell.ia@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2011, Andrew Bell
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following
 * conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided
 *       with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************************/

#ifndef LIBLAS_DETAIL_RT_ALLOCATOR_HPP_INCLUDED
#define LIBLAS_DETAIL_RT_ALLOCATOR_HPP_INCLUDED

/**
namespace liblas {
namespace detail {
**/

template <typename T>
class rt_allocator
{
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T *pointer;
    typedef T const *const_pointer;
    typedef T& reference;
    typedef T const & const_reference;

public:
    pointer address(reference r) const
        { return &r; }

    const_pointer address(const_reference r) const
        { return &r; }

    rt_allocator(const std::string& file_name)
    {



        using namespace boost::interprocess;

        if (!m_file_p)
        {
            // Determine file size so that we don't over-allocate.
            filebuf fbuf;
            fbuf.open(file_name.c_str(), ios_base::in);
            m_max_size = fbuf.pubseekoff(0, ios_base::end);
            fbuf.close();

            m_file_p = new file_mapping(file_name.c_str(), read_write);
        }
    }

    template <typename U>
    map_allocator(map_allocator<U> const &u)
        {}

    size_type max_size() const throw()
    {
        return (m_max_size / sizeof(T));
    }

    pointer allocate(size_type num, void *hint = 0) throw()
    {
        using namespace boost::interprocess;

        (void)hint;
        mapped_region *reg;

        m_next_offset += num / sizeof(T);
        if (m_next_offset > m_max_size)
           throw std::bad_alloc();
        reg = new mapped_region(*m_file_p, read_write, m_next_offset,
            num / sizeof(T));
        pointer p = static_cast<pointer>(reg->get_address());
        m_regions[p] = reg;
        return p;
    }

    void deallocate(pointer p, size_type num)
    {
        RegIterator ri;
        if ((ri = m_regions.find(p)) != m_regions.end())
        {
            delete ri->second;
            m_regions.erase(ri);
        }
    }

    void construct(pointer p, const_reference value)
    {
        new(p) T(value);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

    template <typename U>
    struct rebind {
        typedef map_allocator<U> other;
    };
};

template <typename T>
typename map_allocator<T>::RegVec map_allocator<T>::m_regions;

template <typename T>
typename map_allocator<T>::size_type map_allocator<T>::m_next_offset = 0;

template <typename T>
typename map_allocator<T>::size_type map_allocator<T>::m_max_size = 0;

template <typename T>
boost::interprocess::file_mapping *map_allocator<T>::m_file_p = NULL;

/**
} // namespace detail
} // namespace liblas
**/

#endif // LIBLAS_DETAIL_MAP_ALLOCATOR_HPP_INCLUDED
