// $Id$
//
// ts2las translates TerraSolid .bin file to ASPRS LAS file.
//
// TerraSolid format: http://cdn.terrasolid.fi/tscan.pdf
//
// (C) Copyright Howard Butler 2009, hobu.inc@gmail.com
//
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
// liblas
#include <liblas/liblas.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <string>

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

namespace po = boost::program_options;

using namespace liblas;
using namespace std;

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif



bool term_progress(std::ostream& os, double complete)
{
    static int lastTick = -1;
    int tick = static_cast<int>(complete * 40.0);

    tick = std::min(40, std::max(0, tick));

    // Have we started a new progress run?  
    if (tick < lastTick && lastTick >= 39)
        lastTick = -1;

    if (tick <= lastTick)
        return true;

    while (tick > lastTick)
    {
        lastTick++;
        if (lastTick % 4 == 0)
            os << (lastTick / 4) * 10;
        else
            os << ".";
    }

    if( tick == 40 )
        os << " - done.\n";
    else
        os.flush();

    return true;
}




liblas::Writer* start_writer(   std::ofstream* strm, 
                                std::string const& output, 
                                liblas::Header const& header)
{
    
    if (!liblas::Create(*strm, output.c_str()))
    {
        std::cerr << "Cannot create " << output << "for write.  Exiting...";
    
    }        
    liblas::Writer* writer = new liblas::Writer(*strm, header);
    return writer;
    
}


