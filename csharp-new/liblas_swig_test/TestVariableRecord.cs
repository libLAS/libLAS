using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;
using System.Diagnostics;

namespace swig_test
{
   static internal class TestVariableRecord
   {
      static public void Test(VariableRecord vlr)
      {
         // BUG: padding bool doesn't work?
         Debug.Assert(vlr.GetUserId(false) == "laszip encoded");
         Debug.Assert(vlr.GetRecordId() == 22204);
         Debug.Assert(vlr.GetRecordLength() == 52);
         Debug.Assert(vlr.GetDescription(true) == "encoded for sequential access");

         // BUG: need to check the data contents
      }

      static public void Test_A(VectorVariableRecord vlrs)
      {
         Debug.Assert(vlrs.Count == 0);
      }

      static public void Test_B(VectorVariableRecord vlrs)
      {
         Debug.Assert(vlrs.Count == 1);

         VariableRecord vlr = vlrs[0];

         Test(vlr);
      }

      static public void Test_T(VectorVariableRecord vlrs)
      {
         Debug.Assert(vlrs.Count == 1);

         VariableRecord vlr = vlrs[0];
         Test(vlrs[0]);
      }
   }
}
