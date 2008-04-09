from setuptools import setup, Extension
from sys import version_info

# Require ctypes egg only for Python < 2.5
install_requires = ['setuptools']
if version_info[:2] < (2,5):
    install_requires.append('ctypes')


setup(name          = 'libLAS',
      version       = '0.9.2',
      description   = 'LAS 1.0/1.1 LiDAR data format reader',
      license       = 'BSD',
      keywords      = 'DEM elevation LIDAR',
      author        = 'Howard Butler',
      author_email  = 'hobu.inc@gmail.com',
      maintainer    = 'Howard Butler',
      maintainer_email  = 'hobu.inc@gmail.com',
      url   = 'http://liblas.org',
#      long_description = readme_text,
      packages      = ['liblas'],
      install_requires = install_requires,
      test_suite = 'tests.test_suite',
      classifiers   = [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Topic :: Scientific/Engineering :: GIS',
        ],
)

