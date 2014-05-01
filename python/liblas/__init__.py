#from .core import *
from .core import get_version
from .core import las
version = get_version()
HAVE_GDAL = bool(las.LAS_IsGDALEnabled())
HAVE_LIBGEOTIFF = bool(las.LAS_IsLibGeoTIFFEnabled())

import sys

version = sys.version_info[:3]

from . import file
from . import point
from . import header
from . import vlr
from . import color
from . import srs
