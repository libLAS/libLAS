/***************************************************************************
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose: LAS translation to ASCII text with optional configuration
 * Author:  Martin Isenburg isenburg@cs.unc.edu 
 * Author:  Howard Butler, hobu.inc at gmail.com
 ***************************************************************************
 * Copyright (c) 2010, Howard Butler, hobu.inc at gmail.com 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/

#include <liblas/liblas.hpp>
#include "laskernel.hpp"
#include <liblas/utility.hpp>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

using namespace liblas;
using namespace std;

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif


std::string GetLabels(std::string const& parse_string,
                    std::string const& delimiter)
{
    std::ostringstream output;
    
    boost::uint32_t i = 0;
    for (;;)
    {
        
        switch (parse_string[i])
        {
        /* // the x coordinate */      
        case 'x':
            output << "\"X\"";
            break;
        /* // the y coordinate */
        case 'y':
            output << "\"Y\"";
            break;
        /* // the z coordinate */ 
        case 'z':
            output << "\"Z\"";
            break;
        /* // the raw x coordinate */      
        case 'X':
            output << "\"Raw X\"";
            break;
        /* // the raw y coordinate */
        case 'Y':
            output << "\"Raw Y\"";
            break;
        /* // the raw z coordinate */ 
        case 'Z':
            output << "\"Raw Z\"";
            break;
        /* // the gps-time */
        case 't': 
            output << "\"Time\"";
            break;
        /* // the intensity */
        case 'i':
            output << "\"Intensity\"";
            break;
        /* the scan angle */
        case 'a':
            output << "\"Scan Angle\"";
            break;
        /* the number of the return */
        case 'r': 
            output << "\"Return Number\"";
            break;
        /* the classification */
        case 'c':
            output << "\"Classification\"";
            break;
        /* the classification name */
        case 'C':
            output << "\"Classification Name\"";
            break;
        /* the user data */
        case 'u': 
            output << "\"User Data\"";
            break;
        /* the number of returns of given pulse */
        case 'n':
            output << "\"Number of Returns\"";
            break;
        /* the red channel color */
        case 'R':
            output << "\"Red\"";
            break;            
        /* the green channel color */
        case 'G': 
            output << "\"Green\"";
            break;            
        /* the blue channel color */
        case 'B': 
            output << "\"Blue\"";
            break;            
        case 'M':
            output << "\"ID\"";
            break;
        case 'p':
            output << "\"Point Source ID\"";
            break;
        /* the edge of flight line flag */
        case 'e': 
            output << "\"Flight Line Edge\"";
            break;
        /* the direction of scan flag */
        case 'd': 
            output << "\"Scan Direction\"";
            break;
        }

        i++;

        if (parse_string[i])
        {
            output << delimiter;
        }
        else
        {
            output << std::endl;
            break;
        }

    }
    return output.str();
}

