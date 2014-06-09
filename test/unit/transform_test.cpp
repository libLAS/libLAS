// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/bounds.hpp>
#include <tut/tut.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include "common.hpp"
#include "liblas_test.hpp"

namespace tut
{ 
    struct lastransform_data
    {
        std::string gdal_datasource;
        std::string lidar_data;

        lastransform_data()
            : gdal_datasource(g_test_data_path + "//autzen.jpg")
            , lidar_data(g_test_data_path + "//autzen.las")
        {}
    };

  
    typedef test_group<lastransform_data> tg;
    typedef tg::object to;

    tg test_group_lastransform("liblas::TransformI");


#ifdef HAVE_GDAL

    // Test default constructor
    template<>
    template<>
    void to::test<1>()
    {
        liblas::Header header;
        liblas::TransformPtr color_fetch;

        {
            std::ifstream ifs;
            ifs.open(lidar_data.c_str(), std::ios::in | std::ios::binary);
            liblas::Reader reader(ifs);
            header = reader.GetHeader();
        }

        {
            header.SetDataFormatId(liblas::ePointFormat3);
            header.SetVersionMinor(2);
        
            std::vector<boost::uint32_t> bands;
            bands.resize(3);
            bands[0] = 1;
            bands[1] = 2;
            bands[2] = 3;
            color_fetch = liblas::TransformPtr(new liblas::ColorFetchingTransform(gdal_datasource, bands, &header));
        }
        
            std::ifstream ifs;
            ifs.open(lidar_data.c_str(), std::ios::in | std::ios::binary);
            liblas::Reader reader(ifs);
            std::vector<liblas::TransformPtr> transforms;
            transforms.push_back(color_fetch);
            reader.SetTransforms(transforms);
            
            reader.ReadNextPoint();
            
            liblas::Point const& p = reader.GetPoint();
            
            ensure_equals("Red is incorrect for point", p.GetColor().GetRed(), 210);
            ensure_equals("Green is incorrect for point", p.GetColor().GetGreen(), 205);
            ensure_equals("Blue is incorrect for point", p.GetColor().GetBlue(), 185);
        
    }

#endif
}
