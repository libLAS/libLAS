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
//std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

template <typename T>
class ogr_wrapper
{
    typedef void(*deleter_type)(T p);

public:

    ogr_wrapper(T p, deleter_type d)
        : p_(p), del_(d)
    {}

    ~ogr_wrapper()
    {
        do_delete(p_);
    }

    void reset(T p)
    {
        do_delete(p_);
        p_= p;
    }

    operator T()
    {
        return get();
    }

    T get() const
    {
        return p_;
    }

    void swap(ogr_wrapper& other)
    {
        std::swap(p_, other.p_);
    }

private:

    ogr_wrapper(ogr_wrapper const& other);
    ogr_wrapper& operator=(ogr_wrapper const& rhs);

    void do_delete(T p)
    {
        assert(del_);
        del_(p);
    }

    deleter_type del_;
    T p_;
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
    assert(0 != lyr);

    std::string fldname("return_num");

    OGRFieldDefnH fld = create_field("return_num", OFTInteger, 10, 0);
    OGRErr err = OGR_L_CreateField(lyr, fld, 0);
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
        //std::string lasname("d:\\data\\lidar\\LDR030828_213450_0.LAS");
        std::string lasname("D:\\data\\lidar\\gilmer\\000001.las");
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
        
        OGRSFDriverH drv = OGRGetDriverByName(drvname.c_str());
        if (0 == drv)
        {
            throw std::runtime_error(drvname + " driver not available");
        }
 
        ogr_wrapper<OGRDataSourceH> ds(OGR_Dr_CreateDataSource(drv, ogrname.c_str(), 0),
                                       OGR_DS_Destroy);
        if (0 == ds)
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
        create_layer_def(lyr);

        //
        // Translation of points cloud to features set
        //
        std::cout << "Translating " << reader.GetHeader().GetPointRecordsCount() << " points...";

        ogr_wrapper<OGRFeatureH> feat(OGR_F_Create(OGR_L_GetLayerDefn(lyr)),
                                      OGR_F_Destroy);

        while (reader.ReadNextPoint())
        {
            liblas::LASPoint const& p = reader.GetPoint();   

            OGR_F_SetFieldInteger(feat, 0, p.GetReturnNumber());
            OGR_F_SetFieldInteger(feat, 1, p.GetScanAngleRank());
            OGR_F_SetFieldInteger(feat, 2, p.GetIntensity());
            OGR_F_SetFieldInteger(feat, 3, p.GetClassification());
            OGR_F_SetFieldInteger(feat, 4, p.GetNumberOfReturns());
            OGR_F_SetFieldDouble(feat, 5, p.GetTime());

            ogr_wrapper<OGRGeometryH> geom(OGR_G_CreateGeometry(wkbPoint25D),
                                           OGR_G_DestroyGeometry);
            OGR_G_SetPoint(geom, 0, p.GetX(), p.GetY(), p.GetZ());
            if (OGRERR_NONE != OGR_F_SetGeometry(feat, geom))
            {
                throw std::runtime_error("geometry creation failed");
            }

            if (OGRERR_NONE != OGR_L_CreateFeature(lyr, feat))
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

