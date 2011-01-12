using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;
using System.Diagnostics;

namespace swig_test
{
   static internal class TestSpatialReference
   {
      static public void Test(SpatialReference srs)
      {
         string wkt = srs.GetWKT(SpatialReference.WKTModeFlag.eCompoundOK, false);

         // BUG: need to have one to test
         Debug.Assert(wkt == "");

         return;
      }
   }
}
