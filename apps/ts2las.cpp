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
#include "ts2las.hpp"
#include "laskernel.hpp"

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

#include <fstream>
#include <iostream>
#include <string>

using namespace liblas;

// std::istream* OpenInput(std::string filename) 
// {
//     std::ios::openmode const mode = std::ios::in | std::ios::binary;
//     std::istream* istrm;
//     if (compare_no_case(filename.c_str(),"STDIN",5) == 0)
//     {
//         istrm = &std::cin;
//     }
//     else 
//     {
//         istrm = new std::ifstream(filename.c_str(), mode);
//     }
//     
//     if (!istrm->good())
//     {
//         delete istrm;
//         throw std::runtime_error("Reading stream was not able to be created");
//         exit(1);
//     }
//     return istrm;
// }
// 
// std::ostream* OpenOutput(std::string filename)
// {
//     std::ostream* ostrm;
//     std::ios::openmode m;
//     m = std::ios::out | std::ios::binary | std::ios::ate;
//             
//     if (compare_no_case(filename.c_str(),"STOUT",5) == 0)
//     {
//         ostrm = &std::cout;
//     }
//     else 
//     {
//         ostrm = new std::ofstream(filename.c_str(), m);
//     }
// 
//     
//     if (!ostrm->good())
//     {
//         delete ostrm;
//         throw std::runtime_error("Writing stream was not able to be created");
//         exit(1);
//     }
//     
//     return ostrm;
// }

liblas::Header CreateHeader(ScanHdr* hdr, bool verbose)
{
    liblas::Header header;
    
    // Checks for time and color values
    liblas::PointFormatName format = liblas::ePointFormat0;
    
    if (hdr->Time) {
        if (hdr->Color) {
            format = liblas::ePointFormat3;
        } else {
            format = liblas::ePointFormat1;
        }
    } else if (hdr->Color) {
        format = liblas::ePointFormat2;
    } 
    header.SetVersionMinor(2);
    header.SetDataFormatId(format);

    double scale = 1.0/(double)hdr->Units;
    header.SetScale(scale, scale, scale);
    header.SetOffset(-hdr->OrgX*scale, -hdr->OrgY*scale, -hdr->OrgZ*scale);
    header.SetPointRecordsCount(hdr->PntCnt);
    
    if (verbose)
    {
        std::cout << "The file says there should be " << hdr->PntCnt << " points" << std::endl;
        std::cout << "units: " << hdr->Units << std::endl;
        std::cout << "format: " << format << std::endl;
        std::cout << "scale: " << scale << std::endl;
        std::cout << "x origin: " << hdr->OrgX << std::endl;
        std::cout << "y origin: " << hdr->OrgY << std::endl;
        std::cout << "z origin: " << hdr->OrgZ << std::endl;

        boost::uint32_t precision = GetStreamPrecision(scale);

        std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
        std::cout.precision(precision);
        std::cout << "offset x: " << header.GetOffsetX() << std::endl;
        std::cout << "offset y: " << header.GetOffsetY() << std::endl;
        std::cout << "offset z: " << header.GetOffsetZ() << std::endl;
        
    }



    return header;
}

bool ReadHeader(ScanHdr* hdr, std::istream& istrm)
{
    try
    {
        liblas::detail::read_n(*hdr, istrm, sizeof(ScanHdr));
        
        if (hdr->Tunniste != 970401) return false;
        if (memcmp(hdr->Magic,"CXYZ",4)) return false;
        
        int version = hdr->HdrVersion;
        if (version == 970404) return true;
        if (version == 20010129) return true;
        if (version == 20010712) return true;
        if (version == 20020715) return true;
        if (( version > 20020715) && (version < 20051231)) return true;
        return false;
    }
    catch (std::exception const&)
    {
        return false;
    }    

}

