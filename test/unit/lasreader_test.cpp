// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/lasreader.hpp>
#include <liblas/lasheader.hpp>
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
        lasreader_data()
            : file10_(g_test_data_path + "//TO_core_last_clip.las")
        {}
        std::string file10_;
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

}

