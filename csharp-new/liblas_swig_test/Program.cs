/******************************************************************************
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  test app for swig/C# bindings for liblas
 * Author:   Michael P. Gerlek (mpg@flaxen.com)
 *
 ******************************************************************************
 * Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Liblas;
using System.Diagnostics;

namespace swig_test
{
   public class Program
   {
      static public bool IsApprox(double a, double b, double tol)
      {
         double min = Math.Min(a, b);
         double max = Math.Max(a, b);
         double delta = max - min;
         if (delta > tol)
            return false;
         return true;
      }

      static int Main(string[] args)
      {
         string fileA = @"..\..\test\data\1.2-with-color.las";
         string fileB = @"..\..\test\data\1.2-with-color.laz";
         ReaderFactory factory = new ReaderFactory();

         // tests for A
         {
            SWIGTYPE_p_std__istream ifs = Liblas.ReaderFactory.FileOpen(fileA);

            Reader reader = factory.CreateWithStream(ifs);
            TestReader.Test_A(reader);

            Header header = reader.GetHeader();
            TestHeader.Test_A(header);

            bool ok = reader.ReadPointAt(2);
            Debug.Assert(ok);
            Point pt = reader.GetPoint();
            TestPoint.Test_A2(pt);
         }

         // tests for B
         {
            SWIGTYPE_p_std__istream ifs = Liblas.ReaderFactory.FileOpen(fileB);

            Reader reader = factory.CreateWithStream(ifs);
            TestReader.Test_B(reader);

            Header header = reader.GetHeader();
            TestHeader.Test_B(header);

            bool ok = reader.ReadPointAt(2);
            Debug.Assert(ok);
            Point pt = reader.GetPoint();
            TestPoint.Test_B2(pt);
         }

         return 0;
      }
   }
}
