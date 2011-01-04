//
// (C) Copyright 2010 Michael P. Gerlek (mpg@flaxen.com)
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//

#ifdef HAVE_LASZIPxxx

#include <liblas/liblas.hpp>
#include <tut/tut.hpp>
#include <cstdio>
#include <bitset>
#include <fstream>
#include <string>
#include "liblas_test.hpp"
#include "common.hpp"

namespace tut
{ 
    struct zipwriter_data
    {
        std::string file_laz;

        zipwriter_data() :
            file_laz(g_test_data_path + "//tmp.laz")
        {}

        ~zipwriter_data()
        {
            // remove temporary file after each test case
            int const ret = std::remove(file_laz.c_str());
            if (0 != ret)
            {
                ; // ignore, file may not exist
            }
        }
    };

    typedef test_group<zipwriter_data> tg;
    typedef tg::object to;

    tg test_group_zipwriter("liblas::ZipWriter");

    // Test ability to make a compressed header
    template<>
    template<>
    void to::test<1>()
    {
        // Create new LAS file using default header block
        {
            std::ofstream ofs;
            ofs.open(file_laz, std::ios::out | std::ios::binary);

            // LAS 1.2, Point Format 0
            liblas::Header header;
            ensure_equals(header.Compressed(), false);
            header.SetCompressed(true);
            ensure_equals(header.Compressed(), true);

            liblas::Writer writer(ofs, header);
        }

        // Read previously created LAS file and check its header block, laszip VLR
        {
            std::ifstream ifs;
            ifs.open(file_laz, std::ios::in | std::ios::binary);
            ensure(ifs.is_open());

            liblas::ReaderFactory factory;
            liblas::Reader reader = factory.CreateWithStream(ifs);
          
            ensure_equals(reader.GetHeader().Compressed(), true);

            test_laszip_vlr(reader.GetHeader());
        }

        return;
    }

    // Test ability to write a few points, using the writer factory
    template<>
    template<>
    void to::test<2>()
    {
        {
            std::ofstream ofs;
            ofs.open(file_laz, std::ios::out | std::ios::binary);

            // LAS 1.1, Point Format 0
            liblas::Header header;
            header.SetCompressed(true);

            liblas::Writer writer(ofs, header);

            liblas::Point point;

            // Write 1st point
            point.SetCoordinates(10, 20, 30);
            point.SetIntensity(5);
            point.SetReturnNumber(1);
            point.SetNumberOfReturns(1);
            point.SetScanDirection(1);
            point.SetFlightLineEdge(1);
            point.SetClassification(7);
            point.SetScanAngleRank(90);
            point.SetUserData(0);
            point.SetPointSourceID(1);

            writer.WritePoint(point);

            // write 2nd point
            point.SetCoordinates(40, 50, 60);
            point.SetPointSourceID(2);
            writer.WritePoint(point);

            // write 3rd point
            point.SetCoordinates(70, 80, 90);
            point.SetPointSourceID(3);
            writer.WritePoint(point);
        }

        // Read previously create LAS file with 3 point records
        {
            std::ifstream ifs;
            ifs.open(file_laz, std::ios::in | std::ios::binary);
            ensure(ifs.is_open());

            liblas::ReaderFactory factory;
            liblas::Reader reader = factory.CreateWithStream(ifs);
            
            ensure_equals(reader.GetHeader().Compressed(), true);

            liblas::Point point; // reusable cache

            // read 1st point
            bool ok = reader.ReadNextPoint();
            ensure_equals(ok, true);
            point = reader.GetPoint();

            ensure_distance(point.GetX(), 10.0, 0.1);
            ensure_distance(point.GetY(), 20.0, 0.1);
            ensure_distance(point.GetZ(), 30.0, 0.1);
            ensure_equals(point.GetIntensity(), 5);
            ensure_equals(point.GetReturnNumber(), 1);
            ensure_equals(point.GetNumberOfReturns(), 1);
            ensure_equals(point.GetScanDirection(), 1);
            ensure_equals(point.GetFlightLineEdge(), 1);
            ensure_equals(point.GetScanAngleRank(), 90);
            ensure_equals(point.GetUserData(), 0);
            ensure_equals(point.GetPointSourceID(), 1);

            typedef liblas::Classification::bitset_type bitset_type;
            ensure_equals(bitset_type(point.GetClassification()), bitset_type(7));

            // read 2nd point
            ok = reader.ReadNextPoint();
            ensure_equals(ok, true);
            point = reader.GetPoint();

            ensure_distance(point.GetX(), 40.0, 0.1);
            ensure_distance(point.GetY(), 50.0, 0.1);
            ensure_distance(point.GetZ(), 60.0, 0.1);
            ensure_equals(point.GetIntensity(), 5);
            ensure_equals(point.GetReturnNumber(), 1);
            ensure_equals(point.GetNumberOfReturns(), 1);
            ensure_equals(point.GetScanDirection(), 1);
            ensure_equals(point.GetFlightLineEdge(), 1);
            ensure_equals(point.GetScanAngleRank(), 90);
            ensure_equals(point.GetUserData(), 0);
            ensure_equals(point.GetPointSourceID(), 2);

            // read 3rd point
            ok = reader.ReadNextPoint();
            ensure_equals(ok, true);
            point = reader.GetPoint();

            ensure_distance(point.GetX(), 70.0, 0.1);
            ensure_distance(point.GetY(), 80.0, 0.1);
            ensure_distance(point.GetZ(), 90.0, 0.1);
            ensure_equals(point.GetIntensity(), 5);
            ensure_equals(point.GetReturnNumber(), 1);
            ensure_equals(point.GetNumberOfReturns(), 1);
            ensure_equals(point.GetScanDirection(), 1);
            ensure_equals(point.GetFlightLineEdge(), 1);
            ensure_equals(point.GetScanAngleRank(), 90);
            ensure_equals(point.GetUserData(), 0);
            ensure_equals(point.GetPointSourceID(), 3);

            typedef liblas::Classification::bitset_type bitset_type;
            ensure_equals(bitset_type(point.GetClassification()), bitset_type(7));

            return;
        }
    }
}

#endif // HAVE_LASZIP
