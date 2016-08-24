@echo off

cmake -G "Visual Studio 14 2015 Win64" ^
    -DCMAKE_INSTALL_PREFIX=C:\liblas ^
    -DBOOST_ROOT=C:\Libraries\boost_1_60_0 ^
    -DBOOST_LIBRARYDIR=C:\Libraries\boost_1_60_0\lib64-msvc-14.0 ^
    .
