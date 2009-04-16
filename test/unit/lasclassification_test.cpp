// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/lasclassification.hpp>
#include <liblas/cstdint.hpp>
#include <tut/tut.hpp>
#include <bitset>
#include <sstream>
#include <stdexcept>
#include <string>
#include "common.hpp"

namespace tut
{ 
    struct lasclassification_data
    {
        typedef liblas::LASClassification bitset_type;
        liblas::LASClassification m_default;
    };

    typedef test_group<lasclassification_data> tg;
    typedef tg::object to;

    tg test_group_lasclassification("liblas::LASClassification");

    template<>
    template<>
    void to::test<1>()
    {
        ensure_equals(m_default, bitset_type(0));
        ensure_equals(m_default.GetClass(), 0);
        ensure_not(m_default.IsSynthetic());
        ensure_not(m_default.IsKeyPoint());
        ensure_not(m_default.IsWithheld());
    }

    template<>
    template<>
    void to::test<2>()
    {
        liblas::LASClassification c0(0);

        ensure_equals(c0, m_default);
        ensure_equals(c0, bitset_type(0));
        ensure_equals(c0.GetClass(), 0);
        ensure_not(c0.IsSynthetic());
        ensure_not(c0.IsKeyPoint());
        ensure_not(c0.IsWithheld());
    }

    template<>
    template<>
    void to::test<3>()
    {
        liblas::LASClassification c31(0x1F);

        ensure_not(c31 == bitset_type(0));
        ensure_equals(c31.GetClass(), 31);
        ensure_not(c31.IsSynthetic());
        ensure_not(c31.IsKeyPoint());
        ensure_not(c31.IsWithheld());
    }

    template<>
    template<>
    void to::test<4>()
    {
        liblas::LASClassification c255(255);
        ensure_equals(c255, bitset_type(255));
        ensure_equals(c255.GetClass(), 31);
        ensure(c255.IsSynthetic());
        ensure(c255.IsKeyPoint());
        ensure(c255.IsWithheld());
    }

    template<>
    template<>
    void to::test<5>()
    {
        liblas::LASClassification c;

        c.SetClass(0);
        ensure_equals(c.GetClass(), 0);

        c.SetClass(31);
        ensure_equals(c.GetClass(), 31);

        ensure(c != m_default);
    }

    template<>
    template<>
    void to::test<6>()
    {
        liblas::LASClassification c;

        c.SetSynthetic(true);
        ensure(c.IsSynthetic());
        ensure(c != m_default);

        c.SetSynthetic(false);
        ensure_not(c.IsSynthetic());
        ensure_equals(c, m_default);

        c.SetSynthetic(true);
        ensure(c.IsSynthetic());
        ensure(c != m_default);

        c.SetSynthetic(false);
        ensure_not(c.IsSynthetic());
        ensure_equals(c, m_default);

        ensure_equals(c.GetClass(), 0);
    }

    template<>
    template<>
    void to::test<7>()
    {
        liblas::LASClassification c;

        c.SetKeyPoint(true);
        ensure(c.IsKeyPoint());
        ensure(c != m_default);

        c.SetKeyPoint(false);
        ensure_not(c.IsKeyPoint());
        ensure_equals(c, m_default);

        c.SetKeyPoint(true);
        ensure(c.IsKeyPoint());
        ensure(c != m_default);

        c.SetKeyPoint(false);
        ensure_not(c.IsKeyPoint());
        ensure_equals(c, m_default);

        ensure_equals(c.GetClass(), 0);
    }

    template<>
    template<>
    void to::test<8>()
    {
        liblas::LASClassification c;

        c.SetWithheld(true);
        ensure(c.IsWithheld());
        ensure(c != m_default);

        c.SetWithheld(false);
        ensure_not(c.IsWithheld());
        ensure_equals(c, m_default);

        c.SetWithheld(true);
        ensure(c.IsWithheld());
        ensure(c != m_default);

        c.SetWithheld(false);
        ensure_not(c.IsWithheld());
        ensure_equals(c, m_default);

        ensure_equals(c.GetClass(), 0);
    }

    template<>
    template<>
    void to::test<9>()
    {
        liblas::LASClassification c;

        c.SetKeyPoint(true);
        ensure(c.IsKeyPoint());
        ensure(c != m_default);

        c.SetWithheld(true);
        ensure(c.IsWithheld());
        ensure(c.IsKeyPoint());
        ensure(c != m_default);
        
        c.SetSynthetic(true);
        ensure(c.IsWithheld());
        ensure(c.IsKeyPoint());
        ensure(c.IsSynthetic());
        ensure(c != m_default);

        ensure_equals(c.GetClass(), 0);
    }

    template<>
    template<>
    void to::test<10>()
    {
        liblas::LASClassification c;

        c.SetKeyPoint(true);
        c.SetSynthetic(true);
        c.SetWithheld(true);
        ensure(c.IsWithheld());
        ensure(c.IsKeyPoint());
        ensure(c.IsSynthetic());
        ensure_not(c == m_default);
        ensure_equals(c.GetClass(), 0);

        c.SetKeyPoint(false);
        c.SetSynthetic(false);
        c.SetWithheld(false);
        ensure_not(c.IsWithheld());
        ensure_not(c.IsKeyPoint());
        ensure_not(c.IsSynthetic());
        ensure_equals(c.GetClass(), 0);

        liblas::LASClassification::bitset_type bits1(c);
        liblas::LASClassification::bitset_type bits2(m_default);
        ensure_equals(c, m_default);
    }

    template<>
    template<>
    void to::test<11>()
    {
        liblas::LASClassification c;

        c.SetKeyPoint(true);
        c.SetClass(1);
        c.SetSynthetic(true);
        c.SetWithheld(true);
        ensure(c.IsWithheld());
        ensure(c.IsKeyPoint());
        ensure(c.IsSynthetic());
        ensure_equals(c.GetClass(), 1);
        ensure_not(c == m_default);
        
        c.SetKeyPoint(false);
        c.SetSynthetic(false);
        c.SetClass(0);
        c.SetWithheld(false);
        ensure_not(c.IsWithheld());
        ensure_not(c.IsKeyPoint());
        ensure_not(c.IsSynthetic());
        ensure_equals(c.GetClass(), 0);

        liblas::LASClassification::bitset_type bits1(c);
        liblas::LASClassification::bitset_type bits2(m_default);
        ensure_equals(c, m_default);
    }

    template<>
    template<>
    void to::test<12>()
    {
        std::string sbits("00000000");

        liblas::LASClassification c;

        std::ostringstream oss;
        oss << c;
        ensure_equals(oss.str(), sbits);
    }

    template<>
    template<>
    void to::test<13>()
    {
        std::string sbits("00000011");

        liblas::LASClassification c;
        c.SetClass(3);

        std::ostringstream oss;
        oss << c;
        ensure_equals(oss.str(), sbits);
    }

    template<>
    template<>
    void to::test<14>()
    {
        std::string sbits("10000001");

        liblas::LASClassification c;
        
        c.SetWithheld(true);
        c.SetClass(1);

        std::ostringstream oss;
        oss << c;
        ensure_equals(oss.str(), sbits);
    }

    template<>
    template<>
    void to::test<15>()
    {
        std::string sbits("10110000");

        liblas::LASClassification c;
        
        c.SetClass(16);
        c.SetSynthetic(true);
        c.SetKeyPoint(false);
        c.SetWithheld(true);

        std::ostringstream oss;
        oss << c;
        ensure_equals(oss.str(), sbits);
    }
}
