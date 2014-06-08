// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/header.hpp>
#include <tut/tut.hpp>
#include "common.hpp"
// boost
#include <boost/cstdint.hpp>
// std
#include <string>

using namespace boost;

namespace tut {

void test_default_header(liblas::Header const& h)
{
    using liblas::Header;

    ensure_equals("wrong default file signature",
        h.GetFileSignature(), Header::FileSignature);

    ensure_equals("wrong default file source id",
        h.GetFileSourceId(), 0);
    ensure_equals("wrong default reserved value",
        h.GetReserved(), 0);

    boost::uuids::uuid g = boost::uuids::nil_uuid();
    ensure_equals("wrong default project guid",
        h.GetProjectId(), g);

    ensure_equals("wrong default major version",
        h.GetVersionMajor(), 1);
    ensure_equals("wrong default minor version",
        h.GetVersionMinor(), 2);

    ensure_equals("wrong default system id",
        h.GetSystemId(), Header::SystemIdentifier);
    ensure_equals("wrong default software id",
        h.GetSoftwareId(), Header::SoftwareIdentifier);

    // TODO: Fix me to use todays day # and year
    // ensure_equals("wrong default creation day-of-year",
    //     h.GetCreationDOY(), 0);
    // ensure_equals("wrong default creation year",
    //     h.GetCreationYear(), 0);
    ensure_equals("wrong default header size",
        h.GetHeaderSize(), boost::uint16_t(227));
    
    boost::uint32_t offset = 229;
    if (h.GetVersionMinor() == 1 || h.GetVersionMinor() == 2)
    {
        offset = 227;
    }
    ensure_equals("wrong default data offset",
        h.GetDataOffset(), offset);
    
    ensure_equals("wrong default records count",
        h.GetRecordsCount(), boost::uint32_t(0));
    ensure_equals("wrong default data format id",
        h.GetDataFormatId(), liblas::ePointFormat3);
    ensure_equals("wrong default data record length",
        h.GetDataRecordLength(), liblas::ePointSize3);
    ensure_equals("wrong default point records count",
        h.GetPointRecordsCount(), boost::uint32_t(0));

    ensure_equals("wrong default X scale", h.GetScaleX(), double(1.0));
    ensure_equals("wrong default Y scale", h.GetScaleY(), double(1.0));
    ensure_equals("wrong default Z scale", h.GetScaleZ(), double(1.0));

    ensure_equals("wrong default X offset", h.GetOffsetX(), double(0));
    ensure_equals("wrong default Y offset", h.GetOffsetY(), double(0));
    ensure_equals("wrong default Z offset", h.GetOffsetZ(), double(0));

    ensure_equals("wrong default min X", h.GetMinX(), double(0));
    ensure_equals("wrong default max X", h.GetMaxX(), double(0));
    ensure_equals("wrong default min Y", h.GetMinY(), double(0));
    ensure_equals("wrong default max Y", h.GetMaxY(), double(0));
    ensure_equals("wrong default min Z", h.GetMinZ(), double(0));
    ensure_equals("wrong default max Z", h.GetMaxZ(), double(0));
}

void test_default_point(liblas::Point const& p)
{
    ensure_equals("wrong default X coordinate",
        p.GetX(), double(0));
    ensure_equals("wrong default Y coordinate",
        p.GetY(), double(0));
    ensure_equals("wrong default Z coordinate",
        p.GetZ(), double(0));
    ensure_equals("wrong defualt intensity",
        p.GetIntensity(), 0);
    ensure_equals("wrong defualt return number",
        p.GetReturnNumber(), 0);
    ensure_equals("wrong defualt number of returns",
        p.GetNumberOfReturns(), 0);
    ensure_equals("wrong defualt scan direction",
        p.GetScanDirection(), 0);
    ensure_equals("wrong defualt edge of flight line",
        p.GetFlightLineEdge(), 0);
    ensure_equals("wrong defualt classification",
        p.GetClassification(), liblas::Classification::bitset_type());
    ensure_equals("wrong defualt scan angle rank",
        p.GetScanAngleRank(), 0);
    ensure_equals("wrong defualt file marker/user data value",
        p.GetUserData(), 0);
    ensure_equals("wrong defualt user bit field/point source id value",
        p.GetPointSourceID(), 0);
    ensure_equals("wrong defualt time",
        p.GetTime(), double(0));

    ensure_equals("invalid default red color",
        p.GetColor().GetRed(), 0);
    ensure_equals("invalid default green color",
        p.GetColor().GetGreen(), 0);
    ensure_equals("invalid default blue color",
        p.GetColor().GetBlue(), 0);
            
    ensure("invalid defualt point record", p.IsValid());
}

void test_file10_header(liblas::Header const& h)
{
    ensure_equals(h.GetFileSignature(), liblas::Header::FileSignature);
    ensure_equals(h.GetFileSourceId(), 0);
    ensure_equals(h.GetReserved(), 0);

    boost::uuids::uuid g = boost::uuids::nil_uuid();
    ensure_equals(g, boost::uuids::nil_uuid());
    ensure_equals("wrong ProjectId", h.GetProjectId(), g);

    ensure_equals("wrong VersionMajor", h.GetVersionMajor(), 1);
    ensure_equals("wrong VersionMinor", h.GetVersionMinor(), 0);
    ensure_equals("wrong GetSystemId", h.GetSystemId(), std::string(""));
    ensure_equals("wrong GetSoftwareId", h.GetSoftwareId(), std::string("TerraScan"));
    ensure_equals("Wrong GetCreationDOY", h.GetCreationDOY(), 0);
    ensure_equals("Wrong GetCreationYear", h.GetCreationYear(), 0);
    ensure_equals("Wrong GetHeaderSize", h.GetHeaderSize(), boost::uint16_t(227));
    ensure_equals("Wrong GetDataOffset", h.GetDataOffset(), boost::uint32_t(229));
    ensure_equals("Wrong GetRecordsCount", h.GetRecordsCount(), boost::uint32_t(0));
    ensure_equals("Wrong GetDataFormatId", h.GetDataFormatId(), liblas::ePointFormat1);
    ensure_equals("Wrong GetDataRecordLength", h.GetDataRecordLength(), liblas::ePointSize1);
    ensure_equals("Wrong GetPointRecordsCount", h.GetPointRecordsCount(), boost::uint32_t(8));
    ensure_equals("Wrong GetScaleX", h.GetScaleX(), double(0.01));
    ensure_equals("Wrong GetScaleY", h.GetScaleY(), double(0.01));
    ensure_equals("Wrong GetScaleZ", h.GetScaleZ(), double(0.01));
    ensure_equals("Wrong GetOffsetX", h.GetOffsetX(),double(-0));
    ensure_equals("Wrong GetOffsetY", h.GetOffsetY(), double(-0));
    ensure_equals("Wrong GetOffsetZ", h.GetOffsetZ(), double(-0));
    ensure_equals("Wrong GetMinX", h.GetMinX(), double(630262.3));
    ensure_equals("Wrong GetMaxX", h.GetMaxX(), double(630346.83));
    ensure_equals("Wrong GetMinY", h.GetMinY(), double(4834500));
    ensure_equals("Wrong GetMaxY", h.GetMaxY(), double(4834500));
    ensure_equals("Wrong GetMinZ", h.GetMinZ(), double(50.9));
    ensure_equals("Wrong GetMaxZ", h.GetMaxZ(), double(55.26));
}

void test_file10_point1(liblas::Point const& p)
{
    ensure_distance(p.GetX(), double(630262.30), 0.0001);
    ensure_distance(p.GetY(), double(4834500), 0.0001);
    ensure_distance(p.GetZ(), double(51.53), 0.0001);
    ensure_equals(p.GetIntensity(), 670);
    ensure_equals(p.GetClassification(), liblas::Classification::bitset_type(1));
    ensure_equals(p.GetScanAngleRank(), 0);
    ensure_equals(p.GetUserData(), 3);
    ensure_equals(p.GetPointSourceID(), 0);
    ensure_equals(p.GetScanFlags(), 9);
    ensure_distance(p.GetTime(), double(413665.23360000004), 0.0001);
}

void test_file10_point2(liblas::Point const& p)
{

    ensure_distance(p.GetX(), double(630282.45), 0.0001);
    ensure_distance(p.GetY(), double(4834500), 0.0001);
    ensure_distance(p.GetZ(), double(51.63), 0.0001);
    ensure_equals(p.GetIntensity(), 350);
    ensure_equals(p.GetClassification(), liblas::Classification::bitset_type(1));
    ensure_equals(p.GetScanAngleRank(), 0);
    ensure_equals(p.GetUserData(), 3);
    ensure_equals(p.GetPointSourceID(), 0);
    ensure_equals(p.GetScanFlags(), 9);
    ensure_distance(p.GetTime(), double(413665.52880000003), 0.0001);
}

void test_file10_point4(liblas::Point const& p)
{
    ensure_distance(p.GetX(), double(630346.83), 0.0001);
    ensure_distance(p.GetY(), double(4834500), 0.0001);
    ensure_distance(p.GetZ(), double(50.90), 0.0001);
    ensure_equals(p.GetIntensity(), 150);
    ensure_equals(p.GetClassification(), liblas::Classification::bitset_type(1));
    ensure_equals(p.GetScanAngleRank(), 0);
    ensure_equals(p.GetUserData(), 4);
    ensure_equals(p.GetPointSourceID(), 0);
    ensure_equals(p.GetScanFlags(), 18);
    ensure_distance(p.GetTime(), double(414093.84360000002), 0.0001);
}

void test_file_12Color_point0(liblas::Point const& p)
{
    ensure_distance(p.GetX(), double(637012.240000), 0.0001);
    ensure_distance(p.GetY(), double(849028.310000), 0.0001);
    ensure_distance(p.GetZ(), double(431.660000), 0.0001);
    ensure_distance(p.GetTime(), double(245380.782550), 0.0001);

    ensure_equals(p.GetReturnNumber(), 1);
    ensure_equals(p.GetNumberOfReturns(), 1);
    ensure_equals(p.GetFlightLineEdge(), 0);
    ensure_equals(p.GetIntensity(), 143);
    ensure_equals(p.GetScanDirection(), 1);
    ensure_equals(p.GetScanAngleRank(), -9);

    ensure_equals(p.GetClassification().GetClass(), 1);
    ensure_equals(p.GetClassification().IsWithheld(), false);
    ensure_equals(p.GetClassification().IsKeyPoint(), false);
    ensure_equals(p.GetClassification().IsSynthetic(), false);

    ensure_equals(p.GetColor().GetRed(), 68);
    ensure_equals(p.GetColor().GetGreen(), 77);
    ensure_equals(p.GetColor().GetBlue(), 88);
}

void test_file_12Color_point1(liblas::Point const& p)
{
    ensure_distance(p.GetX(), double(636896.330000), 0.0001);
    ensure_distance(p.GetY(), double(849087.700000), 0.0001);
    ensure_distance(p.GetZ(), double(446.390000), 0.0001);
    ensure_distance(p.GetTime(), double(245381.452799), 0.0001);

    ensure_equals(p.GetReturnNumber(), 1);
    ensure_equals(p.GetNumberOfReturns(), 2);
    ensure_equals(p.GetFlightLineEdge(), 0);
    ensure_equals(p.GetIntensity(), 18);
    ensure_equals(p.GetScanDirection(), 1);
    ensure_equals(p.GetScanAngleRank(), -11);

    ensure_equals(p.GetClassification().GetClass(), 1);
    ensure_equals(p.GetClassification().IsWithheld(), false);
    ensure_equals(p.GetClassification().IsKeyPoint(), false);
    ensure_equals(p.GetClassification().IsSynthetic(), false);

    ensure_equals(p.GetColor().GetRed(), 54);
    ensure_equals(p.GetColor().GetGreen(), 66);
    ensure_equals(p.GetColor().GetBlue(), 68);
}
    
void test_file_12Color_point2(liblas::Point const& p)
{
    ensure_distance(p.GetX(), double(636784.740000), 0.0001);
    ensure_distance(p.GetY(), double(849106.660000), 0.0001);
    ensure_distance(p.GetZ(), double(426.710000), 0.0001);
    ensure_distance(p.GetTime(), double(245382.135950), 0.0001);

    ensure_equals(p.GetReturnNumber(), 1);
    ensure_equals(p.GetNumberOfReturns(), 1);
    ensure_equals(p.GetFlightLineEdge(), 0);
    ensure_equals(p.GetIntensity(), 118);
    ensure_equals(p.GetScanDirection(), 0);
    ensure_equals(p.GetScanAngleRank(), -10);

    ensure_equals(p.GetClassification().GetClass(), 1);
    ensure_equals(p.GetClassification().IsWithheld(), false);
    ensure_equals(p.GetClassification().IsKeyPoint(), false);
    ensure_equals(p.GetClassification().IsSynthetic(), false);

    ensure_equals(p.GetColor().GetRed(), 112);
    ensure_equals(p.GetColor().GetGreen(), 97);
    ensure_equals(p.GetColor().GetBlue(), 114);
}

void test_laszip_vlr(liblas::Header const& header)
{
    bool found_vlr = false;

    std::vector<liblas::VariableRecord> const& vlrs = header.GetVLRs();
    std::vector<liblas::VariableRecord>::const_iterator it;
    for (it = vlrs.begin(); it != vlrs.end(); ++it)
    {
        liblas::VariableRecord const& vlr = *it;
        if (vlr.GetUserId(false).compare("laszip encoded") == 0)
        {
            ensure_equals(found_vlr, false); // make sure we only find one
            found_vlr = true;

            ensure_equals(vlr.GetRecordId(), 22204);
            ensure_equals(vlr.GetRecordLength(), 52);
        }
    }
    
    ensure_equals(found_vlr, true); // make sure we found exactly one

    return;
}
}
