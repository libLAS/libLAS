// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/lasreader.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/liblas.hpp>
#include <tut/tut.hpp>
#include <fstream>
#include <string>
#include "liblas_test.hpp"

namespace tut
{ 
    struct lasreader_data
    {
        std::string file10_;

        lasreader_data()
            : file10_(g_test_data_path + "//TO_core_last_clip.las")
        {}

        void test_file10_point1(liblas::LASPoint const& p)
        {
            ensure_distance(p.GetX(), double(630262.30), 0.0001);
            ensure_distance(p.GetY(), double(4834500), 0.0001);
            ensure_distance(p.GetZ(), double(51.53), 0.0001);
            ensure_equals(p.GetIntensity(), 670);
            ensure_equals(p.GetClassification(), liblas::uint8_t(1));
            ensure_equals(p.GetScanAngleRank(), 0);
            ensure_equals(p.GetUserData(), 3);
            ensure_equals(static_cast<unsigned int>(p.GetScanFlags()), 9);
            ensure_distance(p.GetTime(), double(413665.23360000004), 0.0001);
        }

        void test_file10_point2(liblas::LASPoint const& p)
        {
            ensure_distance(p.GetX(), double(630282.45), 0.0001);
            ensure_distance(p.GetY(), double(4834500), 0.0001);
            ensure_distance(p.GetZ(), double(51.63), 0.0001);
            ensure_equals(p.GetIntensity(), 350);
            ensure_equals(p.GetClassification(), 1);
            ensure_equals(p.GetScanAngleRank(), 0);
            ensure_equals(p.GetUserData(), 3);
            ensure_equals(static_cast<unsigned int>(p.GetScanFlags()), 9);
            ensure_distance(p.GetTime(), double(413665.52880000003), 0.0001);
        }

        void test_file10_point4(liblas::LASPoint const& p)
        {
            ensure_distance(p.GetX(), double(630346.83), 0.0001);
            ensure_distance(p.GetY(), double(4834500), 0.0001);
            ensure_distance(p.GetZ(), double(50.90), 0.0001);
            ensure_equals(p.GetIntensity(), 150);
            ensure_equals(p.GetClassification(), 1);
            ensure_equals(p.GetScanAngleRank(), 0);
            ensure_equals(p.GetUserData(), 4);
            ensure_equals(static_cast<unsigned int>(p.GetScanFlags()), 18);
            ensure_distance(p.GetTime(), double(414093.84360000002), 0.0001);
        }
    };

    typedef test_group<lasreader_data> tg;
    typedef tg::object to;

    tg test_group_lasreader("liblas::LASReader");

    // Test user-declared constructor
    template<>
    template<>
    void to::test<1>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);

        ensure_equals(reader.GetVersion(), liblas::eLASVersion10);
    }

    // Test reading header
    template<>
    template<>
    void to::test<2>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::LASHeader const& hdr = reader.GetHeader();

        ensure_equals(hdr.GetFileSignature(), liblas::LASHeader::FileSignature);
        ensure_equals(hdr.GetFileSourceId(), 0);
        ensure_equals(hdr.GetReserved(), 0);

        liblas::guid g;
        ensure(g.is_null());
        ensure_equals(hdr.GetProjectId(), g);

        ensure_equals(hdr.GetVersionMajor(), 1);
        ensure_equals(hdr.GetVersionMinor(), 0);
        ensure_equals(hdr.GetSystemId(), std::string(""));
        ensure_equals(hdr.GetSoftwareId(), std::string("TerraScan"));
        ensure_equals(hdr.GetCreationDOY(), 0);
        ensure_equals(hdr.GetCreationYear(), 0);
        ensure_equals(hdr.GetHeaderSize(), liblas::uint16_t(227));
        ensure_equals(hdr.GetDataOffset(), liblas::uint32_t(229));
        ensure_equals(hdr.GetRecordsCount(), liblas::uint32_t(0));
        ensure_equals(hdr.GetDataFormatId(), liblas::LASHeader::ePointFormat1);
        ensure_equals(hdr.GetDataRecordLength(), liblas::LASHeader::ePointSize1);
        ensure_equals(hdr.GetPointRecordsCount(), liblas::uint32_t(8));
        ensure_equals(hdr.GetScaleX(), double(0.01));
        ensure_equals(hdr.GetScaleY(), double(0.01));
        ensure_equals(hdr.GetScaleZ(), double(0.01));
        ensure_equals(hdr.GetOffsetX(), double(-0));
        ensure_equals(hdr.GetOffsetY(), double(-0));
        ensure_equals(hdr.GetOffsetZ(), double(-0));
        ensure_equals(hdr.GetMinX(), double(630262.3));
        ensure_equals(hdr.GetMaxX(), double(630346.83));
        ensure_equals(hdr.GetMinY(), double(4834500));
        ensure_equals(hdr.GetMaxY(), double(4834500));
        ensure_equals(hdr.GetMinZ(), double(50.9));
        ensure_equals(hdr.GetMaxZ(), double(55.26));
    }

    // Test GetPoint method
    template<>
    template<>
    void to::test<3>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);

        // uninitialized point object, a null-point
        liblas::LASPoint const& p = reader.GetPoint();
        ensure(p == liblas::LASPoint());
    }

    // Test ReadPoint and GetPoint pair
    template<>
    template<>
    void to::test<4>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);

        // read 1st point
        reader.ReadNextPoint();
        test_file10_point1(reader.GetPoint());

        // read 2nd point
        reader.ReadNextPoint();
        test_file10_point2(reader.GetPoint());

        // read and skip 3rd point
        reader.ReadNextPoint();

        // read 4th point
        reader.ReadNextPoint();
        test_file10_point4(reader.GetPoint());

        // read and count remaining points from 5 to 8
        unsigned short c = 0;
        while (reader.ReadNextPoint())
        {
            ++c;
        }
        ensure_equals(c, 4);
    }

    // Test ReadPointAt and GetPoint pair
    template<>
    template<>
    void to::test<5>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);

        // read 1st point
        reader.ReadPointAt(0);
        test_file10_point1(reader.GetPoint());

        // read 4th point
        reader.ReadPointAt(3);
        test_file10_point4(reader.GetPoint());

        // read back to 2nd point
        reader.ReadPointAt(1);
        test_file10_point2(reader.GetPoint());
    }

    // Test operator[] and GetPoint pair
    template<>
    template<>
    void to::test<6>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);

        // read 1st point
        test_file10_point1(reader[0]);

        // read 4th point
        test_file10_point4(reader[3]);

        // read back to 2nd point
        test_file10_point2(reader[1]);
    }

    // Test GetStream method
    template<>
    template<>
    void to::test<7>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);

        std::istream& is = reader.GetStream();

        std::istream const* const p1 = &ifs;
        std::istream const* const p2 = &is;
        ensure_equals(ifs, is); // same streams
    }
}

