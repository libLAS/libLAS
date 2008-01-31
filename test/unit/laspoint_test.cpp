// $Id$
#include <liblas/laspoint.hpp>
#include <tut/tut.hpp>
namespace tut
{ 
    struct laspoint_data
    {
        liblas::LASPoint m_default;
    };

    typedef test_group<laspoint_data> tg;
    typedef tg::object to;

    tg test_group("liblas::LASPoint");

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
        ensure_equals("wrong time",
                      p.GetTime(), double(0));
    }
};
