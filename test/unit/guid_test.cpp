// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/guid.hpp>
#include <tut/tut.hpp>
#include <string>
#include <algorithm> // std::std::transform
#include <cstdlib> // std::size_t
#include <cctype> // std::toupper

namespace tut
{ 
    struct guid_data
    {
        guid_data()
            : m_dstr("00000000-0000-0000-0000-000000000000")
        {}

        std::string m_dstr;
        liblas::guid m_d;
    };

    typedef test_group<guid_data> tg;
    typedef tg::object to;

    tg test_group_guid("liblas::guid");

    // Test default construction
    template<>
    template<>
    void to::test<1>()
    {
        ensure(m_d.is_null());

        std::size_t const len = 16;
        ensure_equals(len, m_d.byte_count());

        std::string s = m_d.to_string();
        ensure_equals(s, m_dstr);

        liblas::guid g;
        ensure_equals(len, g.byte_count());
        ensure_equals(g, m_d);
    }

    // Test construction from C string
    template<>
    template<>
    void to::test<2>()
    {
        std::string s1("3F2504E0-4F89-11D3-9A0C-0305E82C3301");
        liblas::guid g(s1.c_str());

        ensure_not(g.is_null());
        ensure_not(m_d == g);

        std::string s2 = m_d.to_string();
        std::transform(s2.begin(), s2.end(), s2.begin(), (int(*)(int))std::toupper);
        ensure_not(s1 == s2);

        s2 = g.to_string();
        std::transform(s2.begin(), s2.end(), s2.begin(), (int(*)(int))std::toupper);
        ensure_equals(s2, s1);
    }

    // Test construction from std::string
    template<>
    template<>
    void to::test<3>()
    {
        std::string s1("3F2504E0-4F89-11D3-9A0C-0305E82C3301");
        liblas::guid g(s1.c_str());

        ensure_not(g.is_null());
        ensure_not(m_d == g);

        std::string s2 = g.to_string();
        std::transform(s2.begin(), s2.end(), s2.begin(), (int(*)(int))std::toupper);
        ensure_equals(s2, s1);
    }

    // Test construction from 4-2-2-8-bytes long fields
    template<>
    template<>
    void to::test<4>()
    {
        liblas::uint32_t d1(0);
        liblas::uint16_t d2(0);
        liblas::uint16_t d3(0);
        liblas::uint8_t d4[8] = { 0 };

        liblas::guid g(d1, d2, d3, d4);

        ensure(g.is_null());
        ensure_equals(g, m_d);

    }

    // Test copy constructor
    template<>
    template<>
    void to::test<5>()
    {
        liblas::guid g(m_d);

        ensure(g.is_null());
        ensure_equals(g, m_d);

        std::string s1("3F2504E0-4F89-11D3-9A0C-0305E82C3301");
        liblas::guid g1(s1.c_str());

        liblas::guid g2(g1);
        ensure_equals(g2, g1);
    }

    // Test assignment operator
    template<>
    template<>
    void to::test<6>()
    {
        liblas::guid g;
        ensure(g.is_null());

        g = m_d;
        ensure(g.is_null());
        ensure_equals(g, m_d);

        std::string s1("3F2504E0-4F89-11D3-9A0C-0305E82C3301");
        liblas::guid g1(s1.c_str());

        liblas::guid g2;
        ensure(g.is_null());
        ensure_equals(g, m_d);

        g2 = g1;
        ensure_equals(g2, g1);
    }
}

