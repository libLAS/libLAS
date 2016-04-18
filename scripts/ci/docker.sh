#!/bin/bash

# laszip/docs image has all of the Sphinx
# dependencies need to build libLAS's docs

docker pull pdal/dependencies
docker pull laszip/docs