std::string GetPointString( std::string const& parse_string,
                            std::string const& delimiter,
                            liblas::Point const& p, 
                            boost::array<boost::uint32_t, 4> precisions, 
                            boost::uint32_t index)
{
    
    std::ostringstream output;

    boost::uint32_t i = 0;
    liblas::Color const& c = p.GetColor();
    for (;;)
    {
        
        switch (parse_string[i])
        {
        /* // the x coordinate */      
        case 'x':
            output.setf(std::ios_base::fixed, std::ios_base::floatfield);
            output.precision(precisions[0]); //x precision
            output << p.GetX();
            output.unsetf(std::ios_base::fixed);
            output.unsetf(std::ios_base::floatfield);
            break;
        /* // the y coordinate */
        case 'y':
            output.setf(std::ios_base::fixed, std::ios_base::floatfield);
            output.precision(precisions[1]); //y precision
            output << p.GetY();
            output.unsetf(std::ios_base::fixed);
            output.unsetf(std::ios_base::floatfield);
            break;
        /* // the z coordinate */ 
        case 'z':
            output.setf(std::ios_base::fixed, std::ios_base::floatfield);
            output.precision(precisions[2]); //z precision
            output << p.GetZ();
            output.unsetf(std::ios_base::fixed);
            output.unsetf(std::ios_base::floatfield);
            break;
        /* // the raw x coordinate */      
        case 'X':
            output << p.GetRawX();
            break;
        /* // the raw y coordinate */
        case 'Y':
            output << p.GetRawY();
            break;
        /* // the raw z coordinate */ 
        case 'Z':
            output << p.GetRawZ();
            break;
        /* // the gps-time */
        case 't': 
            output.setf(std::ios_base::fixed, std::ios_base::floatfield);
            output.precision(precisions[3]); //t precision
            output << p.GetTime();
            output.unsetf(std::ios_base::fixed);
            output.unsetf(std::ios_base::floatfield);
            break;
        /* // the intensity */
        case 'i':
            output << p.GetIntensity();
            break;
        /* the scan angle */
        case 'a':
            output << (int)p.GetScanAngleRank();
            break;
        /* the number of the return */
        case 'r': 
            output << p.GetReturnNumber();
            break;
        /* the classification */
        case 'c':
            output << static_cast<boost::uint32_t>(p.GetClassification().GetClass());
            break;
        /* the classification name */
        case 'C':
            output << p.GetClassification().GetClassName();
            break;
        /* the user data */
        case 'u': 
            // output.setf( std::ios_base::hex, std::ios_base::basefield );
            // output.setf( std::ios::showbase);
            output << (int)p.GetUserData();
            // output.unsetf(std::ios_base::hex);
            // output.unsetf(std::ios_base::basefield);
            // output.unsetf( std::ios::showbase);
            break;
        /* the number of returns of given pulse */
        case 'n':
            output << p.GetNumberOfReturns();
            break;
        /* the red channel color */
        case 'R':
            output << c.GetRed();
            break;            
        /* the green channel color */
        case 'G': 
            output << c.GetGreen();
            break;            
        /* the blue channel color */
        case 'B': 
            output << c.GetBlue();
            break;            
        case 'M':
            output << index;
            break;
        case 'p':
            output << p.GetPointSourceID();
            break;
        /* the edge of flight line flag */
        case 'e': 
            output << p.GetFlightLineEdge();
            break;
        /* the direction of scan flag */
        case 'd': 
            output << p.GetScanDirection();
            break;
        }

        i++;

        if (parse_string[i])
        {
            output << delimiter;
        }
        else
        {
            output << std::endl;
            break;
        }

    }

    
    return output.str();
}

std::string GetHeader(liblas::Reader& reader)
{
    boost::ignore_unused_variable_warning(reader);

    std::ostringstream oss;
    
    return oss.str();
}
void write_points(  liblas::Reader& reader,
                    std::ostream& oss,
                    std::string const& parse_string,
                    std::string const& delimiter,
                    std::vector<liblas::FilterPtr>& filters,
                    std::vector<liblas::TransformPtr>& transforms,
                    boost::array<boost::uint32_t, 4> precisions,
                    bool bPrintLabels,
                    bool bPrintHeader,
                    bool verbose)
{

    liblas::Summary summary;
    
    reader.SetFilters(filters);
    reader.SetTransforms(transforms);


    if (verbose)
    std::cout << "Writing points:" 
        << "\n - : "
        << std::endl;

    //
    // Translation of points cloud to features set
    //
    boost::uint32_t i = 0;
    boost::uint32_t const size = reader.GetHeader().GetPointRecordsCount();
    
    if (bPrintHeader)
    {
        oss << GetHeader(reader);
    }
    
    if (bPrintLabels) 
    {
        oss << GetLabels(parse_string, delimiter);
    }


    while (reader.ReadNextPoint())
    {
        liblas::Point const& p = reader.GetPoint();
        // summary.AddPoint(p); 
        std::string output = GetPointString(parse_string, delimiter, p, precisions, i);
        
        oss << output;
        if (verbose)
            term_progress(std::cout, (i + 1) / static_cast<double>(size));
        i++;

    }
    if (verbose)
        std::cout << std::endl;
    
    
}

