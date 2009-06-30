#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>

#include <liblas/laswriter.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lascolor.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/index/index.hpp>

#ifdef HAVE_BOOST
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/copy.hpp> 
#include <boost/range/iterator_range.hpp>
#include <boost/format.hpp> 
#endif

#ifdef HAVE_SPATIALINDEX
#include <spatialindex/SpatialIndex.h>
#endif

#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <sstream>

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif

std::istream* OpenInput(std::string filename) 
{
    std::ios::openmode const mode = std::ios::in | std::ios::binary;
    std::istream* istrm;
    if (compare_no_case(filename.c_str(),"STDIN",5) == 0)
    {
        istrm = &std::cin;
    }
    else 
    {
        istrm = new std::ifstream(filename.c_str(), mode);
    }
    
    if (!istrm->good())
    {
        delete istrm;
        throw std::runtime_error("Reading stream was not able to be created");
        exit(1);
    }
    return istrm;
}

void usage() {}

using namespace liblas;

int main(int argc, char* argv[])
{
    int rc = 0;

    std::string input;
    std::string output;
    
    for (int i = 1; i < argc; i++)
    {
        if (    strcmp(argv[i],"-h") == 0 ||
                strcmp(argv[i],"--help") == 0
            )
        {
            usage();
            exit(0);
        }
        else if (   strcmp(argv[i],"--input") == 0  ||
                    strcmp(argv[i],"-input") == 0   ||
                    strcmp(argv[i],"-i") == 0       ||
                    strcmp(argv[i],"-in") == 0
                )
        {
            i++;
            input = std::string(argv[i]);
        }
        else if (   strcmp(argv[i],"--output") == 0  ||
                    strcmp(argv[i],"--out") == 0     ||
                    strcmp(argv[i],"-out") == 0     ||
                    strcmp(argv[i],"-o") == 0       
                )
        {
            i++;
            output = std::string(argv[i]);
        }
        else if (i == argc - 2 && output.empty() && input.empty())
        {
            input = std::string(argv[i]);
        }
        else if (i == argc - 1 && output.empty() && input.empty())
        {
            input = std::string(argv[i]);
        }
        else if (i == argc - 1 && output.empty() && input.empty())
        {
            output = std::string(argv[i]);
        }
        else 
        {
            usage();
            exit(1);
        }
    }
    
    if (input.empty()) {
        usage();
        exit(-1);
    }
    std::cout << "input: " << input<<  " output: " <<output<<std::endl;
    // 
    std::istream* istrm = OpenInput(input);
    LASReader* reader = new LASReader(*istrm);
    
    
//     LASIndexDataStream* idxstrm = new LASIndexDataStream(reader);
// 
//     LASIndex* index = new LASIndex( input);
//     index->Initialize(*idxstrm);
//     
//     delete idxstrm;
//     delete index;
//     delete reader;
//     delete istrm;
//     
//     LASIndex* idx = new LASIndex(input);
//     
//     idx->Initialize();
//     std::vector<liblas::uint32_t>* ids = 0;
// 
//     try{
// //        ids = idx->intersects(289815.12,4320979.06, 289818.01,4320982.59,46.83,170.65);
// 
// // _zoom
// //        ids = idx->intersects(630355.0,4834609.0,630395.0,4834641.0,0.0,200.0);
// 
// // _clip
// // 630346.830000,4834500.000000,55.260000
//         ids = idx->intersects(630262.300000,4834500.000000,630346.830000,4834500.000000,50.900000,55.260000);
// //       ids = idx->intersects(630297.0,4834497.0,630302.0,4834501.0,0.0,200.0);
// 
//     } catch (Tools::Exception& e) {
//         std::string s = e.what();
//         std::cout << "error querying index value" << s <<std::endl; exit(1);
//     }
//         
//     
//     if (ids != 0) delete ids;
//     if (idx != 0) delete idx;


    LASIndexDataStream* idxstrm = new LASIndexDataStream(reader);

    
    std::string nothing = std::string("");
    LASIndex* idx = new LASIndex(nothing);
    idx->SetType(LASIndex::eMemoryIndex);
    idx->Initialize(*idxstrm);
    std::vector<liblas::uint32_t>* ids = 0;

    try{
//        ids = idx->intersects(289815.12,4320979.06, 289818.01,4320982.59,46.83,170.65);

// _zoom
//        ids = idx->intersects(630355.0,4834609.0,630395.0,4834641.0,0.0,200.0);

// _clip
// 630346.830000,4834500.000000,55.260000
        ids = idx->intersects(630262.300000,4834500.000000,630346.830000,4834500.000000,50.900000,55.260000);
//       ids = idx->intersects(630297.0,4834497.0,630302.0,4834501.0,0.0,200.0);

    } catch (Tools::Exception& e) {
        std::string s = e.what();
        std::cout << "error querying index value" << s <<std::endl; exit(1);
    }
    
    std::cout << "hits: ";
    for (int i=0;i<ids->size();i++) {
        std::cout << ids->at(i) <<",";
    }
    std::cout << std::endl;
    // 
    // if (ids != 0) delete ids;
    // if (idx != 0) delete idx;
    // idx = 0;
    // 
    namespace io = boost::iostreams;

    // io::filtering_ostream ofilter;
    // std::stringstream  junk;

    // std::vector<liblas::uint8_t> compressed;
    
    std::string compressed;
    io::filtering_streambuf<io::output> ofilter; 
    ofilter.push(io::zlib_compressor());
    ofilter.push(back_inserter(compressed));
    
    LASVariableRecord *vlr = idx->GetVLR();

    std::string data("some junkdsfasdfasdfqwerasdfasdfasdfasdfasdfweradsfasdfasdfasdfasdfasdqwerasdfasdfasdfasdfqwerasdfasdfv");
    // std::string data(vlr->GetData());
    std::cout << "uncompressed size " << data.size() << std::endl;
    
    io::copy(boost::make_iterator_range(data),ofilter);
 
    // std::vector<liblas::uint8_t>&  d= *(vlr->GetData());
    // io::copy(boost::make_iterator_range(d.begin(), d.end()),ofilter);

    std::cout << "compressed size " << compressed.size() << std::endl;
    // 
    // ofilter.push(io::zlib_compressor());
    // ofilter.push(back_inserter(compressed));
    // 
    // LASVariableRecord *vlr = idx->GetVLR();
    // std::cout << "VLR length: " << vlr->GetRecordLength() << std::endl;
    // 
    // ofilter << "some junkdsfasdfasdfqwerasdfv";
    // 
    // // ofilter << *vlr;
    // std::cout << "stream size: " << ofilter.size() << std::endl;
    // 
    // ofilter.flush();
    // 
    // std::cout << "compressed size: " << compressed.size() << std::endl;
    //  
    // 
    // 
    
}
