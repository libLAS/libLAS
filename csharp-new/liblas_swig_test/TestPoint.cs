using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;
using System.Diagnostics;

namespace swig_test
{
   static internal class TestPoint
   {
      static public void Test_A2(Point point)
      {
         double x = point.GetX();
         double y = point.GetY();
         double z = point.GetZ();
         Debug.Assert(Program.IsApprox(x, 636784.74, 0.01));
         Debug.Assert(Program.IsApprox(y, 849106.66, 0.01));
         Debug.Assert(Program.IsApprox(z, 426.71, 0.01));

         double time = point.GetTime();
         Debug.Assert(Program.IsApprox(time, 245382.13595, 0.00001));

         Debug.Assert(point.GetIntensity() == 118);
         Debug.Assert(point.GetReturnNumber() == 1);
         Debug.Assert(point.GetNumberOfReturns() == 1);

         Classification classif = point.GetClassification();
         Debug.Assert(classif.GetClassName() == "Unclassified");
         Debug.Assert(!classif.IsKeyPoint());
         Debug.Assert(!classif.IsSynthetic());
         Debug.Assert(!classif.IsWithheld());

         Color color = point.GetColor();
         Debug.Assert(color.GetRed() == 112);
         Debug.Assert(color.GetGreen() == 97);
         Debug.Assert(color.GetBlue() == 114);
      }

      static public void Test_B2(Point point)
      {
         Test_A2(point);
      }
   }
}
