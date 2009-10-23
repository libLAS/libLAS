#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>

#include <liblas/laswriter.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lascolor.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/lasheader.hpp>
#include <liblas/lasvariablerecord.hpp>
#include <liblas/index/index.hpp>


#include <cstring>
#include <iterator>
#include <iterator>
#include <fstream>
#include <vector>
#include <sstream>

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif

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
        exit(1);
    }
    return istrm;
}

void usage() {}

using namespace liblas;

int main(int argc, char* argv[])
{
    std::string input;
    
    long dimension = 3;
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
        else if (   std::strcmp(argv[i],"--dimension") == 0  ||
                    std::strcmp(argv[i],"-dim") == 0     ||
                    std::strcmp(argv[i],"-d") == 0       
                )
        {
            i++;
            dimension = atoi(argv[i]);
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

    // FIXME: Missing RAII and try-catch, no LAS throws exceptions.
    std::istream* istrm = OpenInput(input);
    LASReader* reader = new LASReader(*istrm);
    std::cout << "Indexing " << input<< " "<<std::endl;

    LASIndexDataStream* idxstrm = new LASIndexDataStream(reader, dimension);
    
    
    LASIndex* idx = new LASIndex(input);
    idx->SetType(LASIndex::eExternalIndex);
    idx->SetLeafCapacity(capacity);
    idx->SetFillFactor(0.8);
    idx->SetDimension(dimension);
    idx->Initialize(*idxstrm);

    delete idx;
    delete idxstrm;
    delete reader;
    delete istrm;
}
