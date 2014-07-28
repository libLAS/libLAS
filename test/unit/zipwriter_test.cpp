//
// (C) Copyright 2010 Michael P. Gerlek (mpg@flaxen.com)
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//

#ifdef HAVE_LASZIP

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
        std::string header_10;
        std::string header_10_vlr;
        std::string header_12;
        std::string header_12_vlr;
        std::string header_10_compressed;
        std::string header_10_compressed_vlr;
        std::string header_12_compressed;
        std::string header_12_compressed_vlr;
        std::string header_12_padded;
        std::string header_12_too_small_pad;

        zipwriter_data() :
            file_laz(g_test_data_path + "//tmp.laz"),
            header_10(g_test_data_path + "//header-1.0-test.las"),
            header_10_vlr(g_test_data_path + "//header-1.0-vlr-test.las"),
            header_12(g_test_data_path + "//header-1.2-test.las"),
            header_12_vlr(g_test_data_path + "//header-1.2-vlr-test.las"),
            header_10_compressed(g_test_data_path + "//header-1.0-compressed-test.laz"),
            header_10_compressed_vlr(g_test_data_path + "//header-1.0-compressed-test.laz"),
            header_12_compressed(g_test_data_path + "//header-1.2-compressed-test.laz"),
            header_12_compressed_vlr(g_test_data_path + "//header-1.2-compressed-test.laz"),
            header_12_padded(g_test_data_path + "//header-1.2-padded.laz"),
            header_12_too_small_pad(g_test_data_path + "//header-1.2-too-small-pad.laz")
        {}

        ~zipwriter_data()
        {
            cleanup(file_laz);
            cleanup(header_10);
            cleanup(header_10_vlr);
            cleanup(header_12);
            cleanup(header_12_vlr);
            cleanup(header_10_compressed);
            cleanup(header_10_compressed_vlr);
            cleanup(header_12_compressed);
            cleanup(header_12_compressed_vlr);
            cleanup(header_12_padded);
            cleanup(header_12_too_small_pad);

        }
        
        void cleanup(std::string const& input)
        {
            const int ret = std::remove(input.c_str());
            if (0 != ret)
            {
                ; // ignore, file may not exist
            }
        }
        
        void setup(  std::string const& input, 
                liblas::Header const& header)
        {

            std::ofstream ofs;
            ofs.open(input.c_str(), std::ios::out | std::ios::binary);

            liblas::Writer writer(ofs, header);

            liblas::Point point(&writer.GetHeader());

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
        
        liblas::Header FetchHeader(std::string input)
        {
            std::ifstream ifs;
            ifs.open(input.c_str(), std::ios::in | std::ios::binary);
            ensure(ifs.is_open());

            liblas::ReaderFactory factory;
            liblas::Reader reader = factory.CreateWithStream(ifs);
            return reader.GetHeader();
        }
        
        liblas::VariableRecord ConstructVLR()
        {
            liblas::VariableRecord r;
            
            std::vector<boost::uint8_t> vdata;
            vdata.resize(256);
            for(int i=0; i < 256; i++)
            {
                vdata[i] = (boost::uint8_t)i;
            }
            r.SetReserved(0xAABB);
            r.SetUserId("HOBU");
            r.SetRecordId(1234);
            r.SetDescription("some lovely text");
            r.SetData(vdata);

            r.SetRecordLength((boost::uint16_t)vdata.size());
            return r;
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
            ofs.open(file_laz.c_str(), std::ios::out | std::ios::binary);

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
            ifs.open(file_laz.c_str(), std::ios::in | std::ios::binary);
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
            ofs.open(file_laz.c_str(), std::ios::out | std::ios::binary);

            // LAS 1.1, Point Format 0
            liblas::Header header;
            header.SetCompressed(true);

            liblas::Writer writer(ofs, header);

            liblas::Point point(&writer.GetHeader());

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
            ifs.open(file_laz.c_str(), std::ios::in | std::ios::binary);
            ensure(ifs.is_open());

            liblas::ReaderFactory factory;
            liblas::Reader reader = factory.CreateWithStream(ifs);
            
            ensure_equals(reader.GetHeader().Compressed(), true);

            liblas::Point point(&reader.GetHeader()); // reusable cache

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

    // Test header integrity
    template<>
    template<>
    void to::test<3>()
    {

        {
            liblas::Header header;

            header.SetVersionMinor(0);
            header.SetHeaderPadding(0);
            header.SetCompressed(false);

            setup(header_10, header);
        }

        {
            liblas::Header header = FetchHeader(header_10);
            
            ensure_equals("1.0 no-vlr iscompressed?", header.Compressed(), false);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.0 no-vlr data offset", header.GetDataOffset(), (boost::uint32_t)229);
            ensure_equals("1.0 no-vlr header padding", header.GetHeaderPadding(), (boost::uint32_t)2);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(0);
            header.SetHeaderPadding(0);
            header.SetCompressed(false);
            liblas::VariableRecord v = ConstructVLR();
            header.AddVLR(v);

            setup(header_10_vlr, header);
        }

        {
            liblas::Header header = FetchHeader(header_10_vlr);
            
            ensure_equals("1.0 vlr iscompressed?", header.Compressed(), false);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.0 vlr data offset", header.GetDataOffset(), (boost::uint32_t)539);
            ensure_equals("1.0 vlr header padding", header.GetHeaderPadding(), (boost::uint32_t)2);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(2);
            header.SetHeaderPadding(0);
            header.SetCompressed(false);

            setup(header_12, header);
        }

        {
            liblas::Header header = FetchHeader(header_12);
            
            ensure_equals("1.2 no-vlr iscompressed?", header.Compressed(), false);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.2 no-vlr data offset", header.GetDataOffset(), (boost::uint32_t)227);
            ensure_equals("1.2 no-vlr header padding", header.GetHeaderPadding(), (boost::uint32_t)0);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(2);
            header.SetHeaderPadding(0);
            header.SetCompressed(false);
            liblas::VariableRecord v = ConstructVLR();
            header.AddVLR(v);

            setup(header_12_vlr, header);
        }

        {
            liblas::Header header = FetchHeader(header_12_vlr);
            
            ensure_equals("1.2 vlr iscompressed?", header.Compressed(), false);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.2 vlr data offset", header.GetDataOffset(), (boost::uint32_t)537);
            ensure_equals("1.2 vlr header padding", header.GetHeaderPadding(), (boost::uint32_t)0);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(0);
            header.SetHeaderPadding(0);
            header.SetCompressed(true);

            setup(header_10_compressed, header);
        }

        {
            liblas::Header header = FetchHeader(header_10_compressed);
            
            ensure_equals("1.0 no-vlr compressed iscompressed?", header.Compressed(), true);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.0 no-vlr compressed data offset", header.GetDataOffset(), (boost::uint32_t)335);
            ensure_equals("1.0 no-vlr compressed header padding", header.GetHeaderPadding(), (boost::uint32_t)2);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(2);
            header.SetHeaderPadding(0);
            header.SetCompressed(true);

            setup(header_12_compressed, header);
        }

        {
            liblas::Header header = FetchHeader(header_12_compressed);
            
            ensure_equals("1.2 no-vlr compressed iscompressed?", header.Compressed(), true);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.2 no-vlr compressed data offset", header.GetDataOffset(), (boost::uint32_t)333);
            ensure_equals("1.2 no-vlr compressed header padding", header.GetHeaderPadding(), (boost::uint32_t)0);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(2);
            header.SetHeaderPadding(0);
            header.SetCompressed(true);
            liblas::VariableRecord v = ConstructVLR();
            header.AddVLR(v);

            setup(header_12_compressed_vlr, header);
        }

        {
            liblas::Header header = FetchHeader(header_12_compressed_vlr);
            
            ensure_equals("1.2 vlr compressed iscompressed?", header.Compressed(), true);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.2 vlr compressed data offset", header.GetDataOffset(), (boost::uint32_t)643);
            ensure_equals("1.2 vlr compressed header padding", header.GetHeaderPadding(), (boost::uint32_t)0);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(2);
            header.SetHeaderPadding(0);
            header.SetCompressed(true);
            header.SetDataOffset(1024);
            liblas::VariableRecord v = ConstructVLR();
            header.AddVLR(v);

            setup(header_12_padded, header);
        }

        {
            liblas::Header header = FetchHeader(header_12_padded);
            
            ensure_equals("1.2 header_12_padded iscompressed?", header.Compressed(), true);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.2 header_12_padded data offset", header.GetDataOffset(), (boost::uint32_t)1024);
            ensure_equals("1.2 header_12_padded header padding", header.GetHeaderPadding(), (boost::uint32_t)381);

        }

        {
            liblas::Header header;

            header.SetVersionMinor(2);
            header.SetHeaderPadding(0);
            header.SetCompressed(true);
            header.SetDataOffset(440);
            liblas::VariableRecord v = ConstructVLR();
            header.AddVLR(v);

            setup(header_12_too_small_pad, header);
        }

        {
            liblas::Header header = FetchHeader(header_12_too_small_pad);
            
            ensure_equals("1.2 header_12_too_small_pad iscompressed?", header.Compressed(), true);
            
            // compression is going to add the LASzip VLR
            ensure_equals("1.2 header_12_too_small_pad data offset", header.GetDataOffset(), (boost::uint32_t)643);
            ensure_equals("1.2 header_12_too_small_pad header padding", header.GetHeaderPadding(), (boost::uint32_t)0);

        }
    }
}

#endif // HAVE_LASZIP