bool WritePoints(liblas::Writer& writer, std::istream& strm, ScanHdr* hdr, bool verbose) 
{
    ScanPnt* point = new ScanPnt;
    ScanRow* row = new ScanRow;
    boost::uint32_t i = 0;
    
    for (std::size_t t = 0; t < static_cast<std::size_t>(hdr->PntCnt); t++)
    {

        if (hdr->HdrVersion == 20020715) {
            try 
            {
                liblas::detail::read_n(*point, strm, sizeof(ScanPnt));
            }
            catch (std::out_of_range&) // we reached the end of the file
            {
                return true;
                // break;
            }            
        } else{
            try 
            {
                liblas::detail::read_n(*row, strm, sizeof(ScanRow));
            }
            catch (std::out_of_range&) // we reached the end of the file
            {
                return true;
                // break;
            }   
            
            point->Pnt.x = row->x;
            point->Pnt.y = row->y;
            point->Pnt.z = row->z;
            point->Code = row->Code;
            point->Line = row->Line;
            point->Intensity = row->EchoInt & 0x3FFF;
            point->Echo = (row->EchoInt >> 14);
        }
        Point p(&writer.GetHeader());
        
        p.SetRawX(point->Pnt.x);
        p.SetRawY(point->Pnt.y);
        p.SetRawZ(point->Pnt.z);

        // std::cout << "read x: " << point->Pnt.x << " y: "<< point->Pnt.y << " z: " <<point->Pnt.z<< std::endl;
        // std::cout << "wrote x: " << p.GetX() << " y: "<< p.GetY() << " z: " <<p.GetZ()<< std::endl;
        // std::cout << "Code: " << point->Code << " Intensity: "<< point->Intensity << std::endl;
        p.SetClassification(point->Code);
        p.SetIntensity(point->Intensity);
        if (hdr->Time) {
            boost::uint32_t t = 0xFFFFFFFF;
            liblas::detail::read_n(t, strm, sizeof(t));

            // Time stamps are assumed to be GPS week seconds. The 
            // storage format is a 32 bit unsigned integer where 
            // each integer step is 0.0002 seconds.

            p.SetTime(t*0.0002);
        }
        if (hdr->Color) {
            boost::uint8_t r, g, b, a = 0;
            liblas::Color color;
            liblas::detail::read_n(r, strm, sizeof(r));
            liblas::detail::read_n(b, strm, sizeof(b));
            liblas::detail::read_n(g, strm, sizeof(g));
            
            // TS .bin says to read 4 bytes here for some reason.  Maybe 
            // this is an alpha value or something
            liblas::detail::read_n(a, strm, sizeof(a));
            
            color.SetGreen(g);
            color.SetBlue(b);
            color.SetRed(r);
            p.SetColor(color);
        }
        
        // Set return number
        /* 
            TerraScan uses two bits for storing echo information. The possible values are: 
            0 Only echo 
            1 First of many echo 
            2 Intermediate echo 
            3 Last of many echo
        */
        if (point->Echo == 0) { 
            p.SetNumberOfReturns(1);
            p.SetReturnNumber(1);
        } else if (point->Echo == 1) {
            p.SetReturnNumber(1);
        } else if (point->Echo == 3) {
            p.SetReturnNumber(2);
            p.SetNumberOfReturns(2);
        } else {
            // I don't know what the hell to do here without cumulating
            // through all of the points.  Why wouldn't you store the return 
            // number?!
            p.SetReturnNumber(2);
            p.SetNumberOfReturns(3);
        }

        try {
            writer.WritePoint(p);
        } catch (std::exception) 
        {
            std::cerr << "Point writing failed!" << std::endl; 
        }
        i++;
        if (verbose)
            term_progress(std::cout, (i + 1) / static_cast<double>(hdr->PntCnt));
     
    }

    delete point;
    delete row;
    return true;    

}

void OutputHelp( std::ostream & oss, po::options_description const& options)
{
    oss << "--------------------------------------------------------------------\n";
    oss << "    ts2las (" << GetFullVersion() << ")\n";
    oss << "--------------------------------------------------------------------\n";

    oss << options;

    oss <<"\nFor more information, see the full documentation for ts2las at:\n";

    oss << " http://liblas.org/utilities/ts2las.html\n";
    oss << "----------------------------------------------------------\n";

}

int main(int argc, char* argv[])
{
    std::string input;
    std::string output;
    bool verbose = false;
    std::vector<liblas::FilterPtr> filters;
    
    po::options_description ts2las_options("ts2las options");
    po::options_description filtering_options = GetFilteringOptions();
    po::positional_options_description p;
    p.add("input", 1);
    p.add("output", 1);

    ts2las_options.add_options()
        ("help,h", "Produce this help message")
        ("input,i", po::value< std::string >(&input), "input TerraSolid .bin file")
        ("output,o", po::value< std::string >(&output)->default_value(""), "The output .las file (defaults to input filename + .las)")
        ("verbose,v", po::value<bool>(&verbose)->zero_tokens(), "Verbose message output")

    ;
    
    po::variables_map vm;        
    po::options_description options;
    options.add(ts2las_options).add(filtering_options);
    po::store(po::command_line_parser(argc, argv).
      options(options).positional(p).run(), vm);
;

    po::notify(vm);

    if (vm.count("help")) 
    {
        OutputHelp(std::cout, options);
        return 1;
    }
    
    if (input.empty())
    {
        std::cerr << "No input TerraSolid .bin file was specfied!" << std::endl;
        OutputHelp(std::cout, options);
        return 1;
    }
    
    
    filters = GetFilters(vm, verbose);

    if (verbose)
        std::cout << "input: " << input<<  " output: " <<output<<std::endl;
    
    ScanHdr* hdr = new ScanHdr;

    std::ifstream istrm;
    bool opened = liblas::Open(istrm, input);
    if (!opened)
    {
        std::cerr << "Could not open file '" << input << "' to read TerraSolid .bin data! " << std::endl;
        return 1;
    }
    
    std::ofstream ostrm;
    bool created = liblas::Create(ostrm, output);
    if (!created)
    {
        std::cerr << "Could not create file " << output << " to write LAS data!" << std::endl;;
        return 1;
    }
    
    bool success;
    success = ReadHeader(hdr, istrm);
    if (!success) 
    {
        std::cerr<<"Unable to read " << input << "to read file!" << std::endl; 
        return 1;
    }
    
    // std::cout << "stream position is: " << istrm->tellg() << std::endl;
    liblas::Header header = CreateHeader(hdr, verbose);
    liblas::Writer writer(ostrm, header);
    
    writer.SetFilters(filters);
    
    success = WritePoints(writer, istrm, hdr, verbose);

    if (verbose)
    {
        std::cout << "Successfully wrote " << header.GetPointRecordsCount() 
                  <<" points to " << output << std::endl;
    }
    return 0;
}

