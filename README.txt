LASreader and LASwriter

This is the API for reading and writing LAS files from and to the
standard format (*.las) as defined by (www.lasformat.org) as well
from and to our compressed format (*.las or *.las.lz).

Included are libraries (./lib and ./libD) and header files (./inc)
plus two example projects (./examples) that demonstrate how to use
our LASreader and LASwriter classes. All this is for MSVC 6.0. 

We also include our sources (./src) for the LASreader and LASwriter
classes. However, we did not include the source for the compressor.
For the moment the compressor is only accessible through function
calls to the libs.

try these sources for sample lidar data in LAS format:

http://www.geoict.net/LidarData/Data/OptechSampleLAS.zip
http://www.qcoherent.com/data/LP360_Sample_Data.rar
http://www.appliedimagery.com/Serpent%20Mound%20Model%20LAS%20Data.las

Several tools have been implemented using this API and their source
code and project files can be found in the ./tools directory.

The precompiled binary executables are in the ./bin directory.

If you find this code useful or if you crash the code or if you have
suggestions, please email me.

Martin (isenburg@cs.unc.edu), May 2007
