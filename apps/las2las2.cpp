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
#include <sstream>
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

#define SEPARATORS ",|"

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

bool IsDualRangeFilter(std::string parse_string) 
{

string::size_type dash = parse_string.find_first_of("-");

if (dash != std::string::npos) {
    return true;
}
return false;
}

liblas::FilterI*  MakeReturnFilter(std::string return_string, liblas::FilterI::FilterType ftype) 
{
    boost::char_separator<char> sep(SEPARATORS);

    std::vector<uint16_t> returns;
    tokenizer tokens(return_string, sep);
    for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
        returns.push_back(atoi((*t).c_str()));
    }

    liblas::ReturnFilter* return_filter = new ReturnFilter(returns, false);
    return_filter->SetType(ftype);
    return return_filter;
}


liblas::FilterI*  MakeClassFilter(std::string class_string, liblas::FilterI::FilterType ftype) 
{
    boost::char_separator<char> sep(SEPARATORS);

    std::vector<uint8_t> classes;
    tokenizer tokens(class_string, sep);
    for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
        classes.push_back(atoi((*t).c_str()));
    }

    liblas::ClassificationFilter* class_filter = new ClassificationFilter(classes); 
    class_filter->SetType(ftype);
    return class_filter;
}

liblas::FilterI*  MakeBoundsFilter(std::string bounds_string, liblas::FilterI::FilterType ftype) 
{
    boost::char_separator<char> sep(SEPARATORS);
    std::vector<double> vbounds;
    tokenizer tokens(bounds_string, sep);
    liblas::Bounds<double> bounds;
    for (tokenizer::iterator t = tokens.begin(); t != tokens.end(); ++t) {
        vbounds.push_back(atof((*t).c_str()));
    }
    if (vbounds.size() == 4) 
    {
        bounds = liblas::Bounds<double>(vbounds[0], 
                                vbounds[1], 
                                vbounds[2], 
                                vbounds[3]);
    } else if (vbounds.size() == 6)
    {
        bounds = liblas::Bounds<double>(vbounds[0], 
                                vbounds[1], 
                                vbounds[2], 
                                vbounds[3], 
                                vbounds[4], 
                                vbounds[5]);
    } else {
        ostringstream oss;
        oss << "Bounds must be specified as a 4-tuple or "
               "6-tuple, not a "<< vbounds.size()<<"-tuple" << "\n";
        throw std::runtime_error(oss.str());
    }
    liblas::BoundsFilter* bounds_filter = new BoundsFilter(bounds);
    bounds_filter->SetType(ftype);
    return bounds_filter;
}

liblas::FilterI*  MakeIntensityFilter(std::string intensities, liblas::FilterI::FilterType ftype) 
{
    liblas::ContinuousValueFilter<uint16_t>::filter_func f = &liblas::Point::GetIntensity;
    liblas::ContinuousValueFilter<uint16_t>* intensity_filter = new liblas::ContinuousValueFilter<uint16_t>(f, intensities);
    intensity_filter->SetType(ftype);
    return intensity_filter;
}


