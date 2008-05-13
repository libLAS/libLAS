// $Id$
//
// las2ogr translates LAS file to OGR datasource.
// Inspired by pylas.py script developed by Matthew Perry and Carl Anderson,
// available at http://code.google.com/p/pylas/
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#if defined(_MSC_VER) && defined(USE_VLD)
#include <vld.h>
#endif
// liblas
#include <liblas/liblas.hpp>
#include <liblas/laspoint.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/cstdint.hpp>
// ogr
#include <ogr_api.h>
// #include <ogrsf_frmts.h>
// #include <ogr_p.h>
//std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

template <typename T>
class raii_wrapper
{
    typedef void(*deleter_type)(T* p);

public:

    explicit raii_wrapper(T* p)
        : p_(p), del_(0)
    {}

    raii_wrapper(T* p, deleter_type d)
        : p_(p), del_(d)
    {}

    ~raii_wrapper()
    {
        do_delete(p_);
    }

    void reset(T* p)
    {
        do_delete(p_);
        p_= p;
    }

    T* get() const
    {
        return p_;
    }

    T* operator->() const
    {
        return p_;
    }

    T& operator*() const
    {
        return (*p_);
    }

    void swap(raii_wrapper& other)
    {
        std::swap(p_, other.p_);
    }

private:

    raii_wrapper(raii_wrapper const& other);
    raii_wrapper& operator=(raii_wrapper const& rhs);

    void do_delete(T* p)
    {
        if (del_)
        {
            del_(p);
        }
        else
        {
            delete p;
        }
    }

    deleter_type del_;
    T* p_;
};

OGRFieldDefnH create_field(const char* name, OGRFieldType type, int width, int precision ) {

    OGRFieldDefnH fld;
    fld= OGR_Fld_Create(name, type);
    OGR_Fld_SetWidth(fld, width);
    OGR_Fld_SetPrecision(fld, precision);

    return fld;
    
}
void create_layer_def(OGRLayerH lyr)
{
    OGRErr err;
    std::string fldname("return_num");
    
    OGRFieldDefnH fld;
    
    fld = create_field("return_num", OFTInteger, 10, 0);
    err = OGR_L_CreateField(lyr, fld, 0);
    if (OGRERR_NONE != err)
    {
        throw std::runtime_error("return_num field creation failed");
    }
    
    OGR_Fld_Destroy(fld);

    fld = create_field("angle", OFTInteger, 10, 0);
    err = OGR_L_CreateField(lyr, fld, 0);
    if (OGRERR_NONE != err)
    {
        throw std::runtime_error("angle field cration failed");
    }
    
    OGR_Fld_Destroy(fld);
    
    fld = create_field("intensity", OFTInteger, 10, 0);
    err = OGR_L_CreateField(lyr, fld, 0);
    if (OGRERR_NONE != err)
    {
        throw std::runtime_error("intensity field cration failed");
    }
    
    OGR_Fld_Destroy(fld);

    fld = create_field("asprsclass", OFTInteger, 10, 0);
    err = OGR_L_CreateField(lyr, fld, 0);
    if (OGRERR_NONE != err)
    {
        throw std::runtime_error("asprsclass field creation failed");
    }
    
    OGR_Fld_Destroy(fld);

    fld = create_field("return_tot", OFTInteger, 10, 0);
    err = OGR_L_CreateField(lyr, fld, 0);
    if (OGRERR_NONE != err)
    {
        throw std::runtime_error("return_tot field creation failed");
    }
    
    OGR_Fld_Destroy(fld);

    fld = create_field("gpstime", OFTReal, 10, 0);
    err = OGR_L_CreateField(lyr, fld, 0);
    if (OGRERR_NONE != err)
    {
        throw std::runtime_error("gpstime field cration failed");
    }
    
    OGR_Fld_Destroy(fld);

}

void report_ogr_formats()
{

    std::cout << "Supported target formats:";

    for (int i = 0; i < OGRGetDriverCount(); ++i)
    {
        OGRSFDriverH drv = OGRGetDriver(i);
        assert(0 != drv);

        if (OGR_Dr_TestCapability(drv,"ODrCCreateDataSource"))
        {
            std::cout << "\n - " << OGR_Dr_GetName(drv);
        }
    }

    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    // TODO:
    // - rename las2shp to las2ogr with ESRI Shapefile as default output format
    // - input params parsing
    // - usage info
    // - progress info

    OGRRegisterAll();

    try
    {
        // TODO: input params should come from argv
        std::string lasname("d:\\data\\lidar\\LDR030828_213450_0.LAS");
        //std::string lasname("D:\\data\\lidar\\gilmer\\000001.las");
        std::string ogrname("cloud.shp");

        //
        // Source
        //
        std::cout << "Source:" << "\n - dataset: " << lasname << std::endl;

        std::ifstream ifs;
        if (!liblas::Open(ifs, lasname.c_str()))
        {
            throw std::runtime_error(std::string("Can not open ") + lasname);
        }
        liblas::LASReader reader(ifs);

        //
        // Target
        //
        std::string const drvname("ESRI Shapefile");
        std::string const lyrname(ogrname.substr(0, ogrname.find_last_of('.')));

        std::cout << "Target:" 
                  << "\n - format: " << drvname
                  << "\n - dataset: " << ogrname
                  << "\n - layer: " << lyrname
                  << std::endl;
        
        OGRSFDriver* drv = 0;
        drv = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(drvname.c_str());
        if (0 == drv)
        {
            throw std::runtime_error(drvname + " driver not available");
        }

        raii_wrapper<OGRDataSource> ds(drv->CreateDataSource(ogrname.c_str()),
                                       OGRDataSource::DestroyDataSource);
        if (0 == ds.get())
        {
            throw std::runtime_error(ogrname + " datasource  cration failed");
        }

        OGRLayerH lyr = 0;
        lyr = OGR_DS_CreateLayer(ds, lyrname.c_str(), 0, wkbPoint25D, 0);
        if (0 == lyr)
        {
            throw std::runtime_error(ogrname + " layer  cration failed");
        }
       
        // Prepare layer schema
        create_layer_def(*lyr);

        //
        // Translation of points cloud to features set
        //
        std::cout << "Translating " << reader.GetHeader().GetPointRecordsCount() << " points...";

        raii_wrapper<OGRFeature> feat(OGRFeature::CreateFeature(lyr->GetLayerDefn()),
                                      OGRFeature::DestroyFeature);

        while (reader.ReadNextPoint())
        {
            liblas::LASPoint const& p = reader.GetPoint();   

            feat->SetField(0, p.GetReturnNumber());
            feat->SetField(1, p.GetScanAngleRank());
            feat->SetField(2, p.GetIntensity());
            feat->SetField(3, p.GetClassification());
            feat->SetField(4, p.GetNumberOfReturns());
            feat->SetField(5, p.GetTime());

            OGRPoint gp(p.GetX(), p.GetY(), p.GetZ());
            feat->SetGeometry(&gp); 

            if (OGRERR_NONE != lyr->CreateFeature(feat.get()))
            {
                throw std::runtime_error("feature creation failed");
            }
        }

        std::cout << "done!\n";
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error\n";
    }

    return 0;
}

