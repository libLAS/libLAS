# libLAS

libLAS is a C/C++ library for reading and writing the very common `LAS`
LiDAR format. The [ASPRS LAS format](https://www.asprs.org/divisions-committees/lidar-division/laser-las-file-format-exchange-activities)
is a sequential binary file format used to store data from LiDAR sensors
and by LiDAR processing software for data interchange and archival.

libLAS supports the ASPRS LAS format specification versions:
1.0, 1.1, 1.2 and 1.3 (rudimentary support).

http://liblas.org

## Build Status

| Branch | Travis CI | AppVeyor | Coverity |
|:--- |:--- |:--- |:--- |
|`master`| [![master](https://travis-ci.org/libLAS/libLAS.svg?branch=master)](https://travis-ci.org/libLAS/libLAS/) | [![master](https://ci.appveyor.com/api/projects/status/r2ajb1qwe9rh0xkd/branch/master?svg=true)](https://ci.appveyor.com/project/mloskot/liblas?branch=master) | [![coverity_scan](https://scan.coverity.com/projects/10975/badge.svg)](https://scan.coverity.com/projects/liblas-liblas) |

## Building

Prerequisites:

- C++03 compiler
- CMake 2.8 or later
- Boost C++ Libraries 1.42 or later
- Libraries to enable detailed features: GDAL and PROJ4, libgeotiff, LASzip.
