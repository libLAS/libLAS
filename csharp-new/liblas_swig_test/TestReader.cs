using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;
using System.Diagnostics;

namespace swig_test
{
   static internal class TestReader
   {
      static public void Test_A(Reader reader)
      {
         Header h = reader.GetHeader();
         Debug.Assert(!h.Compressed());
      }

      static public void Test_B(Reader reader)
      {
         Header h = reader.GetHeader();
         Debug.Assert(h.Compressed());
      }

      static public void Test_T(Reader reader)
      {
         Header h = reader.GetHeader();
         Debug.Assert(h.Compressed());
      }
   }
}
