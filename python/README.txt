******************************************************************************
libLAS - LAS 1.0/1.1/1.2 ASPRS LiDAR data translation toolset
******************************************************************************

libLAS is a C/C++ library for reading and writing the very common `LAS`
`LiDAR`_ format. The `ASPRS LAS format`_ is a sequential binary file format
used to store data from LiDAR sensors and by LiDAR processing software for
data interchange and archival.

libLAS supports the ASPRS LAS format specification versions: 1.0, 1.1, 1.2.

libLAS' initial development was supported in 2007-2008 by the `IGSB`_ of the
Iowa DNR for use in its state-wide `LIDAR`_ project.  Later, support for 
libLAS was provided by a number of organizations including the 
`U.S. Army Cold Regions Research and Engineering Laboratory`_.

.. _`U.S. Army Cold Regions Research and Engineering Laboratory` : http://www.crrel.usace.army.mil/

Pre-requisites
------------------------------------------------------------------------------

The Python bindings require the libLAS base C and C++ libraries be installed.  
Obtain a source copy of the library from http://liblas.org/download.html and 
follow the compilation instructions at http://liblas.org/compilation.html to 
build and install the library.

News
------------------------------------------------------------------------------

02/02/2011

libLAS 1.6.0 adds a number of new features:

* Compression/decompression support via `LASzip`_
* Octree indexing
* Tiling support via `lasblock`_
* Loading data to `Oracle Point Cloud`_ via `las2oci`_
* Horizontal and vertical reprojection
* XML summary output via `lasinfo`_

Updates to the Python API include support for manipulating raw point data, 
rescaling support through manipulation of `point.header`, and a number of 
additions to bring things in line with the libLAS C++ API.

Notes
------------------------------------------------------------------------------

The Windows builds provided by PyPI include statically-linked `LASzip`_ support, 
but do not provide coordinate system support via `GDAL`_ or `libgeotiff`_.  
Because the Windows binaries provide `LASzip`_ support, which is LGPL, they 
should be considered LGPL as well.  

If you need coordinate system support, or fully-featured `tools`_, obtain an 
install via `OSGeo4W`_.  `OSGeo4W`_ does include its own Python 2.5, however, 
which may what you need, but libLAS' bindings are not tied to any specific 
Python version (they use `ctypes`_), and you should be able to piece together 
a system that way.

Documentation
------------------------------------------------------------------------------

A `tutorial`_ and `class documentation`_ are provided.

License
------------------------------------------------------------------------------

libLAS is available under the terms of the `BSD License`_. 
See `License`_ for a copy.

.. _`ctypes`: http://docs.python.org/library/ctypes.html
.. _`tools`: http://liblas.org/utilities/
.. _`GDAL`: http://gdal.org
.. _`libgeotiff`: http://trac.osgeo.org/geotiff/
.. _`lasblock`: http://liblas.org/utilities/lasblock.html
.. _`LASzip`: http://laszip.org
.. _`las2oci`: http://liblas.org/utilities/las2oci.html
.. _`lasinfo`: http://liblas.org/utilities/lasinfo.html
.. _`Oracle Point Cloud`: http://download.oracle.com/docs/cd/B28359_01/appdev.111/b28400/sdo_pc_pkg_ref.htm"
.. _`class documentation`: http://liblas.org/python.html
.. _`tutorial`: http://liblas.org/tutorial/python.html

.. _`LIDAR`: http://en.wikipedia.org/wiki/LIDAR
.. _`IGSB`: http://www.igsb.uiowa.edu/
.. _`Martin Isenburg`: http://www.cs.unc.edu/~isenburg/
.. _`LAStools`: http://www.cs.unc.edu/~isenburg/lastools/
.. _`Martin Isenburg and Jonathan Shewchuk`: http://www.cs.unc.edu/~isenburg/lastools/
.. _`LAS Format`: http://www.lasformat.org/
.. _`ASPRS Standards Committee`: https://www.asprs.org/divisions-committees/lidar-division/laser-las-file-format-exchange-activities
.. _`ASPRS LAS format`: https://www.asprs.org/divisions-committees/lidar-division/laser-las-file-format-exchange-activities

.. _`BSD license`: http://www.opensource.org/licenses/bsd-license.php
.. _`OSGeo4W`: http://wiki.osgeo.org/wiki/OSGeo_Win32_Installer
.. _`License`: http://liblas.org/copyright.html#license