@echo off

set COMPILER="Visual Studio 10 Win64"
set COMPILER="Visual Studio 10"

set BUILD_TYPE=Release
set BUILD_TYPE=Debug

set UTILS_DIR=c:\Utils
set DEV_DIR=d:\dev


set LIBLAS=%DEV_DIR%\liblas
set LASZIP=%DEV_DIR%\laszip
set BOOST=%UTILS_DIR%\boost_pro_1_46_1

cmake -G %COMPILER% ^
    -DBOOST_INCLUDEDIR=%BOOST% ^
    -DWITH_GDAL=OFF ^
    -DWITH_GEOTIFF=OFF ^
    -DWITH_ORACLE=OFF ^
    -DWITH_LASZIP=ON ^
    -DLASZIP_INCLUDE_DIR=%LASZIP%\include ^
    -DLASZIP_LIBRARY=%LASZIP%\bin\Debug\Debug\laszip.lib ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_VERBOSE_MAKEFILE=OFF ^
    %LIBLAS%
