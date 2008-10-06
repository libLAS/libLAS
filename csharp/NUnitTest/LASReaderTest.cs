using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using LibLAS;


namespace NUnitTest
{
    [TestFixture]
    public class LASReaderTest
    {
        private string folder = @"C:\las\data";

        private string TO_core_last_zoom;

        [SetUp]
        protected void SetUp()
        {

            TO_core_last_zoom = folder + "\\TO_core_last_zoom.las";

        }

        [Test]
        public void hhhhh()
        {

           // LASReader lasreader = new LASReader(@"c:\las\sample_our2.las");
                  LASReader lasreader = new LASReader(@"C:\las\data\TO_core_last_clip.las");

            LASPoint laspoint;

            //laspoint = lasreader.GetPointAt(0);

            //Console.WriteLine(laspoint.X + "," + laspoint.Y + "," + laspoint.Z + "  " + laspoint.Intensity );


            LASHeader lasheader = lasreader.GetHeader();

            LASWriter laswriter = new LASWriter(@"c:\las\sample_our.las", lasheader, LASReadWriteMode.LASModeWrite);

            //LASReader lasreader = new LASReader(@"C:\las\data\TO_core_last_clip.las");

            //LASPoint laspoint = new LASPoint();

           laspoint = lasreader.GetPointAt(0);
            //laspoint.X = 23.0;
            //Assert.AreEqual(laspoint.X, 23.0);
            //byte gg = laspoint.Classification;
            //bool d = lasreader.GetNextPoint();
            ////d = lasreader.GetNextPoint();
            //LASPoint point = lasreader.GetPointAt(1);
            //Assert.LessOrEqual(Math.Abs(point.X - 630262.30), 0.0001);
            //Assert.LessOrEqual(Math.Abs(point.Y - 4834500.0), 0.0001);
            //Assert.LessOrEqual(Math.Abs(point.Z - 51.53), 0.0001);
            Assert.AreEqual(laspoint.Intensity, 670);
            // Assert.AreEqual(laspoint.Classification,(byte) 1);

            //Assert.AreEqual(point.ScanAngleRank, 0);
            //Assert.AreEqual(point.UserData, 3);
            //Assert.AreEqual(point.ScanFlags, 9);
            //Assert.LessOrEqual(Math.Abs(point.Time - 413665.23360000004), 0.0001);


        }

        //[Test]
        //public void Point2()
        //{
        //    LASReader lasreader = new LASReader(@"C:\las\data\TO_core_last_clip.las");
        //   // LASHeader lasheader = lasreader.GetHeader();
        //    if (lasreader.GetNextPoint())
        //    {
        //        LASPoint point = lasreader.GetPointAt(2);
        //        Assert.LessOrEqual(Math.Abs(point.X - 630282.45), 0.0001);
        //        Assert.LessOrEqual(Math.Abs(point.Y - 4834500.0), 0.0001);
        //        Assert.LessOrEqual(Math.Abs(point.Z - 51.63), 0.0001);
        //        Assert.AreEqual(point.Intensity, 350);
        //        Assert.AreEqual(point.Classification, 1);
        //        Assert.AreEqual(point.ScanAngleRank, 0);
        //        Assert.AreEqual(point.UserData, 3);
        //        Assert.AreEqual(point.ScanFlags, 9);
        //        Assert.LessOrEqual(Math.Abs(point.Time - 413665.52880000003), 0.0001);
        //    }

        //}


    }
}

