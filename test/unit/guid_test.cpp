// $Id$
#include <liblas/guid.hpp>
#include <tut/tut.hpp>
#include <string>

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

    tg test_group("liblas::guid");

    // Test default construction
    template<>
    template<>
    void to::test<1>()
    {
        ensure(m_d.is_null());

        ensure_equals(16, m_d.byte_count());

        std::string s = m_d.to_string();
        ensure_equals(s, m_dstr);

        liblas::guid g;
        ensure_equals(g, m_d);
    }
};
