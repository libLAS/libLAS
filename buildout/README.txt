libLAS and friends builout
==================================

Completely stolen from Shapely/gispython.org

To begin:

  $ virtualenv .
  $ source ./bin/activate
  $ python bootstrap.py
  $ buildout
  $ . ./setenv
  $ labpy
  >>> from osgeo import gdal
  ...
