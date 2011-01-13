using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;
using System.Diagnostics;

namespace swig_test
{
   static internal class TestHeader
   {
      static private void Test(Header header)
      {
         Debug.Assert(Program.IsApprox(header.GetMinX(), 635619.85, 0.01));
         Debug.Assert(Program.IsApprox(header.GetMinY(), 848899.70, 0.01));
         Debug.Assert(Program.IsApprox(header.GetMinZ(), 406.59, 0.01));
           
         Debug.Assert(Program.IsApprox(header.GetMaxX(), 638982.55, 0.01));
         Debug.Assert(Program.IsApprox(header.GetMaxY(), 853535.43, 0.01));
         Debug.Assert(Program.IsApprox(header.GetMaxZ(), 586.38, 0.01));

         Debug.Assert(Program.IsApprox(header.GetScaleX(), 0.01, 0.01));
         Debug.Assert(Program.IsApprox(header.GetScaleY(), 0.01, 0.01));
         Debug.Assert(Program.IsApprox(header.GetScaleZ(), 0.01, 0.01));

         Debug.Assert(Program.IsApprox(header.GetOffsetX(), 0.0, 0.001));
         Debug.Assert(Program.IsApprox(header.GetOffsetY(), 0.0, 0.001));
         Debug.Assert(Program.IsApprox(header.GetOffsetZ(), 0.0, 0.001));

         Debug.Assert(header.GetFileSignature() == "LASF");
         Debug.Assert(header.GetFileSourceId() == 0);
         Debug.Assert(header.GetVersionMajor() == 1);
         Debug.Assert(header.GetVersionMinor() == 2);
         Debug.Assert(header.GetDataFormatId() == PointFormatName.ePointFormat3);
         Debug.Assert(header.GetPointRecordsCount() == 1065);

         return;
      }

      static public void Test_A(Header header)
      {
         Test(header);
         Debug.Assert(header.GetCreationDOY() == 0);
         Debug.Assert(header.GetCreationYear() == 0);
         Debug.Assert(header.GetRecordsCount() == 0);
         Debug.Assert(!header.Compressed());
         Debug.Assert(header.GetSystemId(false) == "");
         Debug.Assert(header.GetSoftwareId(true) == "TerraScan");
      }

      static public void Test_B(Header header)
      {
         Test(header);
         Debug.Assert(header.GetCreationDOY() == 0);
         Debug.Assert(header.GetCreationYear() == 0);
         Debug.Assert(header.GetRecordsCount() == 1);
         Debug.Assert(header.Compressed());
         Debug.Assert(header.GetSystemId(false) == "");
         Debug.Assert(header.GetSoftwareId(true) == "TerraScan");
      }

      static public void Test_T(Header header)
      {
         Test(header);
         Debug.Assert(header.GetCreationDOY() == 12);
         Debug.Assert(header.GetCreationYear() == 2011);
         Debug.Assert(header.GetRecordsCount() == 1);
         Debug.Assert(header.Compressed());
         Debug.Assert(header.GetSystemId(false) == "liblas test");
         Debug.Assert(header.GetSoftwareId(true) == "swig test");
      }
   }
}
