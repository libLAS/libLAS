****************************************************************

  LASinfo : simply prints the header contents and a short
            summary of the points. when there are differences
            between header info and point content they are
            reported as a warning.

            can also be used to modify entries in the header
            as described below

****************************************************************

example usage:

>> lasinfo lidar.las

reports header and point information.

>> lasinfo -i lidar.las -variable

also reports the contents of any variable headers there might be.

>> lasinfo -i lidar.las -nocheck

omits reading over all the points. only reports header information.

>> lasinfo -i lidar.las -repair

if there are missing or wrong entries in the header they are corrected

>> lasinfo -i lidar.las -repair_boundingbox

reads all points, computes their bounding box, and updates it in the header

>> lasinfo -i lidar.las -file_creation 8 2007

sets the file creation day/year in the header to 8/2007

>> lasinfo -i lidar.las -system_identifier "hello world!"

copies the first 31 characters of the string into the system_identifier field of the header 

>> lasinfo -i lidar.las -generating_software "this is a test (-:"

copies the first 31 characters of the string into the generating_software field of the header 

----

try these sources for sample lidar data in LAS format:

http://www.geoict.net/LidarData/Data/OptechSampleLAS.zip
http://www.qcoherent.com/data/LP360_Sample_Data.rar
http://www.appliedimagery.com/Serpent%20Mound%20Model%20LAS%20Data.las

if you find bugs let me (isenburg@cs.unc.edu) know.
