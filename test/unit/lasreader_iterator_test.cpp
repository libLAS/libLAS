// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/iterator.hpp>
#include <liblas/cstdint.hpp>
#include <tut/tut.hpp>
#include <fstream>
#include <iterator>
#include "liblas_test.hpp"
#include "common.hpp"

namespace tut
{ 
    struct lasreader_iterator_data
    {
        std::string file10_;

        lasreader_iterator_data()
            : file10_(g_test_data_path + "//TO_core_last_clip.las")
        {}
    };

    typedef test_group<lasreader_iterator_data> tg;
    typedef tg::object to;

    tg test_group_lasreader_iterator("liblas::lasreader_iterator");

    // Test default constructor
    template<>
    template<>
    void to::test<1>()
    {
        liblas::lasreader_iterator it;
    }

    // Test user-defined constructor
    template<>
    template<>
    void to::test<2>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader);
    }

    // Test copy constructor with default initialized iterator
    template<>
    template<>
    void to::test<3>()
    {
        liblas::lasreader_iterator it1;
        liblas::lasreader_iterator it2(it1);

        ensure(it1 == it2);
    }

    // Test copy constructor with initialized iterator
    template<>
    template<>
    void to::test<4>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it1(reader);
        liblas::lasreader_iterator it2(it1);

        ensure(it1 == it2);
    }

    // Test assignment operator with default initialized iterator
    template<>
    template<>
    void to::test<5>()
    {
        liblas::lasreader_iterator it1;
        liblas::lasreader_iterator it2;
        it1 = it2;

        ensure(it1 == it2);
    }

    // Test assignment operator with initialized iterator
    template<>
    template<>
    void to::test<6>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it1(reader);
        liblas::lasreader_iterator it2;
        it1 = it2;

        ensure(it1 == it2);
    }

    // Test dereference operator
    template<>
    template<>
    void to::test<7>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader);

        test_file10_point1(*it);
    }

 // Test pointer-to-member operator
    template<>
    template<>
    void to::test<8>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader);

        // test 1st point data record 
        ensure_distance(it->GetX(), double(630262.30), 0.0001);
        ensure_distance(it->GetY(), double(4834500), 0.0001);
        ensure_distance(it->GetZ(), double(51.53), 0.0001);
        ensure_equals(it->GetIntensity(), 670);
        ensure_equals(it->GetClassification(), liblas::uint8_t(1));
        ensure_equals(it->GetScanAngleRank(), 0);
        ensure_equals(it->GetUserData(), 3);
        ensure_equals(it->GetScanFlags(), 9);
        ensure_distance(it->GetTime(), double(413665.23360000004), 0.0001);
    }

    // Test pre-increment operator
    template<>
    template<>
    void to::test<9>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader); // move to 1st point
        ++it; // move to 2nd record

        test_file10_point2(*it);
    }

    // Test post-increment operator
    template<>
    template<>
    void to::test<10>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader); // move to 1st point
        it++; // move to 2nd record

        test_file10_point2(*it);
    }

    // Test equal-to operator
    template<>
    template<>
    void to::test<11>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader); // move to 1st point
        liblas::lasreader_iterator end;

        ensure_not(end == it);
    }

    // Test not-equal-to operator
    template<>
    template<>
    void to::test<12>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader); // move to 1st point
        liblas::lasreader_iterator end;

        ensure(end != it);
    }

    // Test iteration
    template<>
    template<>
    void to::test<13>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::uint32_t const cnt = reader.GetHeader().GetPointRecordsCount();
        liblas::lasreader_iterator it(reader); // move to 1st point
        liblas::lasreader_iterator end;

        liblas::uint32_t s = 0;
        while (end != it)
        {
            s++;
            ++it;
        }

        ensure_equals(cnt, s);
    }

    // Test std::distance
    template<>
    template<>
    void to::test<14>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::uint32_t const cnt = reader.GetHeader().GetPointRecordsCount();
        liblas::lasreader_iterator it(reader); // move to 1st point
        liblas::lasreader_iterator end;

        ensure_equals(std::distance(it, end), cnt);
    }

    // Test std::advance
    template<>
    template<>
    void to::test<15>()
    {
        std::ifstream ifs(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::LASReader reader(ifs);
        liblas::lasreader_iterator it(reader); // move to 1st point

        std::advance(it, 1); // move to 2nd record
        test_file10_point2(*it);

        std::advance(it, 2); // move to 4th record
        test_file10_point4(*it);
    }
}

