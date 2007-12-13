****************************************************************

     LASzip: Compressing LIDAR data in binary LAS format

           http://www.cs.unc.edu/~isenburg/laszip/

****************************************************************

This document describes the prototype compressor for LIDAR data
stored in binary LAS format (1.0 / 1.1).

try these sources for sample lidar data in LAS format:

http://www.geoict.net/LidarData/Data/OptechSampleLAS.zip
http://www.qcoherent.com/data/LP360_Sample_Data.rar
http://www.appliedimagery.com/Serpent%20Mound%20Model%20LAS%20Data.las

This compressor is under development. If you have a dataset for
which the compressor does not work correctly or even crashed please
let me (isenburg@cs.unc.edu) know.

example usage:

>> laszip lidar.las

compresses the lidar file 'lidar.las' to the file 'lidar.laz'

>> laszip lidar.laz

decompresses the lidar file 'lidar.laz' to the file 'lidar.las'
this will overwrite any exiting file.

>> laszip lidar.las lidar.las.lz

compresses the lidar file 'lidar.las' to the file 'lidar.laz.gz'

>> laszip lidar.las.lz

decompresses the lidar file 'lidar.las.lz' to the file 'lidar.las'
this will overwrite any exiting file.

>> laszip lidar.las lidar_comp.laz
>> laszip -i lidar.las -o lidar_comp.laz
>> laszip -i lidar.las -olaz > lidar_comp.laz

all three commands will compresses the lidar file 'lidar.las' to
the file 'lidar_comp.laz'

------------------------------------------------------------

you can test whether the compressed and the original file are
identical with:

>> lasdiff lidar1.las lidar2.las
>> lasdiff lidar1.las lidar1.laz
>> lasdiff lidar1.las lidar1.las.lz
