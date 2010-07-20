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


#include <liblas/lasreader.hpp>
#include <liblas/lasbounds.hpp>
#include "chipper.hpp"


#include <fstream>
#include <vector>


#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif


void usage() {
    std::cout << "----------------------------------------------------------\n";
    std::cout << "    lasblock (version ) usage:\n";
    std::cout << "----------------------------------------------------------\n";
    
    std::cout << " lasblock filename.las -c 1000 \n";
    std::cout << "--capacity: (-c)  capacity of the blocks\n";
       
}

using namespace liblas;


int main(int argc, char* argv[])
{
    std::string input;
    std::string output;
    
    long capacity = 10000;

    
    for (int i = 1; i < argc; i++)
    {
        if (    std::strcmp(argv[i],"-h") == 0 ||
                std::strcmp(argv[i],"--help") == 0
            )
        {
            usage();
            exit(0);
        }
        else if (   std::strcmp(argv[i],"--input") == 0  ||
                    std::strcmp(argv[i],"-input") == 0   ||
                    std::strcmp(argv[i],"-i") == 0       ||
                    std::strcmp(argv[i],"-in") == 0
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
            output = std::string(argv[i]) + ".kdx";
        }
                
        else if (   std::strcmp(argv[i],"--capacity") == 0  ||
                    std::strcmp(argv[i],"-cap") == 0     ||
                    std::strcmp(argv[i],"-c") == 0       
                )
        {
            i++;
            capacity = atoi(argv[i]);
        }

        else if (input.empty())
        {
            input = std::string(argv[i]);
        }

        else if (output.empty())
        {
            output = std::string(input) + ".kdx";
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

    if (output.empty())
    {
        output = std::string(input) + ".kdx";
    }

    std::ifstream in(input.c_str());
    std::ofstream out(output.c_str());
    
    liblas::Reader reader( in );
    
    liblas::chipper::Chipper c(&reader, capacity);
    c.Chip();

    std::cout << "Blocking " << input<< " to " << output <<std::endl;

    liblas::uint32_t num_blocks = c.GetBlockCount();
    
    std::cout << "Block count: " << num_blocks << std::endl;
    for ( liblas::uint32_t i = 0; i < num_blocks; ++i )
    {
        const liblas::chipper::Block& b = c.GetBlock(i);

        std::vector<liblas::uint32_t> ids = b.GetIDs();
        out << i << " " << ids.size() << " ";
        
        out.setf(std::ios::dec, std::ios::floatfield);
        out.precision(8);
        
        out << b.GetXmin() << " " << b.GetYmin() << " " << b.GetXmax() << " " <<  b.GetYmax()<< " " ;
        
        for ( liblas::uint32_t pi = 0; pi < ids.size(); ++pi )
        {
            out << ids[pi] << " ";
        }

        liblas::Bounds bds(b.GetXmin(), b.GetYmin(), b.GetXmax(), b.GetYmax());

        out << std::endl;
    }

}
