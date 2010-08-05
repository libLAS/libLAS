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
#include <functional>

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
                std::vector<liblas::FilterI*>& filters,
                uint32_t split_size)
{


    
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
    int fileno = 2;

    while (reader.ReadNextPoint())
    {
        liblas::Point const& p = reader.GetPoint(); 
        writer->WritePoint(p);  
        term_progress(std::cout, (i + 1) / static_cast<double>(size));
        i++;

        split_bytes_count = split_bytes_count - reader.GetHeader().GetSchema().GetByteSize();        
        if (split_bytes_count < 0 && split_size > 0) {
            // The user specifies a split size in mb, and we keep counting 
            // down until we've written that many points into the file.  
            // After that point, we make a new file and start writing into 
            // that.  
            // FIXME. No header accounting is done at this time.
            delete writer;
            delete ofs;
            
            ofs = new std::ofstream;
            ostringstream oss;
            oss << out << "-"<< fileno <<".las";

            writer = start_writer(ofs, oss.str(), reader.GetHeader());
            fileno++;
            split_bytes_count = 1024*1024*split_size;
        }
    }
    
    delete writer;
    delete ofs;
    
    //FIXME: clean up filters
    return true;
}
int main(int argc, char* argv[])
{

    uint32_t split_size;
    uint32_t thin;
    std::string input;
    std::string output;



    std::vector<boost::uint8_t> drop_returns;
    
    liblas::Bounds bounds;
    
    bool last_return_only;
    bool first_return_only;
    bool valid_only;
    std::vector<liblas::FilterI*> filters;    

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
            ("thin,t", po::value<uint32_t>(&thin)->default_value(0), "Simple decimation-style thinning.\nThin the file by removing every t'th point from the file.")
            ("last_return_only", po::value<bool>(&last_return_only)->zero_tokens(), "Keep last returns (cannot be used with --first_return_only)")
            ("first_return_only", po::value<bool>(&first_return_only)->zero_tokens(), "Keep first returns (cannot be used with --last_return_only")
            ("keep-returns", po::value< string >(), "Return numbers to keep.\nUse a comma-separated list, for example, --keep-returns 1\nUse --last_return_only or --first_return_only if you want to ensure getting either one of these.")
            ("drop-returns", po::value< string >(), "Return numbers to drop.\nUse a comma-separated list, for example, --drop-returns 2,3,4,5\nUse --last_return_only or --first_return_only if you want to ensure getting either one of these.")
            ("valid_only", po::value<bool>(&valid_only)->zero_tokens(), "Keep only valid points")
            ("keep-intensity", po::value< string >(), "Range in which to keep intensity.\nUse a comma-separated list, for example, --keep-intensity 0-100 --keep-intensity <200\n")
            

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

        boost::char_separator<char> sep(",|");

        if (vm.count("keep-classes")) 
        {
            std::vector<boost::uint8_t> keep_classes;
            tokenizer tokens(vm["keep-classes"].as< string >(), sep);
            for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
                keep_classes.push_back(atoi((*t).c_str()));
            }
            liblas::ClassificationFilter* class_filter = new ClassificationFilter(keep_classes);
            class_filter->SetType(liblas::FilterI::eInclusion);
            filters.push_back(class_filter);
        }

        if (vm.count("drop-classes")) 
        {
            std::vector<boost::uint8_t> drop_classes;            
            tokenizer tokens(vm["drop-classes"].as< string >(), sep);
            for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
                drop_classes.push_back(atoi((*t).c_str()));
            }
            liblas::ClassificationFilter* class_filter = new ClassificationFilter(drop_classes);
            class_filter->SetType(liblas::FilterI::eExclusion);
            filters.push_back(class_filter);       
        }

        if (vm.count("keep-returns")) 
        {
            std::vector<boost::uint16_t> keep_returns;
            tokenizer tokens(vm["keep-returns"].as< string >(), sep);
            for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
                keep_returns.push_back(atoi((*t).c_str()));
            }
            liblas::ReturnFilter* return_filter = new ReturnFilter(keep_returns, false);
            return_filter->SetType(liblas::FilterI::eInclusion);
            filters.push_back(return_filter);
        }

        if (vm.count("drop-returns")) 
        {
            std::vector<boost::uint16_t> drop_returns;
            tokenizer tokens(vm["drop-returns"].as< string >(), sep);
            for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
                drop_returns.push_back(atoi((*t).c_str()));
            }
            liblas::ReturnFilter* return_filter = new ReturnFilter(drop_returns, false);
            return_filter->SetType(liblas::FilterI::eInclusion);
            filters.push_back(return_filter); 
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
            liblas::BoundsFilter* bounds_filter = new BoundsFilter(bounds);
            filters.push_back(bounds_filter);
            
        }
        if (vm.count("keep-intensity")) 
        {
            std::string intensities = vm["keep-intensity"].as< string >();
            boost::function<uint16_t (const Point*) > f =&liblas::Point::GetIntensity;

            liblas::ContinuousValueFilter<uint16_t>* intensity_filter = new liblas::ContinuousValueFilter<uint16_t>(f, atoi(intensities.c_str()));
            intensity_filter->SetType(liblas::FilterI::eInclusion);
            filters.push_back(intensity_filter);
        }

        if (thin > 0) 
        {
            liblas::ThinFilter* thin_filter = new ThinFilter(thin);
            filters.push_back(thin_filter);    
        }
        
        if (first_return_only && last_return_only) {
            std::cerr << "--first_return_only and --last_return_only cannot be used simultaneously.  Use --keep-returns";
            return 1;
        }

        if (last_return_only){
            std::vector<boost::uint16_t> returns;
            liblas::ReturnFilter* last_filter = new ReturnFilter(returns, true);
            filters.push_back(last_filter);
        }
    
        if (first_return_only){
            std::vector<boost::uint16_t> returns;
            returns.push_back(1);
            liblas::ReturnFilter* return_filter = new ReturnFilter(returns, false);
            filters.push_back(return_filter);
        }
        
        if (valid_only){
            liblas::ValidationFilter* valid_filter = new ValidationFilter();
            filters.push_back(valid_filter);            
        }
        
        
        if (vm.count("input")) 
        {
            input = vm["input"].as< string >();
        } else {
            std::cerr << "Input LAS file not specified!\n" << desc << "\n";
            return 1;
        } 
        
        bool op = process(  input, 
                            output, 
                            filters,
                            split_size
                            );
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
