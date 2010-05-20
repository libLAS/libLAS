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
#include <liblas/laspoint.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/laswriter.hpp>
#include <liblas/lasfilter.hpp>
#include <liblas/cstdint.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <string>

using namespace liblas;

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


void tokenize(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiters = ",")
{
    // stolen from http://www.linuxselfhelp.com/HOWTO/C++Programming-HOWTO-7.html
    
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}



void usage() {}

int main(int argc, char* argv[])
{
    int rc = 0;

    std::vector<std::string> tokens;
    std::vector<liblas::uint8_t> classes;
    std::vector<double> bounds;
    std::vector<liblas::FilterI*> filters;
    
    try
    {

        // Parse command-line options
        std::string in_file;
        std::string out_file;
        std::string keep_classes;
        {
            int on_arg = 1;
            while (on_arg < argc)
            {
                std::string arg(argv[on_arg]);
                if (arg == "-h")
                {
                    usage();
                    return 0;
                }
                else if (arg == "-i" && (on_arg + 1 < argc))
                {   
                    ++on_arg;
                    assert(on_arg < argc);
                    in_file = argv[on_arg];
                }
                else if (arg == "-c" && (on_arg + 1 < argc))
                {   
                    ++on_arg;
                    assert(on_arg < argc);
                    tokenize(std::string(argv[on_arg]), tokens);
                    std::vector<std::string>::const_iterator i;
                    for (i = tokens.begin(); i != tokens.end(); ++i) {
                        classes.push_back(atoi((*i).c_str()));
                        std::cout << ", "<<*i;
                    }
                    std::cout << std::endl;
                    tokens.clear();
                
                }
                else if (arg == "-b" && (on_arg + 1 < argc))
                {   
                    ++on_arg;
                    assert(on_arg < argc);
                    tokenize(std::string(argv[on_arg]), tokens);
                    std::vector<std::string>::const_iterator i;
                    for (i = tokens.begin(); i != tokens.end(); ++i) {
                        bounds.push_back(atof((*i).c_str()));
                        std::cout << ", "<<*i;
                    }
                    tokens.clear();
                    std::cout << std::endl;
                
                }
                else if (arg == "-o" && (on_arg + 1 < argc))
                {
                    ++on_arg;
                    assert(on_arg < argc);
                    out_file = argv[on_arg];
                }

                else
                {
                    throw std::runtime_error(std::string("unrecognized parameter: ") + arg);
                }
                ++on_arg;
            }

            if (in_file.empty() || out_file.empty())
            {
                throw std::runtime_error("missing input paremeters");
            }
        }

        //
        // Source
        //
        std::cout << "Source:" << "\n - dataset: " << in_file << std::endl;

        // Make the filter

    
        liblas::ClassificationFilter* class_filter = new ClassificationFilter(classes);
        class_filter->SetKeep(true);
        filters.push_back(class_filter);
        
        liblas::BoundsFilter* bounds_filter = 0;
        if (bounds.size() > 0) {
            if (bounds.size() == 4) {
                bounds_filter = new BoundsFilter(bounds[0], bounds[1], bounds[2], bounds[3]);
            } else if (bounds.size() == 6) {
                bounds_filter = new BoundsFilter(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
            
            } else {
              throw std::runtime_error("bounds must be a 4-tuple or 6-tuple in the form of minx, miny, maxx, maxy, [minx, maxz]");  
            }
            filters.push_back(bounds_filter);
        }
        std::ifstream ifs;
        if (!liblas::Open(ifs, in_file.c_str()))
        {
            throw std::runtime_error(std::string("Can not open \'") + in_file + "\'");
        }
        liblas::Reader reader(ifs);
        
        reader.SetFilters(&filters);
    
        std::ofstream ofs;
        if (!liblas::Create(ofs, out_file.c_str()))
        {
            throw std::runtime_error(std::string("Can not create \'") + in_file + "\'");
        }        
        liblas::Writer writer(ofs, reader.GetHeader());


        std::cout << "Target:" 
            << "\n - : " << out_file
            << std::endl;

        //
        // Translation of points cloud to features set
        //
        liblas::uint32_t i = 0;
        liblas::uint32_t const size = reader.GetHeader().GetPointRecordsCount();

      while (reader.ReadNextPoint())
        {
            liblas::Point const& p = reader.GetPoint(); 
            writer.WritePoint(p);  

            term_progress(std::cout, (i + 1) / static_cast<double>(size));
            i++;
        }

        std::cout << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        rc = -1;
    }
    catch (...)
    {
        std::cerr << "Unknown error\n";
        rc = -1;
    }
    return rc;
}

