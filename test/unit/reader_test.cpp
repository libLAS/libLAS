// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//

#include <liblas/liblas.hpp>
#include <tut/tut.hpp>
#include <fstream>
#include <string>
#include "liblas_test.hpp"
#include "common.hpp"

namespace tut
{ 
    struct lasreader_data
    {
        std::string file10_;
        std::string file12_;

        lasreader_data()
            : file10_(g_test_data_path + "//TO_core_last_clip.las")
            , file12_(g_test_data_path + "//certainty3d-color-utm-feet-navd88.las")
        {}
    };

    typedef test_group<lasreader_data> tg;
    typedef tg::object to;

    tg test_group_lasreader("liblas::Reader");

    // Test user-declared constructor
    template<>
    template<>
    void to::test<1>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::Reader reader(ifs);

        ensure_equals(reader.GetHeader().GetVersionMinor(), 0);
    }

    // Test reading header
    template<>
    template<>
    void to::test<2>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::Reader reader(ifs);
        liblas::Header const& hdr = reader.GetHeader();

        test_file10_header(hdr);
    }

    // Test GetPoint method
    template<>
    template<>
    void to::test<3>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::Reader reader(ifs);

        // uninitialized point object, a null-point
        liblas::Point const& p = reader.GetPoint();
        liblas::Point t(&liblas::DefaultHeader::get());
        ensure(p == t);
    }

    // Test ReadPoint and GetPoint pair
    template<>
    template<>
    void to::test<4>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::Reader reader(ifs);

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
        liblas::Reader reader(ifs);

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
        liblas::Reader reader(ifs);

        // read 1st point
        test_file10_point1(reader[0]);

        // read 4th point
        test_file10_point4(reader[3]);

        // read back to 2nd point
        test_file10_point2(reader[1]);
    }

    // Test seek and GetPoint pair
    template<>
    template<>
    void to::test<7>()
    {
        std::ifstream ifs;
        ifs.open(file10_.c_str(), std::ios::in | std::ios::binary);
        liblas::Reader reader(ifs);

        // read 1st point
        reader.ReadNextPoint();
        test_file10_point1(reader.GetPoint());
        
        // seek to 4th point
        reader.Seek(4);

        // read 4th point
        test_file10_point4(reader[3]);

    }

    // Test summarization and valid values
    template<>
    template<>
    void to::test<8>()
    {
        std::ifstream ifs;
        ifs.open(file12_.c_str(), std::ios::in | std::ios::binary);
        liblas::Reader reader(ifs);
        
        liblas::Summary summary;
        liblas::CoordinateSummary coord_summary;
        
        bool read = reader.ReadNextPoint();
        while (read)
        {
            summary.AddPoint(reader.GetPoint());
            coord_summary.AddPoint(reader.GetPoint());
            read = reader.ReadNextPoint();
        }

        liblas::property_tree::ptree tree = summary.GetPTree();
        
        ensure_equals("Point count not correct", 
                       tree.get<boost::uint32_t>("summary.points.count"),
                       static_cast<boost::uint32_t>(10126));
                       
        ensure_distance("Min X does not match", 
                        tree.get<double>("summary.points.minimum.x"), 
                        6326726.536120,
                        0.000001);

        ensure_distance("Min Y does not match", 
                        tree.get<double>("summary.points.minimum.y"), 
                        2068062.385430,
                        0.000001);

        ensure_distance("Min Z does not match", 
                        tree.get<double>("summary.points.minimum.z"), 
                        2700.5303501,
                        0.000001);


        ensure_distance("Max X does not match", 
                        tree.get<double>("summary.points.maximum.x"), 
                        6330162.951062,
                        0.000001);

        ensure_distance("Max Y does not match", 
                        tree.get<double>("summary.points.maximum.y"), 
                        2071932.240223,
                        0.000001);

        ensure_distance("Max Z does not match", 
                        tree.get<double>("summary.points.maximum.z"), 
                        2975.7118862,
                        0.000001);

        ensure_equals("Min red not correct", 
                       tree.get<boost::uint16_t>("summary.points.minimum.color.red"),
                       static_cast<boost::uint16_t>(4096)); 
        ensure_equals("Min green not correct", 
                       tree.get<boost::uint16_t>("summary.points.minimum.color.green"),
                       static_cast<boost::uint16_t>(2304));                                              
        ensure_equals("Min blue not correct", 
                       tree.get<boost::uint16_t>("summary.points.minimum.color.blue"),
                       static_cast<boost::uint16_t>(0)); 

        ensure_equals("Max red not correct", 
                       tree.get<boost::uint16_t>("summary.points.maximum.color.red"),
                       static_cast<boost::uint16_t>(65280)); 
        ensure_equals("Max green not correct", 
                       tree.get<boost::uint16_t>("summary.points.maximum.color.green"),
                       static_cast<boost::uint16_t>(64512));                                              
        ensure_equals("Max blue not correct", 
                       tree.get<boost::uint16_t>("summary.points.maximum.color.blue"),
                       static_cast<boost::uint16_t>(56320));   
        
        tree = coord_summary.GetPTree();
        ensure_distance("Coordinate Summary Min X does not match", 
                        tree.get<double>("summary.points.minimum.x"), 
                        6326726.536120,
                        0.000001);

        ensure_distance("Coordinate Summary Min Y does not match", 
                        tree.get<double>("summary.points.minimum.y"), 
                        2068062.385430,
                        0.000001);

        ensure_distance("Coordinate Summary Min Z does not match", 
                        tree.get<double>("summary.points.minimum.z"), 
                        2700.5303501,
                        0.000001);


        ensure_distance("Coordinate Summary Max X does not match", 
                        tree.get<double>("summary.points.maximum.x"), 
                        6330162.951062,
                        0.000001);

        ensure_distance("Coordinate Summary Max Y does not match", 
                        tree.get<double>("summary.points.maximum.y"), 
                        2071932.240223,
                        0.000001);

        ensure_distance("Coordinate Summary Max Z does not match", 
                        tree.get<double>("summary.points.maximum.z"), 
                        2975.7118862,
                        0.000001);

    }

}

