#include <vld.h>
#include <liblas/lasreader.hpp>
#include <liblas/laspoint.hpp>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <cassert>
using namespace std;

int main()
{
    try
    {
        char* const name = "d:\\data\\lidar\\LDR030828_213450_0.LAS"; 
        liblas::LASReader reader(name);
        
        cout << "File: " << name << '\n';
        cout << "Version: " << reader.GetVersion() << '\n';

        cout << "Signature: " << reader.GetHeader().GetFileSignature() << '\n';
        cout << "Points count: " << reader.GetHeader().GetPointRecordsCount() << '\n';

        std::size_t i = 0;
        while (reader.ReadPoint())
        {
            liblas::LASPoint const& p = reader.GetPoint();
            
            // TODO: do sth with point data

            ++i;
        }

        cout << "Read points: " << i << '\n';
        if (reader.GetHeader().GetPointRecordsCount() != i)
            throw std::runtime_error("read incorrect number of point records");
    }
    catch (std::exception const& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}