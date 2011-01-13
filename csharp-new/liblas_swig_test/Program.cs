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
using System.IO;

namespace swig_test
{
   public class Program
   {
      static string fileA = @"..\..\test\data\1.2-with-color.las";
      static string fileB = @"..\..\test\data\1.2-with-color.laz";
      static string fileT = @"tmp.laz";
      
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
         // reader tests for A
         Test_ReadA();

         // reader tests for B
         Test_ReadB();

         // writer tests
         Test_WriteT();

         // reader tests for the generated B
         Test_ReadT();

         File.Delete(fileT);

         return 0;
      }

      private static void Test_ReadA()
      {
         ReaderFactory factory = new ReaderFactory();

         SWIGTYPE_p_std__istream ifs = Liblas.ReaderFactory.FileOpen(fileA);

         // the "using" here is required, because it forces the Reader
         // to be disposed before the stream explicitly goes away
         using (Reader reader = factory.CreateWithStream(ifs))
         {
            TestReader.Test_A(reader);

            Header header = reader.GetHeader();
            TestHeader.Test_A(header);

            TestGuid.Test_A(header.GetProjectId());

            VectorVariableRecord vlrs = header.GetVLRs();
            TestVariableRecord.Test_A(vlrs);

            SpatialReference srs = header.GetSRS();
            TestSpatialReference.Test(srs);

            bool ok = reader.ReadPointAt(2);
            Debug.Assert(ok);
            Point pt = reader.GetPoint();
            TestPoint.Test_A2(pt);
         }

         Liblas.ReaderFactory.FileClose(ifs);
      }

      private static void Test_ReadB()
      {
         ReaderFactory factory = new ReaderFactory();

         SWIGTYPE_p_std__istream ifs = Liblas.ReaderFactory.FileOpen(fileB);

         using (Reader reader = factory.CreateWithStream(ifs))
         {
            TestReader.Test_B(reader);

            Header header = reader.GetHeader();
            TestHeader.Test_B(header);

            TestGuid.Test_B(header.GetProjectId());

            VectorVariableRecord vlrs = header.GetVLRs();
            TestVariableRecord.Test_B(vlrs);

            SpatialReference srs = header.GetSRS();
            TestSpatialReference.Test(srs);

            bool ok = reader.ReadPointAt(2);
            Debug.Assert(ok);
            Point pt = reader.GetPoint();
            TestPoint.Test_B2(pt);
         }

         Liblas.ReaderFactory.FileClose(ifs);
      }

      private static void Test_WriteT()
      {
         // read in all the points from A, and write out to a temp file that looks like B

         ReaderFactory factory = new ReaderFactory();

         SWIGTYPE_p_std__istream ifs = Liblas.ReaderFactory.FileOpen(fileB);
         SWIGTYPE_p_std__ostream ofs = Liblas.WriterFactory.FileCreate(fileT);

         using (Reader reader = factory.CreateWithStream(ifs))
         {
            Header rHeader = reader.GetHeader();

            {
               Header wHeader = new Header(rHeader);
               {
                  wHeader.SetSystemId("liblas test");
                  wHeader.SetSoftwareId("swig test");
                  wHeader.SetCompressed(true);
                  guid g = new guid("D59B08E7-79EE-47E4-AAE1-2B8DE4B87331");
                  wHeader.SetProjectId(g);
                  wHeader.SetCreationYear(2011);
                  wHeader.SetCreationDOY(12);
               }
               {
                  using (Writer writer = new Writer(ofs, wHeader))
                  {
                     while (reader.ReadNextPoint())
                     {
                        Point pt = reader.GetPoint();
                        bool ok = writer.WritePoint(pt);
                        Debug.Assert(ok);
                     }
                  }
               }
            }
         }
         Liblas.WriterFactory.FileClose(ofs);
         Liblas.ReaderFactory.FileClose(ifs);
      }

      private static void Test_ReadT()
      {
         SWIGTYPE_p_std__istream ifs = Liblas.ReaderFactory.FileOpen(fileT);

         ReaderFactory factory = new ReaderFactory();

         using (Reader reader = factory.CreateWithStream(ifs))
         {
            TestReader.Test_T(reader);

            Header header = reader.GetHeader();
            TestHeader.Test_T(header);

            TestGuid.Test_T(header.GetProjectId());

            VectorVariableRecord vlrs = header.GetVLRs();
            TestVariableRecord.Test_T(vlrs);

            SpatialReference srs = header.GetSRS();
            TestSpatialReference.Test(srs);

            bool ok = reader.ReadPointAt(2);
            Debug.Assert(ok);
            Point pt = reader.GetPoint();
            TestPoint.Test_B2(pt);
         }
         Liblas.ReaderFactory.FileClose(ifs);
      }

   }
}