liblas::FilterI*  MakeTimeFilter(std::string times, liblas::FilterI::FilterType ftype) 
{
    liblas::ContinuousValueFilter<double>::filter_func f = &liblas::Point::GetTime;
    liblas::ContinuousValueFilter<double>* time_filter = new liblas::ContinuousValueFilter<double>(f, times);
    time_filter->SetType(ftype);
    return time_filter;
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
    
    std::cout << std::endl;
    
    delete writer;
    delete ofs;
    
    for(std::vector<liblas::FilterI*>::iterator i=filters.begin(); i!=filters.end(); i++)
    {
        delete *i;
    }
    
    return true;
}
int main(int argc, char* argv[])
{

    uint32_t split_size;
    uint32_t thin;
    std::string input;
    std::string output;



    // std::vector<boost::uint16_t> returns;
    
    // liblas::Bounds<double bounds;
    
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
            ("keep-intensity", po::value< string >(), "Range in which to keep intensity.\nThe following expression types are supported.  --keep-intensity 0-100\n --keep-intensity <200\n --keep-intensity >400\n--keep-intensity >=200")
            ("drop-intensity", po::value< string >(), "Range in which to drop intensity.\nThe following expression types are supported.  --drop-intensity <200\n --drop-intensity >400\n--drop-intensity >=200")
            ("keep-time", po::value< string >(), "Range in which to keep time.\nThe following expression types are supported.  --keep-time 413665.2336-414092.8462\n --keep-time <414094.8462\n --keep-time >413665.2336\n--keep-time >=413665.2336")
            ("drop-time", po::value< string >(), "Range in which to drop time.\nThe following expression types are supported.  --drop-time <413666.2336\n --drop-time >413665.2336\n--drop-time >=413665.2336")
            

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

        boost::char_separator<char> sep(SEPARATORS);

        if (vm.count("keep-classes")) 
        {
            std::string returns = vm["keep-classes"].as< string >();
            liblas::FilterI* return_filter = MakeClassFilter(  returns, 
                                                                liblas::FilterI::eInclusion);
            filters.push_back(return_filter); 
        }

        if (vm.count("drop-classes")) 
        {
            std::string returns = vm["drop-classes"].as< string >();
            liblas::FilterI* return_filter = MakeClassFilter(  returns, 
                                                                liblas::FilterI::eExclusion);
            filters.push_back(return_filter);
        }

        if (vm.count("keep-returns")) 
        {
            std::string returns = vm["keep-returns"].as< string >();
            liblas::FilterI* return_filter = MakeReturnFilter(  returns, 
                                                                liblas::FilterI::eInclusion);
            filters.push_back(return_filter); 
        }

        if (vm.count("drop-returns")) 
        {
            std::string returns = vm["drop-returns"].as< string >();
            liblas::FilterI* return_filter = MakeReturnFilter(  returns, 
                                                                liblas::FilterI::eExclusion);
            filters.push_back(return_filter); 
        }
                
        if (vm.count("extent")) 
        {
            std::string bounds = vm["extent"].as< string >();
            liblas::FilterI* bounds_filter = MakeBoundsFilter(bounds, liblas::FilterI::eInclusion);
            filters.push_back(bounds_filter);
            
        }
        if (vm.count("keep-intensity")) 
        {
            std::string intensities = vm["keep-intensity"].as< string >();
            if (IsDualRangeFilter(intensities)) {
                // We need to make two filters
                // Given a range 0-200, split the expression into two filters 
                string::size_type dash = intensities.find_first_of("-");
                std::string low = intensities.substr(0,dash);
                std::string high = intensities.substr(dash+1, intensities.size());

                liblas::FilterI* lt_filter = MakeIntensityFilter(">="+low, liblas::FilterI::eInclusion);
                filters.push_back(lt_filter);
                liblas::FilterI* gt_filter = MakeIntensityFilter("<="+high, liblas::FilterI::eInclusion);
                filters.push_back(gt_filter);                
            } else {
                liblas::FilterI* intensity_filter = MakeIntensityFilter(intensities, liblas::FilterI::eInclusion);
                filters.push_back(intensity_filter);
                
            }
        }
        if (vm.count("drop-intensity")) 
        {
            std::string intensities = vm["drop-intensity"].as< string >();
            if (IsDualRangeFilter(intensities)) {
                std::cerr << "Range filters are not supported for drop-intensity" << std::endl;
                return(1);
            } else {
                liblas::FilterI* intensity_filter = MakeIntensityFilter(intensities, liblas::FilterI::eExclusion);
                filters.push_back(intensity_filter);   
            }
        }
        if (vm.count("keep-time")) 
        {
            std::string times = vm["keep-time"].as< string >();
            if (IsDualRangeFilter(times)) {
                // We need to make two filters
                // Given a range 0-200, split the expression into two filters 
                string::size_type dash = times.find_first_of("-");
                std::string low = times.substr(0,dash);
                std::string high = times.substr(dash+1, times.size());

                liblas::FilterI* lt_filter = MakeTimeFilter(">="+low, liblas::FilterI::eInclusion);
                filters.push_back(lt_filter);
                liblas::FilterI* gt_filter = MakeTimeFilter("<="+high, liblas::FilterI::eInclusion);
                filters.push_back(gt_filter);                
            } else {
                liblas::FilterI* time_filter = MakeTimeFilter(times, liblas::FilterI::eInclusion);
                filters.push_back(time_filter);
                
            }
        }
        if (vm.count("drop-time")) 
        {
            std::string times = vm["drop-time"].as< string >();
            if (IsDualRangeFilter(times)) {
                std::cerr << "Range filters are not supported for drop-time" << std::endl;
                return(1);
            } else {
                liblas::FilterI* time_filter = MakeTimeFilter(times, liblas::FilterI::eExclusion);
                filters.push_back(time_filter);   
            }
        }
        if (thin > 0) 
        {
            liblas::ThinFilter* thin_filter = new ThinFilter(thin);
            filters.push_back(thin_filter);    
        }
        
        if (first_return_only && last_return_only) {
            std::cerr << "--first_return_only and --last_return_only cannot "
                         "be used simultaneously.  Use --keep-returns 1 in "
                         "combination with --last_return_only";
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
