// $Id$
//
// lasblock generates block output for las2oci to store lidar data in OPC tables
//
//
// (C) Copyright Howard Butler 2010, hobu.inc@gmail.com
//
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/liblas.hpp>
#include <liblas/chipper.hpp>
#include "laskernel.hpp"

// std
#include <fstream>
#include <vector>

// boost
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4512)
#endif

#include <boost/program_options.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace po = boost::program_options;

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif

typedef boost::shared_ptr<liblas::Writer> WriterPtr;
typedef boost::shared_ptr<liblas::CoordinateSummary> SummaryPtr;
typedef boost::shared_ptr<std::ofstream> OStreamPtr;


liblas::Writer* start_writer(   std::ostream*& ofs, 
                      std::string const& output, 
                      liblas::Header const& header)
{
    ofs = liblas::Create(output, std::ios::out | std::ios::binary);
    if (!ofs)
    {
        std::ostringstream oss;
        oss << "Cannot create " << output << "for write.  Exiting...";
        throw std::runtime_error(oss.str());
    }

    return new liblas::Writer(*ofs, header);
}

using namespace liblas;

void OutputHelp( std::ostream & oss, po::options_description const& options)
{
    oss << "--------------------------------------------------------------------\n";
    oss << "    lasblock (" << GetFullVersion() << ")\n";
    oss << "--------------------------------------------------------------------\n";

    oss << options;

    oss <<"\nFor more information, see the full documentation for lasblock at:\n";
    
    oss << " http://liblas.org/utilities/lasblock.html\n";
    oss << "----------------------------------------------------------\n";

}


