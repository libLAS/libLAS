/***************************************************************************
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS translation with optional configuration
 * Author:  Howard Butler, hobu.inc at gmail.com
 ***************************************************************************
 * Copyright (c) 2010, Howard Butler, hobu.inc at gmail.com 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#include <liblas/liblas.hpp>
#include "laskernel.hpp"

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>


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
        std::ostringstream oss;
        oss << "Cannot create " << output << "for write.  Exiting...";
        throw std::runtime_error(oss.str());
    }        
    liblas::Writer* writer = new liblas::Writer(*strm, header);
    return writer;
    
}


void RepairHeader(liblas::Summary const& summary, std::string const& filename)
{
    std::ifstream ifs;
    if (!liblas::Open(ifs, filename.c_str()))
    {
        std::ostringstream oss;
        oss << "Cannot open " << filename << "for read.  Exiting...";
        throw std::runtime_error(oss.str());
    }
    liblas::Reader reader(ifs);
    liblas::Header header = reader.GetHeader();
    ifs.close();

    for (boost::uint32_t i = 0; i < 5; i++)
    {
        header.SetPointRecordsByReturnCount(i, 0);
    }    

    std::ios::openmode m = std::ios::out | std::ios::in | std::ios::binary | std::ios::ate;

    // Write a blank header first
    std::ofstream ofs(filename.c_str(), m);
    liblas::Writer writer(ofs, header);
    ofs.close();
    
    liblas::property_tree::ptree tree = summary.GetPTree();
    
    try
    {
        header.SetMin(tree.get<double>("minimum.x"),
                      tree.get<double>("minimum.y"),
                      tree.get<double>("minimum.z"));
    
        header.SetMax(tree.get<double>("maximum.x"),
                      tree.get<double>("maximum.y"),
                      tree.get<double>("maximum.z"));

        for (boost::uint32_t i = 0; i < 5; i++)
        {
            header.SetPointRecordsByReturnCount(i, 0);
        }
    
        BOOST_FOREACH(ptree::value_type &v,
                tree.get_child("points_by_return"))
        {
            boost::uint32_t i = v.second.get<boost::uint32_t>("id");
            boost::uint32_t count = v.second.get<boost::uint32_t>("count");
            header.SetPointRecordsByReturnCount(i-1, count);        
        } 
        
    }     catch (liblas::property_tree::ptree_bad_path const& e) 
    {
        std::cerr << "Unable to write summarized header info.  Does the outputted file have any points?";
        return;
    }
    
    // Write our updated header with summary info
    std::ofstream ofs2(filename.c_str(), m);
    liblas::Writer writer2(ofs2, header);
    ofs2.close();
    
}

void SetStreamPrecision(std::ostream& os, double scale)
{
    os.setf(std::ios_base::fixed, std::ios_base::floatfield);

    double frac = 0;
    double integer = 0;
    frac = std::modf(scale, &integer);

    boost::uint32_t prec = static_cast<boost::uint32_t>(std::fabs(std::floor(std::log10(frac))));
    os.precision(prec);    
}

bool process(   std::string const& input,
                std::string const& output,
                liblas::Header & header,
                std::vector<liblas::FilterPtr>& filters,
                std::vector<liblas::TransformPtr>& transforms,
                boost::uint32_t split_size,
                bool verbose,
                bool min_offset)
{


    
    std::ifstream ifs;
    if (!liblas::Open(ifs, input.c_str()))
    {
        std::cerr << "Cannot open " << input << "for read.  Exiting...";
        return false;
    }
    liblas::Reader reader(ifs);
    liblas::Summary summary;
    
    reader.SetFilters(filters);
    reader.SetTransforms(transforms);

    if (min_offset) 
    {
        
        liblas::property_tree::ptree tree = reader.Summarize();
    
        try
        {
            header.SetOffset(tree.get<double>("minimum.x"),
                             tree.get<double>("minimum.y"),
                             tree.get<double>("minimum.z"));
    
                              
        }     catch (liblas::property_tree::ptree_bad_path const& e) 
        {
            std::cerr << "Unable to write minimum header info.  Does the outputted file have any points?";
            return false;
        }
        if (verbose) 
        {
            
    
            std::cout << "Using minimum offsets ";
            SetStreamPrecision(std::cout, header.GetScaleX());
            std::cout << header.GetOffsetX() << " ";
            SetStreamPrecision(std::cout, header.GetScaleY());
            std::cout << header.GetOffsetY() << " ";
            SetStreamPrecision(std::cout, header.GetScaleZ());
            std::cout << header.GetOffsetZ() << " ";
            std::cout << std::endl;
        }
        reader.Reset();
    }

    std::ofstream* ofs = new std::ofstream;
    std::string out = output;
    liblas::Writer* writer = 0;
    if (!split_size) {
        writer = start_writer(ofs, output, header);
        
    } else {
        string::size_type dot_pos = output.find_first_of(".");
        out = output.substr(0, dot_pos);
        writer = start_writer(ofs, out+"-1"+".las", header);
    }

    if (verbose)
    std::cout << "Writing output:" 
        << "\n - : " << output
        << std::endl;

    //
    // Translation of points cloud to features set
    //
    boost::uint32_t i = 0;
    boost::uint32_t const size = header.GetPointRecordsCount();
    
    boost::int32_t split_bytes_count = 1024*1024*split_size;
    int fileno = 2;

    while (reader.ReadNextPoint())
    {
        liblas::Point const& p = reader.GetPoint();
        summary.AddPoint(p);
        writer->WritePoint(p);
        if (verbose)
            term_progress(std::cout, (i + 1) / static_cast<double>(size));
        i++;

        split_bytes_count = split_bytes_count - header.GetSchema().GetByteSize();        
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

            writer = start_writer(ofs, oss.str(), header);
            fileno++;
            split_bytes_count = 1024*1024*split_size;
        }
    }
    if (verbose)
        std::cout << std::endl;
    
    reader.Reset();
    liblas::property_tree::ptree pts = summary.GetPTree();
    liblas::property_tree::ptree top;
    top.add_child("summary.header",reader.GetHeader().GetPTree());
    top.add_child("summary.points",pts);
    liblas::property_tree::write_xml("junk.xml", top);
    liblas::property_tree::write_xml("schema.xml", reader.GetHeader().GetSchema().GetPTree());

    std::cout <<  reader.GetHeader().GetSchema() << std::endl;
    delete writer;
    delete ofs;
    
    RepairHeader(summary, output);
    
    return true;
}

void OutputHelp( std::ostream & oss, po::options_description const& options)
{
    oss << "--------------------------------------------------------------------\n";
    oss << "    las2las (" << GetFullVersion() << ")\n";
    oss << "--------------------------------------------------------------------\n";

    oss << options;

    oss <<"\nFor more information, see the full documentation for las2las2 at:\n";
    
    oss << " http://liblas.org/utilities/las2las2.html\n";
    oss << "----------------------------------------------------------\n";

}

int main(int argc, char* argv[])
{

    boost::uint32_t split_size;
    std::string input;
    std::string output;
    
    bool verbose = false;
    bool bMinOffset = false;
    
    std::vector<liblas::FilterPtr> filters;
    std::vector<liblas::TransformPtr> transforms;
    
    liblas::Header header;

    // try {

        po::options_description file_options("las2las2 options");
        po::options_description filtering_options = GetFilteringOptions();
        po::options_description transform_options = GetTransformationOptions() ;

        po::positional_options_description p;
        p.add("input", 1);
        p.add("output", 1);

        file_options.add_options()
            ("help,h", "produce help message")
            ("split,s", po::value<boost::uint32_t>(&split_size)->default_value(0), "Split file into multiple files with each being this size in MB or less. If this value is 0, no splitting is done")
            ("input,i", po::value< string >(), "input LAS file")
            ("output,o", po::value< string >(&output)->default_value("output.las"), "output LAS file")
            ("verbose,v", po::value<bool>(&verbose)->zero_tokens(), "Verbose message output")
        ;

        po::variables_map vm;
        po::options_description options;
        options.add(file_options).add(transform_options).add(filtering_options);
        po::store(po::command_line_parser(argc, argv).
          options(options).positional(p).run(), vm);

        po::notify(vm);

        if (vm.count("help")) 
        {
            OutputHelp(std::cout, options);
            return 1;
        }

        if (vm.count("input")) 
        {
            input = vm["input"].as< string >();
            std::ifstream ifs;
            if (verbose)
                std::cout << "Opening " << input << " to fetch Header" << std::endl;
            if (!liblas::Open(ifs, input.c_str()))
            {
                std::cerr << "Cannot open " << input << "for read.  Exiting...";
                return 1;
            }
            liblas::Reader reader(ifs);
            header = reader.GetHeader();
        } else {
            std::cerr << "Input LAS file not specified!\n";
            OutputHelp(std::cout, options);
            return 1;
        }

        if (vm.count("min-offset")) 
        {
            bMinOffset = true;
        } 

        filters = GetFilters(vm, verbose);
        
        // Transforms alter our header as well.  Setting scales, offsets, etc.
        transforms = GetTransforms(vm, verbose, header);
        
        bool op = process(  input, 
                            output,
                            header, 
                            filters,
                            transforms,
                            split_size,
                            verbose,
                            bMinOffset
                            );
        if (!op) {
            return (1);
        }
        
    // }
    // catch(std::exception& e) {
    //     std::cerr << "error: " << e.what() << "\n";
    //     return 1;
    // }
    // catch(...) {
    //     std::cerr << "Exception of unknown type!\n";
    // }
    
    return 0;


}

//las2las2 -i lt_srs_rt.las  -o foo.las -c 1,2 -b 2483590,366208,2484000,366612
