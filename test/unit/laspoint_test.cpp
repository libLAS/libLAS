// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/laspoint.hpp>
#include <tut/tut.hpp>
#include <bitset>

namespace tut
{ 
    struct laspoint_data
    {
        liblas::LASPoint m_default;
    };

    typedef test_group<laspoint_data> tg;
    typedef tg::object to;

    tg test_group_laspoint("liblas::LASPoint");

    // Test default construction
    template<>
    template<>
    void to::test<1>()
    {
        ensure_equals("wrong default X coordinate",
                      m_default.GetX(), double(0));
        ensure_equals("wrong default Y coordinate",
                      m_default.GetY(), double(0));
        ensure_equals("wrong default Z coordinate",
                      m_default.GetZ(), double(0));
        ensure_equals("wrong defualt intensity",
                      m_default.GetIntensity(), 0);
        ensure_equals("wrong defualt return number",
                      m_default.GetReturnNumber(), 0);
        ensure_equals("wrong defualt number of returns",
                      m_default.GetNumberOfReturns(), 0);
        ensure_equals("wrong defualt scan direction",
                      m_default.GetScanDirection(), 0);
        ensure_equals("wrong defualt edge of flight line",
                      m_default.GetFlightLineEdge(), 0);
        ensure_equals("wrong defualt classification",
                      m_default.GetClassification(), 0);
        ensure_equals("wrong defualt scan angle rank",
                      m_default.GetScanAngleRank(), 0);
        ensure_equals("wrong defualt user data value",
                      m_default.GetUserData(), 0);
        ensure_equals("wrong defualt time",
                      m_default.GetTime(), double(0));
    }

    // Test copy constructor
    template<>
    template<>
    void to::test<2>()
    {
        liblas::LASPoint p(m_default);

        ensure_equals("wrong X coordinate",
                      p.GetX(), double(0));
        ensure_equals("wrong Y coordinate",
                      p.GetY(), double(0));
        ensure_equals("wrong Z coordinate",
                      p.GetZ(), double(0));
        ensure_equals("wrong intensity",
                      p.GetIntensity(), 0);
        ensure_equals("wrong return number",
                      p.GetReturnNumber(), 0);
        ensure_equals("wrong number of returns",
                      p.GetNumberOfReturns(), 0);
        ensure_equals("wrong scan direction",
                      p.GetScanDirection(), 0);
        ensure_equals("wrong edge of flight line",
                      p.GetFlightLineEdge(), 0);
        ensure_equals("wrong classification",
                      p.GetClassification(), 0);
        ensure_equals("wrong scan angle rank",
                      m_default.GetScanAngleRank(), 0);
        ensure_equals("wrong user data value",
                      m_default.GetUserData(), 0);
        ensure_equals("wrong time",
                      p.GetTime(), double(0));
    }

    // Test assignment operator
    template<>
    template<>
    void to::test<3>()
    {
        liblas::LASPoint p;
        p = m_default;

        ensure_equals("wrong X coordinate",
                      p.GetX(), double(0));
        ensure_equals("wrong Y coordinate",
                      p.GetY(), double(0));
        ensure_equals("wrong Z coordinate",
                      p.GetZ(), double(0));
        ensure_equals("wrong intensity",
                      p.GetIntensity(), 0);
        ensure_equals("wrong return number",
                      p.GetReturnNumber(), 0);
        ensure_equals("wrong number of returns",
                      p.GetNumberOfReturns(), 0);
        ensure_equals("wrong scan direction",
                      p.GetScanDirection(), 0);
        ensure_equals("wrong edge of flight line",
                      p.GetFlightLineEdge(), 0);
        ensure_equals("wrong classification",
                      p.GetClassification(), 0);
        ensure_equals("wrong scan angle rank",
                      p.GetScanAngleRank(), 0);
        ensure_equals("wrong user data value",
                      p.GetUserData(), 0);
        ensure_equals("wrong time",
                      p.GetTime(), double(0));
    }

    // Test equal-to operator
    template<>
    template<>
    void to::test<4>()
    {
        liblas::LASPoint p;
        
        ensure("points are not equal", m_default == p);
    }

    // Test not-equal-to operator
    template<>
    template<>
    void to::test<5>()
    {
        liblas::LASPoint p;
        p.SetCoordinates(1.123, 2.456, 3.789);
        
        ensure("points are equal", m_default != p);
    }

    // Test Get/SetReturnNumber
    template<>
    template<>
    void to::test<6>()
    {
        ensure_equals("invalid default return number",
                      m_default.GetReturnNumber(), 0);

        liblas::uint16_t const num1 = 7;
        m_default.SetReturnNumber(num1);
        ensure_equals("invalid return number",
                      m_default.GetReturnNumber(), num1);

        liblas::uint16_t const num2 = 3;
        m_default.SetReturnNumber(num2);
        ensure_equals("invalid return number",
                      m_default.GetReturnNumber(), num2);

        try
        {
            liblas::uint16_t const outofrange = 8;
            m_default.SetReturnNumber(outofrange);

            ensure("std::out_of_range not thrown", false);
        }
        catch (std::out_of_range const& e)
        {
            ensure(e.what(), true);
        }
    }