void write_tiles(std::string& output, 
                 liblas::chipper::Chipper& c, 
                 liblas::Reader& reader, 
                 bool verbose,
                 bool bCompressed)
{


    std::string out = output;
    liblas::Header header = reader.GetHeader();
    
    if (bCompressed)
        header.SetCompressed(true);
    
    std::string::size_type dot_pos = output.find_first_of(".");
    out = output.substr(0, dot_pos);
    

    if (verbose)
        std::cout << "Writing " << c.GetBlockCount() << " blocks to " << output << std::endl;

    boost::uint32_t prog = 0;
    
    for ( boost::uint32_t i = 0; i < c.GetBlockCount(); ++i )
    {
        std::ostringstream name;
        name << out << "-" << i;
        if (bCompressed)
            name << ".laz";
        else
            name << ".las";
        SummaryPtr summary(new::liblas::CoordinateSummary);        

        {
            std::ostream* ofs;
            
            const liblas::chipper::Block& b = c.GetBlock(i);
            header.SetExtent(b.GetBounds());
        
            liblas::Writer* writer = start_writer(ofs, name.str(), header);

            std::vector<boost::uint32_t> ids = b.GetIDs();
        
        
            for ( boost::uint32_t pi = 0; pi < ids.size(); ++pi )
            {
            
                bool read = reader.ReadPointAt(ids[pi]);
                if (read) {
                    liblas::Point const& p = reader.GetPoint();
                    summary->AddPoint(p);
                    writer->WritePoint(p);
                }
            }
            
            if (writer != 0)
                delete writer;
            if (ofs != 0)
            {
                liblas::Cleanup(ofs);
            }
            
        }
        
        liblas::Header hnew = FetchHeader(name.str());
        RepairHeader(*summary, hnew);
        RewriteHeader(hnew, name.str());

        if (verbose)
            term_progress(std::cout, (prog + 1) / static_cast<double>(c.GetBlockCount()));
        prog++;
    }

}
void write_index(std::string& output, 
                 liblas::chipper::Chipper& c, 
                 liblas::Reader& /*reader*/, 
                 long precision, 
                 bool verbose,
                 bool bUseStdout)
{
    std::ostream* out;
    if (bUseStdout) 
    {
        out = &std::cout;
    } else
    {
        out = static_cast<std::ostream*>(new std::ofstream(output.c_str()));
    }
        
    boost::uint32_t num_blocks = c.GetBlockCount();
    
    if (verbose)
        std::cout << "Writing " << c.GetBlockCount() << " blocks to " << output << std::endl;

    boost::uint32_t prog = 0;
    
    for ( boost::uint32_t i = 0; i < c.GetBlockCount(); ++i )
    {
        const liblas::chipper::Block& b = c.GetBlock(i);

        std::vector<boost::uint32_t> ids = b.GetIDs();
        *out << i << " " << ids.size() << " ";
        
        out->setf(std::ios::fixed,std::ios::floatfield);
        out->precision(precision);
        liblas::Bounds<double> const& bnd = b.GetBounds();
        *out << (bnd.min)(0) << " " << (bnd.min)(1) << " " << (bnd.max)(0) << " " <<  (bnd.max)(1) << " " ;
        
        for ( boost::uint32_t pi = 0; pi < ids.size(); ++pi )
        {
            *out << ids[pi] << " ";
        }

        *out << std::endl;

        // Set back to writing decimals
        out->setf(std::ios::dec);

        if (verbose)
            term_progress(std::cout, (prog + 1) / static_cast<double>(num_blocks));
        prog++;
    }
    if (out != 0 && !bUseStdout)
        delete out;
}
int main(int argc, char* argv[])
{
    std::string input;
    std::string output;
    
    long capacity = 3000;
    long precision = 8;
    bool verbose = false;
    bool tiling = false;
    bool bCompressed = false;
    bool bUseStdout = false;

    try
    {
        po::options_description desc("Allowed lasblock options");
        po::positional_options_description p;
        p.add("input", 1);
        p.add("output", 1);

        desc.add_options()
            ("help,h", "Produce this help message")
            ("capacity,c", po::value<long>(&capacity)->default_value(3000), "Number of points to nominally put into each block (note that this number will not be exact)")
            ("precision,p", po::value<long>(&precision)->default_value(8), "Number of decimal points to write for each bbox")
            ("stdout", po::value<bool>(&bUseStdout)->zero_tokens()->implicit_value(true), "Output data to stdout")
            ("write-points", po::value<bool>(&tiling)->zero_tokens()->implicit_value(true), "Write .las files for each block instead of an index file")
            ("compressed", po::value<bool>(&bCompressed)->zero_tokens()->implicit_value(true), "Produce .laz compressed data for --write-points tiles")

            ("input,i", po::value< std::string >(), "input LAS file")
            ("output,o", po::value< std::string >(&output)->default_value(""), "The output .kdx file (defaults to input filename + .kdx)")
            ("verbose,v", po::value<bool>(&verbose)->zero_tokens(), "Verbose message output")

        ;
    
        po::variables_map vm;        
        po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);

        po::notify(vm);

        if (vm.count("help")) 
        {
            std::cout << desc << "\n";
            std::cout <<"\nFor more information, see the full documentation for lasblock at:\n";
    
            std::cout << " http://liblas.org/utilities/block.html\n";
            std::cout << "----------------------------------------------------------\n";
            return 1;
        }
        
        if (vm.count("input")) 
        {
            input = vm["input"].as< std::string >();
            std::ifstream ifs;

            if (!liblas::Open(ifs, input.c_str()))
            {
                std::cerr << "Cannot open file '" << input << "'for read.  Exiting...";
                return 1;
            }
        } 
             else {
                std::cerr << "Input LAS file not specified!\n";
                OutputHelp(std::cout, desc);
                return 1;
            }    


        if (output.empty())
        {
            output = std::string(input) + ".kdx";
        }

        std::istream* ifs = liblas::Open(input, std::ios::in | std::ios::binary);
        if (!ifs)
        {
            std::cerr << "Cannot open " << input << " for read.  Exiting..." << std::endl;
            return 1;
        }

            
        
        {
            liblas::ReaderFactory f;
            liblas::Reader reader = f.CreateWithStream(*ifs);
    
            liblas::chipper::Chipper c(&reader, capacity);

            if (verbose)
                std::cout << "Chipping " << input<< " to " << output <<std::endl;

            c.Chip();
        

            if (!tiling)
            {
                write_index(output, c, reader, precision, verbose, bUseStdout);
            } else 
            {
                write_tiles(output, c, reader, verbose, bCompressed);
            }
            
        }
        
        if (ifs != 0)
        {
            liblas::Cleanup(ifs);
            ifs = 0;
        }
    }

    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!\n";
    }
    
    return 0;
}