void OutputHelp( std::ostream & oss, po::options_description const& options)
{
    oss << "--------------------------------------------------------------------\n";
    oss << "    las2txt (" << GetFullVersion() << ")\n";
    oss << "--------------------------------------------------------------------\n";

    oss << options;

    oss <<"\nFor more information, see the full documentation for lasinfo at:\n";
    
    oss << " http://liblas.org/utilities/las2txt.html\n";
    oss << "----------------------------------------------------------\n";

}


int main(int argc, char* argv[])
{

    std::string input;
    std::string output;
    std::string parse_string("xyz");
    std::string delimiter(",");
    std::ostream* os = NULL;
    
    bool verbose = false;
    bool output_xml = false;
    bool bPrintLabels = false;
    bool bPrintHeader = false;
    bool bUseStdout = false;
    
    std::vector<liblas::FilterPtr> filters;
    std::vector<liblas::TransformPtr> transforms;
    
    liblas::Header header;
    boost::array<boost::uint32_t, 4> precisions;
    precisions.assign(0);
    
const char* parse_description = "The '--parse txyz' flag specifies how to format each"
                                " each line of the ASCII file. For example, 'txyzia'" 
    " means that the first number of each line should be the" 
    " gpstime, the next three numbers should be the x, y, and"
    " z coordinate, the next number should be the intensity"
    " and the next number should be the scan angle.\n\n" 
    " The supported entries are:\n" 
    "   x - x coordinate as a double\n"
    "   y - y coordinate as a double\n"
    "   z - z coordinate as a double\n"
    "   X - x coordinate as unscaled integer\n"
    "   Y - y coordinate as unscaled integer\n"
    "   Z - z coordinate as unscaled integer\n"
    "   a - scan angle\n"
    "   i - intensity\n"
    "   n - number of returns for given pulse\n"
    "   r - number of this return\n"
    "   c - classification number\n"
    "   C - classification name\n"
    "   u - user data\n"
    "   p - point source ID\n"
    "   e - edge of flight line\n"
    "   d - direction of scan flag\n"
    "   R - red channel of RGB color\n"
    "   G - green channel of RGB color\n"
    "   B - blue channel of RGB color\n"
    "   M - vertex index number\n"
    ;


    try {

        po::options_description file_options("las2txt options");
        po::options_description filtering_options = GetFilteringOptions();
        po::options_description header_options = GetHeaderOptions();

        po::positional_options_description p;
        p.add("input", 1);
        p.add("output", 1);
        
        
        file_options.add_options()
            ("help,h", "produce help message")
            ("input,i", po::value< string >(), "input LAS file.")
            ("output,o", po::value< string >(), "output text file.  Use 'stdout' if you want it written to the standard output stream")
            ("parse", po::value< string >(&parse_string), parse_description)
            ("precision", po::value< std::vector<string> >()->multitoken(), "The number of decimal places to use for x,y,z,[t] output.  \n --precision 7 7 3\n --precision 3 3 4 6\nIf you don't specify any precision, las2txt uses the implicit values defined by the header's scale value (and a precision of 8 is used for any time values.)")
            ("delimiter", po::value< string >(&delimiter), "The character to use for delimiting fields in the output.\n --delimiter \",\"\n --delimiter \"\t\"\n --delimiter \" \"")
            ("labels", po::value<bool>(&bPrintLabels)->zero_tokens()->implicit_value(true), "Print row of header labels")
            ("header", po::value<bool>(&bPrintHeader)->zero_tokens()->implicit_value(true), "Print header information")

            ("verbose,v", po::value<bool>(&verbose)->zero_tokens(), "Verbose message output")
            ("xml", po::value<bool>(&output_xml)->zero_tokens()->implicit_value(true), "Output as XML -- no formatting given by --parse is respected in this case.")
            ("stdout", po::value<bool>(&bUseStdout)->zero_tokens()->implicit_value(true), "Output data to stdout")

        ;

        po::variables_map vm;
        po::options_description options;
        options.add(file_options).add(filtering_options);
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
                std::cerr << "Cannot open " << input << " for read.  Exiting..." << std::endl;
                return 1;
            }
            liblas::ReaderFactory f;
            liblas::Reader reader = f.CreateWithStream(ifs);
            header = reader.GetHeader();
        } else {
            std::cerr << "Input LAS file not specified!\n";
            OutputHelp(std::cout, options);
            return 1;
        }


        if (vm.count("output")) 
        {
            output = vm["output"].as< string >();
            
            std::ios::openmode const mode = std::ios::out | std::ios::binary;
            if (compare_no_case(output.c_str(),"STDOUT",5) == 0)
            {
                os = &std::cout;
                bUseStdout = true;
            }
            else 
            {
                os = new std::ofstream(output.c_str(), mode);
            }
    
            if (!os->good())
            {
                delete os;
                std::cerr << "Cannot open " << output << " to write.  Exiting..." << std::endl;
                return 1;
            }

            

        } else {
            
            if (bUseStdout) 
            {
                os = &std::cout;
            } else 
            {
                std::cerr << "Output text file not specified!\n";
                OutputHelp(std::cout, options);
                return 1;
            }

        }
        filters = GetFilters(vm, verbose);

        std::ifstream ifs;
        if (!liblas::Open(ifs, input.c_str()))
        {
            std::cerr << "Cannot open " << input << " for read.  Exiting..." << std::endl;
            return false;
        }

        if (vm.count("precision")) 
        {
            std::vector<std::string> precision_str = vm["precision"].as< std::vector<std::string> >();
            if (precision_str.size() > 4) {
                ostringstream oss;
                oss << "Too many arguments were given to precision. "
                    << "--precision x y z [t]  or -- precision header  ";
                throw std::runtime_error(oss.str());
            }
            if (precision_str.size() < 3) {
                ostringstream oss;
                oss << "At least three arguments must be given to precision. ";
                throw std::runtime_error(oss.str());
            }

            if (verbose)
            {
                ostringstream oss;
                for (std::vector<std::string>::const_iterator i = precision_str.begin();
                     i != precision_str.end();
                     i++) 
                    {
                        oss << *i << " ";
                    }
                    std::cout << "Setting precisions to: " << oss.str() << std::endl;
            }        

            precisions[0] = boost::lexical_cast<boost::uint32_t>(precision_str[0]);
            precisions[1] = boost::lexical_cast<boost::uint32_t>(precision_str[1]);
            precisions[2] = boost::lexical_cast<boost::uint32_t>(precision_str[2]);
            
            if (precision_str.size() == 4)
            {
                precisions[3] = boost::lexical_cast<boost::uint32_t>(precision_str[3]);
    
            } else {
                precisions[3] = 8;
            }
        } else {

            precisions[0] = GetStreamPrecision(header.GetScaleX());
            precisions[1] = GetStreamPrecision(header.GetScaleY());
            precisions[2] = GetStreamPrecision(header.GetScaleZ());
            precisions[3] = 8;

            ostringstream oss;
            for (boost::array<boost::uint32_t, 4>::const_iterator i = precisions.begin();
                 i != precisions.end();
                 i++) 
                {
                    oss << *i << " ";
                }            
            if (verbose)
            {
                std::cout << "Setting precisions from header to " << oss.str() << std::endl;
            }
        }    

        if (vm.count("delimiter")) 
        {
            std::string delim = vm["delimiter"].as< string >();
            std::string tab("\\t");
            std::string newline("\\n");
            if (!delim.compare(tab)) 
            { 
                delimiter = "\t"; 
            } else if (!delim.compare(newline))
            {
                delimiter = "\n";
            }
            if (verbose)
            {
                std::cout << "Using delimiter '" << delim << "'"<< std::endl;
            } 
            
        } 

        liblas::ReaderFactory f;
        liblas::Reader reader = f.CreateWithStream(ifs);

        write_points(  reader,
                    *os,
                    parse_string,
                    delimiter,
                    filters,
                    transforms,
                    precisions,
                    bPrintLabels,
                    bPrintHeader,
                    verbose);

    if (os != 0 && !bUseStdout)
        delete os;

    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }
    
    return 0;


}

//las2las2 -i lt_srs_rt.las  -o foo.las -c 1,2 -b 2483590,366208,2484000,366612
