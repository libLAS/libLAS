using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;

namespace tester
{
   class Program
   {
      static void Assert(bool b)
      {
         if (!b)
            Environment.Exit(1);
      }

      static int Main(string[] args)
      {
         SWIGTYPE_p_std__istream ifs = Liblas.ReaderFactory.FileOpen(@"..\..\test\data\TO_core_last_zoom.las");

         Reader r = new Reader(ifs);

         double maxx = r.GetHeader().GetMaxX();
         Assert(maxx == 630500.0);
         Console.WriteLine("maxx = {0}", maxx);

         return 0;
      }
   }
}