bool process(   std::string const& input,
                std::string const& output,
                liblas::Bounds const& bounds,
                uint32_t split_size,
                std::vector<uint8_t> keep_classes,
                std::vector<uint8_t> drop_classes)
{

    std::vector<liblas::FilterI*> filters;

 
    //
    // Source
    //
    std::cout << "Processing:" << "\n - dataset: " << input << std::endl;

    // Make the filters
    
    if (keep_classes.size() > 0) {
        liblas::ClassificationFilter* class_filter = new ClassificationFilter(keep_classes);
        class_filter->SetType(liblas::FilterI::eInclusion);
        filters.push_back(class_filter);
        
    }
    
    if (drop_classes.size() > 0)
    {
        liblas::ClassificationFilter* class_filter = new ClassificationFilter(drop_classes);
        class_filter->SetType(liblas::FilterI::eExclusion);
        filters.push_back(class_filter);        
    }
    
    if (bounds.dimension() > 0) {
        liblas::BoundsFilter* bounds_filter = new BoundsFilter(bounds);
        filters.push_back(bounds_filter);
    }
    
    std::ifstream ifs;
    if (!liblas::Open(ifs, input.c_str()))
    {
        std::cerr << "Cannot open " << input << "for read.  Exiting...";
        return false;
    }
    liblas::Reader reader(ifs);
    
    reader.SetFilters(&filters);

    std::ofstream* ofs = new std::ofstream;
    std::string out = output;
    liblas::Writer* writer = 0;
    if (!split_size) {
        writer = start_writer(ofs, output, reader.GetHeader());
        
    } else {
        string::size_type dot_pos = output.find_first_of(".");
        out = output.substr(0, dot_pos);
        writer = start_writer(ofs, out+"-1"+".las", reader.GetHeader());
    }


    std::cout << "Target:" 
        << "\n - : " << output
        << std::endl;

    //
    // Translation of points cloud to features set
    //
    boost::uint32_t i = 0;
    boost::uint32_t const size = reader.GetHeader().GetPointRecordsCount();
    
    boost::int32_t split_bytes_count = 1024*1024*split_size;
    std::cout << "count: " << split_bytes_count;
    int k = 2;
    while (reader.ReadNextPoint())
    {
        liblas::Point const& p = reader.GetPoint(); 
        writer->WritePoint(p);  
        split_bytes_count = split_bytes_count - reader.GetHeader().GetSchema().GetByteSize();
        term_progress(std::cout, (i + 1) / static_cast<double>(size));
        i++;
        if (split_bytes_count < 0) {
            delete writer;
            delete ofs;
            
            ofs = new std::ofstream;
            ostringstream oss;
            oss << out << "-"<< k <<".las";

            // out = std::string(oss.str());
            writer = start_writer(ofs, oss.str(), reader.GetHeader());
            k++;
            split_bytes_count = 1024*1024*split_size;
        }
    }
    
    delete writer;
    delete ofs;
    return true;
}
int main(int argc, char* argv[])
{

    uint32_t split_size;
    std::string input;
    std::string output;

    std::vector<boost::uint8_t> keep_classes;
    std::vector<boost::uint8_t> drop_classes;
    liblas::Bounds bounds;
    
    try {

        po::options_description desc("Allowed options");
        po::positional_options_description p;
        p.add("input", 1);
        p.add("output", 1);

        desc.add_options()
            ("help", "produce help message")
            ("split,s", po::value<uint32_t>(&split_size)->default_value(0), "Split file into multiple files with each being this size in MB or less. If this value is 0, no splitting is done")
            ("input,i", po::value< string >(), "input LAS file")
            ("output,o", po::value< string >(&output)->default_value("output.las"), "output LAS file")
            ("keep-classes,k", po::value< string >(), "Classifications to keep.\nUse a comma-separated list, for example, -k 2,4,12")
            ("drop-classes,d", po::value< string >(), "Classifications to drop.\nUse a comma-separated list, for example, -d 1,7,8")
            ("extent,e", po::value< string >(), "Extent window that points must fall within to keep.\nUse a comma-separated list, for example, \n  -e minx, miny, maxx, maxy\n  or \n  -e minx, miny, maxx, maxy, minz, maxz")

    ;
    
        po::variables_map vm;        
        po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);

        po::notify(vm);

        if (vm.count("help")) 
        {
            std::cout << desc << "\n";
            return 1;
        }

        boost::char_separator<char> sep(",");

        if (vm.count("keep-classes")) 
        {
            tokenizer tokens(vm["keep-classes"].as< string >(), sep);
            for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
                keep_classes.push_back(atoi((*t).c_str()));
            }
        }

        if (vm.count("drop-classes")) 
        {
            tokenizer tokens(vm["drop-classes"].as< string >(), sep);
            for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
                drop_classes.push_back(atoi((*t).c_str()));
            }
        }
        
        if (vm.count("extent")) 
        {
            std::vector<double> vbounds;
            tokenizer tokens(vm["extent"].as< string >(), sep);
            for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
                vbounds.push_back(atof((*t).c_str()));
            }
            if (vbounds.size() == 4) 
            {
                bounds = liblas::Bounds(vbounds[0], vbounds[1], vbounds[2], vbounds[3]);
            } else if (vbounds.size() == 6)
            {
                bounds = liblas::Bounds(vbounds[0], vbounds[1], vbounds[2], vbounds[3], vbounds[4], vbounds[5]);
            } else {
                std::cerr << "Bounds must be specified as a 4-tuple or 6-tuple, not a "<< vbounds.size()<<"-tuple" << "\n";
                return 1;
            }
        }

        if (vm.count("input")) 
        {
            input = vm["input"].as< string >();
        } else {
            std::cerr << "Input LAS file not specified!\n" << desc << "\n";
            return 1;
        } 
        
        bool op = process(input, output, bounds, split_size, keep_classes, drop_classes);
        if (!op) {
            return (1);
        }
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }
    
    return 0;


    //     std::cout << std::endl;
    // }
    // catch (std::exception const& e)
    // {
    //     std::cerr << "Error: " << e.what() << std::endl;
    //     rc = -1;
    // }
    // catch (...)
    // {
    //     std::cerr << "Unknown error\n";
    //     rc = -1;
    // }
    // return rc;
}

//las2las2 -i lt_srs_rt.las  -o foo.las -c 1,2 -b 2483590,366208,2484000,366612
