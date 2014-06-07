"""
$Id$

"""
try:
    from setuptools import setup, Extension
except ImportError:
    from distutils.core import setup, Extension
    
from sys import version_info

try:
    readme = file('README.txt','rb').read()
except:
    readme = ''

install_requires = ['setuptools']

import os

version = '1.8.0'

import socket

# if we're building on Howard's XP VM, we'll include DLLs from his setup
if socket.gethostname() == 'fire-windows':
    data_files=[('DLLs',[r'D:\liblas\bin\RelWithDebInfo\liblas_c.dll',
                         r'D:\liblas\bin\RelWithDebInfo\liblas.dll',]),]
else:
    data_files = None
        
setup(name          = 'libLAS',
      version       = version,
      description   = 'LAS 1.0/1.1/1.2 LiDAR data format translation',
      license       = 'BSD',
      keywords      = 'DEM elevation LIDAR',
      author        = 'Howard Butler',
      author_email  = 'hobu.inc@gmail.com',
      maintainer    = 'Howard Butler',
      maintainer_email  = 'hobu.inc@gmail.com',
      url   = 'http://liblas.org',
      long_description = readme,
      packages      = ['liblas'],
      install_requires = install_requires,
      test_suite = 'tests.test_suite',
      data_files = data_files,
      zip_safe = False,
      classifiers   = [
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Topic :: Scientific/Engineering :: GIS',
        ]
)