    // Test Get/SetNumberOfReturns
    template<>
    template<>
    void to::test<7>()
    {
        ensure_equals("invalid default number of returns",
                      m_default.GetNumberOfReturns(), 0);

        liblas::uint16_t const num1 = 7;
        m_default.SetNumberOfReturns(num1);
        ensure_equals("invalid number of returns",
                      m_default.GetNumberOfReturns(), num1);

        liblas::uint16_t const num2 = 3;
        m_default.SetNumberOfReturns(num2);
        ensure_equals("invalid number of returns",
                      m_default.GetNumberOfReturns(), num2);

        try
        {
            liblas::uint16_t const outofrange = 8;
            m_default.SetNumberOfReturns(outofrange);

            ensure("std::out_of_range not thrown", false);
        }
        catch (std::out_of_range const& e)
        {
            ensure(e.what(), true);
        }
    }

    // Test Get/SetScanDirection
    template<>
    template<>
    void to::test<8>()
    {
        ensure_equals("invalid default scan direction flag",
                      m_default.GetScanDirection(), 0);

        liblas::uint16_t const positive = 1;
        m_default.SetScanDirection(positive);
        ensure_equals("invalid scan direction flag",
                      m_default.GetScanDirection(), positive);

        liblas::uint16_t const negative = 0;
        m_default.SetScanDirection(negative);
        ensure_equals("invalid scan direction flag",
                      m_default.GetScanDirection(), negative);

        try
        {
            liblas::uint16_t const outofrange = 2;
            m_default.SetScanDirection(outofrange);

            ensure("std::out_of_range not thrown", false);
        }
        catch (std::out_of_range const& e)
        {
            ensure(e.what(), true);
        }
    }

    // Test Get/SetFlightLineEdge
    template<>
    template<>
    void to::test<9>()
    {
        ensure_equals("invalid default edge of flight line",
                      m_default.GetFlightLineEdge(), 0);

        liblas::uint16_t const endofscan = 1;
        m_default.SetFlightLineEdge(endofscan);
        ensure_equals("invalid edge of flight line",
                      m_default.GetFlightLineEdge(), endofscan);

        liblas::uint16_t const notendofscan = 0;
        m_default.SetFlightLineEdge(notendofscan);
        ensure_equals("invalid edge of flight line",
                      m_default.GetFlightLineEdge(), notendofscan);

        try
        {
            liblas::uint16_t const outofrange = 2;
            m_default.SetFlightLineEdge(outofrange);

            ensure("std::out_of_range not thrown", false);
        }
        catch (std::out_of_range const& e)
        {
            ensure(e.what(), true);
        }
    }

    // Test Get/SetScanFlags
    template<>
    template<>
    void to::test<10>()
    {
        std::bitset<8> zeros;
        std::bitset<8> bits = m_default.GetScanFlags();
        
        ensure_equals("invalid default scan flags", bits, zeros);

        m_default.SetReturnNumber(3);
        m_default.SetNumberOfReturns(7);
        m_default.SetScanDirection(0);
        m_default.SetFlightLineEdge(1);

        std::bitset<8> expected(std::string("10111011"));
        bits = m_default.GetScanFlags();
        
        ensure_equals("invalid scan flags pattern", bits, expected);

        liblas::LASPoint copy(m_default);
        bits = copy.GetScanFlags();
        
        ensure_equals("invalid copy of scan flags pattern", bits, expected);
    }

    // Test Get/SetScanAngleRank
    template<>
    template<>
    void to::test<11>()
    {
        ensure_equals("invalid default scan angle rank",
                      m_default.GetScanAngleRank(), 0);

        liblas::int8_t const rank1 = -90;
        m_default.SetScanAngleRank(rank1);
        ensure_equals("invalid scan angle rank",
                      m_default.GetScanAngleRank(), rank1);

        liblas::int8_t const rank2 = 90;
        m_default.SetScanAngleRank(rank2);
        ensure_equals("invalid scan angle rank",
                      m_default.GetScanAngleRank(), rank2);

        try
        {
            liblas::int8_t const outofrange = 91;
            m_default.SetScanAngleRank(outofrange);

            ensure("std::out_of_range not thrown", false);
        }
        catch (std::out_of_range const& e)
        {
            ensure(e.what(), true);
        }
    }

    // Test Get/SetUserData
    template<>
    template<>
    void to::test<12>()
    {
        ensure_equals("invalid default user data value",
                      m_default.GetUserData(), 0);

        liblas::uint8_t const data = 7; // dummy value
        m_default.SetUserData(data);

        ensure_equals("invalid user data value",
                      m_default.GetUserData(), data);
    }
}

