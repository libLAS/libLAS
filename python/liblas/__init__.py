from core import *
version = get_version()
HAVE_GDAL = bool(las.LAS_IsGDALEnabled())
HAVE_LIBGEOTIFF = bool(las.LAS_IsLibGeoTIFFEnabled())

import file
import point
import header
import vlr
import color
import srs
