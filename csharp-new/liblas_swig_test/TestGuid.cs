using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;
using System.Diagnostics;

namespace swig_test
{
   static internal class TestGuid
   {
      static private void Test(guid g)
      {
         string s = g.to_string();
         Debug.Assert(s == "00000000-0000-0000-0000-000000000000");
      }

      static public void Test_A(guid g)
      {
         Test(g);
      }
      
      static public void Test_B(guid g)
      {
         Test(g);
      }

      static public void Test_T(guid guid)
      {
         string s = guid.to_string();
         Debug.Assert(s == "D59B08E7-79EE-47E4-AAE1-2B8DE4B87331".ToLower());
      }
   }
}
