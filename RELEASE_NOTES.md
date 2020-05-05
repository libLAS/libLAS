# libLAS Release Notes

##  1.8.2 (YYYY-MM-DD)

### Added
- Document method for color value range ([#175](https://github.com/libLAS/issues/175) and [#180](https://github.com/libLAS/libLAS/issues/180)).

### Changed
- Catch up with changes in GDAL 1.11 as well as 2.x.
- Enable `pkg-config` by default only on Unix and MinGW ([#120](https://github.com/libLAS/libLAS/issues/120)).
- `liblas-config.cmake` tries to find Boost when finding libLAS ([#174](https://github.com/libLAS/libLAS/issues/174)).
- Install `liblas-config.cmake` into `lib/cmake` ([b09f063](https://github.com/libLAS/libLAS/commit/b09f063)).
- Set CMake variable `CMAKE_MACOSX_RPATH` to `OFF` explicitly ([ce9bc0d](https://github.com/libLAS/libLAS/commit/ce9bc0d)).
- `FindGeoTIFF.cmake` can now libgeotiff headers on Fedora ([3c30360](https://github.com/libLAS/libLAS/commit/3c30360)).

### Deprecated
- None

### Removed
- Avoid use of deprecated API of GDAL 2.3 ([9c9799d](https://github.com/libLAS/libLAS/commit/9c9799d)).

### Fixed
- Fixed building against GDAL 2.3 in C++11 mode ([#140](https://github.com/libLAS/libLAS/issues/140)).
- Fixed `Open()` function to always return `NULL` on failure ([#168](https://github.com/libLAS/libLAS/issues/168)).
- Fixed calling convention to C# bindings to prevent `PInvokeStackImbalance`.
- Clarify the license text as 3-Clause BSD License ([#125](https://github.com/libLAS/libLAS/issues/125)).
- Ensure stream is deallocated in case of exception ([#162](https://github.com/libLAS/libLAS/issues/162), [#158](https://github.com/libLAS/libLAS/issues/158)).
- Bug in `Bounds::contains(Bounds)` ([8ea4c13](https://github.com/libLAS/libLAS/commit/8ea4c13)).
- Compiling problem when WITH_GEOTIFF is disabled ([#163](https://github.com/libLAS/libLAS/issues/163)).
- Header was missing from las2txt output ([#167](https://github.com/libLAS/libLAS/issues/167)).
- Avoid memory leak in C API ([9e9f140](https://github.com/libLAS/libLAS/commit/9e9f140)).
- Avoid slicing exceptions when catching ([f4ad631](https://github.com/libLAS/libLAS/commit/f4ad631)).
- Update use of Boost Bind placeholders ([#189](https://github.com/libLAS/libLAS/pull/189)).

### Security
- CVE-2018-20536 issue [#183](https://github.com/libLAS/libLAS/issues/183)
- CVE-2018-20537 issue [#184](https://github.com/libLAS/libLAS/issues/184)
- CVE-2018-20539 issue [#183](https://github.com/libLAS/libLAS/issues/183)
- CVE-2018-20540 issue [#182](https://github.com/libLAS/libLAS/issues/182)

## Acknowledgements

Adeel Ahmad, Andy Maloney, Bas Couwenberg, Ben Boeckel, Björn Piltz, Dan Lipsa,
Even Rouault, Felipe M. López, Florent Guiotte, Howard Butler, John Papadakis,
Māris Nartišs, Mateusz Loskot, Oskar Karlin, Panagiotis Koutsourakis,
Piotr Dobrowolski, Ronald van Haren, Sandro Mani, Wei Mingzhi, Yuksel Yonsel

## 1.8.1 (2016-08-23)

### Added
- New program `las2pg` to translate `.las` to PostgreSQL binary dump format.
- New program `las2col` to translate `.las` MonetDB columnar format.
- Support for libgeotiff 1.4.2.
* Add support for building on GNU/Hurd.
* Add support for building on GNU/FreeBSD.

### Changed
- Use key count instead of VLR size for padded GeoTIFF records.

### Deprecated
- None

### Removed
- None

### Fixed
- `las2las` tried to reproject header bounds without setting coordinates.
- Fixed some incorrect pointer dereferencing and avoid some C# `System.AccessViolationException`.

### Security
- None

## 1.8.0 (2014-08-02)

No significant new features have been added as part of this release, but a number of bugs have been fixed.
The most significant one is the ability for libLAS to work correctly with GDAL 1.10+

### Added
- Added Python 3 support.
- New Python script `las2tindex.py` to create a tile index of several LAS or LAZ files.

### Changed
- Apply filter using `int32_t` instead of `int8_t` for `ScanAngle`.

### Deprecated
- None

### Removed
- Remove dependency on `boost/cstdint.hpp`, replaced with `<stdint.h>`.
- Remove `liblas::guid`, which was a poor implementation of `boost::uuid::uuid`.
- Remove old LAStools utilities that no longer belong in liblas.
- Remove `Point::Point()` default constructor that was confusing, did not work correctly.
- Remove `sha1.hpp` in response to Debian bug showing it was non-free (really?).

### Fixed
- Fixed support for building libLAS against GDAL 1.10 or later.
- Fixed support for reading (but not interpreting) extra bytes in files.
- Fixed potential of segfault when LASzip does not give us an error message.
- Corrected conversion of offset between `.bin` and `.las` files.

### Security
- None

## 1.7.0 (2012-01-07)

### Added
- New C API function `LASReader_GetSummaryXML`.
- New C API functions `LASPoint_GetXML` and `LASHeader_GetXML`.
- New C API functions `LASHeader_SetHeaderPadding` and `LASHeader_GetHeaderPadding`.
- Python can now access raw point data and raw xyz data.
- Python classes `liblas.header.Header`, `liblas.point.Point`, and `liblas.file.File` have new `xml` properties for summary information.

### Changed
- LASzip 2.0+ is required if you wish to enable libLAS 1.7.0+ to read/write .laz files.
- GDAL 1.7.0+ is now required for coordinate system description and reprojection.
- libgeotiff 1.3.0+ is highly recommended, but things will continue to work in a degraded form with respect to vertical datum properties with older libgeotiff versions.
- CMake always finds for LASzip and if found, it is linked by default. LASzip is still optional, not required, to build libLAS via CMake configuration.
- LASzip improved error handling.

### Deprecated
- None

### Removed
- Removed `las2oci` program (replaced with PDAL).

### Fixed
- libLAS now writes 0xAABB by default into reserved field of VLRs for all supported LAS versions. It's a small nit as to whether or not this should be required for all LAS versions after 1.0.
- Fixed `mmap` support for the `liblas::Chipper`.
- `liblas-config` was putting Boost library noise in `--libs` line.
- `liblas::Point::SetHeader` was not scaling data properly when the header's size was not changing.
- `las2txt` was printing unprintable characters into output.
- `txt2las` was not writing RGB properly due to not setting the point's header.
- Indexing was failing to build in situations with highly concentrated points.
- `lasinfo --point --xml` was not printing valid XML.
- `USE_BOOST_IO` was not properly closing files.
- libLAS was not properly closing up some stream types.
- `lasinfo` was reporting "None" for spatial references for files that had GeoTIFF keys in the case when libLAS was built without GeoTIFF/GDAL support.
- The default CMake build type is now "Release". There's a huge performance difference, and you should make sure to build as "Release" if you think things are too slow.

### Security
- None

## 1.6.0 (2011-02-01)

Even though there has not been a major release of libLAS since April of 2009,
development has continued on the library. A number of new features, updates,
and bug fixes have been developed for the next libLAS release, to be called
libLAS 1.6.

### Added
- Rudimentary LAS 1.3 support.

  Support for reading and writing LAS 1.3 point data is not provided in
  libLAS 1.6. Support for reading point formats 4 or 5 is not yet complete,
  however.

- Generic interfaces

  A number of generic interfaces have been added to libLAS to support dynamic
  polymorphism. See `liblas/liblas.hpp` for the C++ interfaces. By implementing
  these interfaces, you can add your own reader/writer implementations
  as well as provide custom filtering and transformation capability.

- Schema description support
  Though it is specifically allowed by the standard but not widely implemented,
  it is possible to store extra data attached to each point after the requisite
  `PointFormat` data are stored. There is neither a regularized way to describe
  these data nor a way to capture metadata about this. To this end, I have
  proposed an XML schema document that could be stored in a VLR as well as
  schema-aware reader and writer implementations that can utilize that VLR to
  work with the data. See <https://lidarbb.cr.usgs.gov/index.php?showtopic=9075>
  for more details on the initial proposal of schema support.

- Alternative formats

  Support for reading the TerraSolid bin format as well as writing the
  Oracle Point Cloud format will be provided for libLAS 1.6, but this code
  will be moving to a new home shortly.  See [libPC](#libPC) for more details
  on these developments

- Generic spatial indexing

  A octree-based spatial index was developed for libLAS to speed up random,
  bounding-box-based queries to LAS files. It is released as part of
  libLAS 1.6, but its full implementation within the library is not yet
  complete. The index can store its data within VLRs (requires a file rewrite)
  in addition to in a file alongside the .las file.

- Support for LAZ as lossless compression for ASPRS LAS data using LASzip.

  The LASzip integration in libLAS allows you to directly read `.laz`
  (LASzip compressed LAS files) directly like you would an uncompressed
  `.las` file.
  You can compress your data from the command line via `las2las`,
  as well as use the new `liblas::{ReaderFactory|WriterFactory}` to work with
  compressed data in your software. Notice, LASzip compressed data comes with
  both a reading performance cost and a writing performance cost.

  LASzip compression has not yet come to the C or Python APIs yet,
  but this is planned for a future release.

- Caching reader

  A reader implementation that provides data caching will be provided at
  libLAS 1.6. If your data reading involves reading the data in multiple passes
  through the file, you can utilize the cached reader to cache the points
  (up to the size of the entire file) for faster repeated and random access.

- Seek support

  It is now possible to seek to a specific point in the file and start reading
  points.  This can significantly speed up the "random sampling" access strategy
  where one starts reading a run of points at a specific location in the file
  and then moves to a different location.

- Classification class

  A class is now provided to abstract the LAS classification value and help
  interpret the bit fields that are present for synthetic, key point,
  and withheld types.

- Vertical datum description support

  libLAS, in combination with a very recent GDAL/proj.4, can be used to preserve
  and describe the vertical datum GeoTIFF keys that are present in LAS files.
  Implementation based on the plan in RFC 2.

- Support for point processing and filtering.
- New command line programs: las2oci, lasblock, lasindex, laskernel, ts2las.
- Support for reading large files (>4 GB).
- las2las now has the ability to set color information from GDAL raster files.
  You need GDAL linked, of course.
- `pkgconfig` script now available (thanks Ed Hill)
- RPM package spec file s now available (thanks Ed Hill)

- Note on future: libPC

  There exists a need in the LiDAR and laser scanning fields for a library much
  like GDAL to provide translation to and from various formats. Further
  development with libLAS has caused it to currently accrete support for reading
  TerraSolid .bin files (ts2las) and write the Oracle Point Cloud (las2oci)
  format into Oracle databases. Additionally, there are a multitude of other
  formats with similar characteristics that would be excellent candidates for
  participating in a GDAL-like library for point cloud data. These include:

    - LAS
    - QFIT
    - Oracle Point Cloud
    - BAG
    - Fixed schema CSV/text
    - Variable schema CSV/text
    - others

  To that end, I will be embarking on developing a new library in the style
  of GDAL to provide support for reading and writing (to varying levels
  of capability) these formats.
  The plan is for it to be bootstrapped after the 1.6 release of libLAS.

### Changed
- Significant internal refactoring of C++ reading/writing code.

  The C++ reader and writer code of libLAS was a bit inflexible, and contained
  significant duplication. Giant updates would be required to the code as the
  ASPRS LAS standard committee adds new versions of the specification.
  Additionally, the old code's design was a bit rigid for adding more
  generic things.

  The reduction in duplication means going to only one place to make changes to
  the code.  In addition to not repeating ourselves, it provides us more
  flexibility to add new features and extensibility to allow the reader
  and writers to be overridden by user code.

- Faster binary I/O

  libLAS gained a more savvy implementation for its binary I/O
  which should provide some significant performance improvements.

- Rewritten lasinfo and las2las

  lasinfo and las2las have been completely rewritten to take advantage of
  faster access and summary methods within the libLAS C++ API.
  In addition to nearly double the speed of the previous libLAS-provided
  lasinfo and las2las implementations, both applications gained a number of
  features including much more expressive filtering and more detailed header
  and file transformation. The old versions of lasinfo and las2las are still
  available to people who have embedded them in processes and applications.
  They are now called lasinfo-old and las2las-old.

- Migrated to CMake as the official build configuration.
- Significant rewrite of las2las program to support more advanced processing and filtering during translation.
- libLAS libraries, `liblas` and `liblas_c`, are now versioned with SONAMEs on Unix systems.
- GDAL 1.8+ and libgeotiff 1.3.0+ should be considered minimum dependencies for libLAS 1.6.0.

### Deprecated
- None

### Removed
- Build configuration files for NMAKE.
- Build configuration files for GNU Autotools.

### Fixed
- Large number of fixes and corrections which are difficult to list all.
- libLAS C++ DLL/.so is built and classes are now properly exported on Windows.
- It's now possible to override the header's scale values as part of `ReprojectionTransform`.
- lasinfo summary display respects header's scale values to define precision
- Cleanups of a number of direct comparison of float/double values to use epsilon testing
- Fixed use of round instead of truncate integers when scaling.
- Fixed issue with reading global encoding.
- Fixed issue with writing headers in existing files opened for append.
- Fixed libgeotiff conflicts.
- las2txt: the classification number was being written out as a byte instead of int.
- numerous performance tweeks, in the form of eliminating unnecessary function calls and reordering operations where it made sense.

### Security
- None

### Acknowledgements
- Andrew Bell for writing a clever bit of code called the "chipper" that is a critical component in the process pipeline for tiling LiDAR data into a database such as Oracle or PostGIS.
- Dave Finnegan and Mike Smith and the US Army Corps of Engineers Cold Regions Research and Engineering Laboratory for providing the resources to keep me grinding away on libLAS.
- Frank Warmerdam for enhancing libLAS and the entire Open Source GIS stack to now give us the ability to do vertical datum transformation operations.
- Gary Huber for providing the octree index for libLAS that applications (and soon the library proper) can now start using to speed up their window queries.
- Mateusz Loskot for reworking the binary I/O and helping greatly with the design of the new `liblas::Schema` implementation.
- Mauricio Terneus of Certainty3D for the performance and LASzip testing help.

## 1.2.1 (2009-10-02)

### Added
- Point Source ID support in las2txt and txt2las.
- Support for classification filtering in las2las.

### Changed
- libLAS 1.2 now requires GDAL 1.7.

### Deprecated
- None

### Removed
- None

### Fixed
- GDAL transform translated points incorrectly when there is scale or translation on the points.
- Fixed setting header with correct byte location where to start writing point records.
- Fixed manipulating bit flags for returns, scan direction and flight line.
- Fixed `point.descale` function in Python bindings.
- Corrected memory management in Python bindings.
- Fixed support 'comma' as a separator in `las2txt`.

### Security
- None

## 1.2.0 (2009-03-25)

### Added
- Reading and writing support for ASPRS LAS 1.2.
- Support for color reading and writing to Python bindings.
- Support for reading and writing point source ID.
- New C++ classes: `LASColor`.
- New C API structures corresponding to the new C++ classes.
- New C API functions to read and write VLR-s.
- Support for setting arbitrary SRS and projection between SRS-s using GDAL (requires GDAL).
- Support for SRS operatios for Python bindings.
- Support for building against GDAL build tree.
- `LASPoint_GetPointSourceId` and `LASPoint_SetPointSourceId` were missing from C API.

### Changed
- Renamed C++ API class `LASSRS` to `LASSpatialReference`.
- Renamed C++ API class `LASVLR` to `LASVariableRecord`.

### Deprecated
- None

### Removed
- `LASHeader::GetProj4` and `LASHeader::SetProj4` in the C++ API and `LASHeader_GetProj4` and `LASHeader_SetProj4` in the C API have been removed since the libLAS 1.0 release. Your code may have to be updated to reflect this change.

### Fixed
- Corrected detecting of libgeotiff during build.
- Fixed memory management and memory leaks in C API functions operting on strings.
- Fixed handling of SRS and GeoTIFF keys.
- Fixed missing reset operation on reader when re-using it with iterators.
- Fixed version reporting in Python bindings.
- Fixed None return value for void functions of the C API.

### Security
- None

## 1.0.1 (2009-02-04)

### Added
- New option `-M` to `las2txt` to output point index.
- New property `intensity` in `point` class of Python bindings.

### Changed
- Improved point equality check in `LASPoint` class.
- Improved time reporting in Python bindings.

### Deprecated
- None

### Removed
- None

### Fixed
- Version reporting in Python bindings.

### Security
- None

## 1.0.0 (2008-10-11)

First major release of libLAS source code.

### Added
- Reading and writing ASPRS LAS 1.0 and 1.1 point record formats 0 nad 1.
- Reading and writing Variable-Length Records (VLR).
- Reading spatial reference and projection from GeoTIFF keys stored in VLR(requires libgeotiff).
- Support for reading spatial reference as PROJ.4 definitions (requires libgeotiff or GDAL).
- Collection of command line programs: las2las, las2txt, lasdiff, lasinfo, lasmerge, txt2las.
- Translator of points from LAS to OGR vector dataset: las2ogr (requires GDAL/OGR).
- Bindings for Python 2 (<2.5).
- Bindings for C# for .NET and Mono.
- Support for building with MSVC++ 7.1 or later or GCC 4.1 or later on Unix and Windows.
- Support for Big-Endian.

### Changed
- None

### Deprecated
- None

### Removed
- None

### Fixed
- None

### Security
- None
