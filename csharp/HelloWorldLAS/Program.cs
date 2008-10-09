using System;
using System.Text;
using LibLAS;

class Program
{
    static void Main(string[] args)
    {
        try
        {
            LASReader lasreader = new LASReader(@"c:\las\sample_our2.las");

            LASPoint laspoint;

            LASHeader lasheader = lasreader.GetHeader();
            Console.WriteLine(lasheader.SoftwareId);// 
            lasheader.VersionMinor = 0;
            LASWriter laswriter = new LASWriter(@"c:\las\sample_our.las", lasheader, LASReadWriteMode.LASModeWrite);

            Console.WriteLine("Number of points in file= {0}", lasheader.PointRecordsCount);

            while (lasreader.GetNextPoint())
            {
                laspoint = lasreader.GetPoint();
                laspoint.X = laspoint.X + 3;

                //Console.WriteLine(laspoint.X + "," + laspoint.Y + "," + laspoint.Z);

                laswriter.WritePoint(laspoint);
            }
        }
        catch (LASException e)
        {
            Console.WriteLine("\nLASException! Msg: {0}", e.Message);
        }
        catch (SystemException e)
        {
            Console.WriteLine("\nException! Msg: {0}", e.Message);
        }
        catch
        {
            Console.WriteLine("Unknown exception caught");
        }
        finally
        {
            Console.WriteLine("Do i need something to do?");
        }
        Console.WriteLine("End of file");
        Console.Read();
    }
}
