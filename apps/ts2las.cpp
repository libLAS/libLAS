
#include "ts2las.hpp"

#include <fstream>
#include <iostream>
#include <string>

using namespace liblas;


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
    }
    return istrm;
}

std::ostream* OpenOutput(std::string filename)
{
    std::ostream* ostrm;
    std::ios::openmode m;
    m = std::ios::out | std::ios::binary | std::ios::ate;
            
    if (compare_no_case(filename.c_str(),"STOUT",5) == 0)
    {
        ostrm = &std::cout;
    }
    else 
    {
        ostrm = new std::ofstream(filename.c_str(), m);
    }

    
    if (!ostrm->good())
    {
        delete ostrm;
        throw std::runtime_error("Writing stream was not able to be created");
    }
    
    return ostrm;
}

LASHeader CreateHeader(ScanHdr* hdr)
{
    LASHeader header;
    
    // Checks for time and color values
    liblas::LASHeader::PointFormat format = liblas::LASHeader::ePointFormat0;
    
    if (hdr->Time) {
        if (hdr->Color) {
            format = liblas::LASHeader::ePointFormat3;
        } else {
            format = liblas::LASHeader::ePointFormat1;
        }
    } else if (hdr->Color) {
        format = liblas::LASHeader::ePointFormat2;
    } 
    header.SetVersionMinor(2);
    header.SetDataFormatId(format);
    header.SetPointRecordsCount(hdr->PntCnt);
    header.SetOffset(hdr->OrgX, hdr->OrgY, hdr->OrgZ);
    std::cout << "offset x: " << header.GetOffsetX() << " offset y: " << header.GetOffsetY()  << " offset z: " <<header.GetOffsetZ() << std::endl;
    std::cout << "units: " << hdr->Units << std::endl;
    std::cout << "format: " << format << std::endl;
    return header;
}
bool ReadHeader(ScanHdr* hdr, std::istream* istrm) {
    
    try {
        liblas::detail::read_n(*hdr, *istrm, sizeof(ScanHdr));
        
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
    catch (std::exception const& e)
    {
        return false;
    }    

    return true;
}

bool WritePoints(LASWriter* writer, std::istream* strm, ScanHdr* hdr) 
{
    while (true) {
        
        if (hdr->HdrVersion == 20020715) {
///std::cout << "We have header version" << std::endl;
            ScanPnt* point = new ScanPnt;
            try
            {
                std::cout << "stream position is: " << strm->tellg() << std::endl;

                detail::read_n(*point, *strm, sizeof(ScanPnt));
                LASPoint p;
                p.SetCoordinates(writer->GetHeader(),point->Pnt.x,point->Pnt.y,point->Pnt.z);
                std::cout << "x: " << point->Pnt.x << " y: "<< point->Pnt.y << " z: " <<point->Pnt.z<< std::endl;
                std::cout << "x: " << p.GetX() << " y: "<< p.GetY() << " z: " <<p.GetZ()<< std::endl;

                std::cout << "Code: " << point->Code << " Intensity: "<< point->Intensity << std::endl;
                p.SetClassification(point->Code);
                p.SetIntensity(point->Intensity);
                writer->WritePoint(p);
                std::cout << "stream position is: " << strm->tellg() << std::endl;

                exit(-1);
               // std::cout << "Wrote point"<< std::endl;
                
            }
            catch (std::out_of_range const& e) // we reached the end of the file
            {
                std::cout << "catching out of range error!" ;
                break;
            }
            catch (std::exception const& e) // we reached the end of the file
            {
                std::cout << e.what() << std::endl;
                break;
            }   
        }
        
        // Reader::FillPoint(record, point);
        // point.SetCoordinates(header, point.GetX(), point.GetY(), point.GetZ());
        //     
        // if (header.GetDataFormatId() == LASHeader::ePointFormat1) {
        //     detail::read_n(t, m_ifs, sizeof(double));
        //     point.SetTime(t);
        // }
    }
    return true;    

}

void usage() {}

int main(int argc, char* argv[])
{
    int rc = 0;

    std::string input;
    std::string output;
    
    for (int i = 1; i < argc; i++) {
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
    
    std::cout << "input: " << input<<  " output: " <<output<<std::endl;
    
    ScanHdr* hdr = new ScanHdr;
    std::istream* istrm = OpenInput(input);
    std::ostream* ostrm = OpenOutput(output);
    
    bool success;
    success = ReadHeader(hdr, istrm);
    if (!success) 
    {
        std::cout<<"header was not read! exiting" << std::cout; exit(1);
    }
    
    std::cout << "stream position is: " << istrm->tellg() << std::endl;
    LASHeader header = CreateHeader(hdr);
    LASWriter* writer = new LASWriter(*ostrm, header);
    
     success = WritePoints(writer, istrm, hdr);
    
    std::cout << "Point Count: " << hdr->PntCnt <<std::endl;
    std::cout << "success: " << success << std::endl;
    return rc;
}