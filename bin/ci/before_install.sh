#!/bin/bash
# Installs requirements for libLAS
source ./bin/ci/common.sh
echo "$(tmstamp) *** before_install::apt-get starting $(date) ***"
sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 16126D3A3E5C1192
sudo add-apt-repository ppa:ubuntugis/ubuntugis-unstable -y
sudo apt-get update -qq
sudo apt-get install -qq cmake libboost-program-options-dev libboost-thread-dev libboost-system-dev libboost-filesystem-dev
sudo apt-get install \
    libgdal-dev \
    libgeos-dev \
    libgeos++-dev \
    libproj-dev \
    libxml2-dev \
    libtiff4-dev
# install libgeotiff from sources
wget http://download.osgeo.org/geotiff/libgeotiff/libgeotiff-1.4.0.tar.gz
tar -xzf libgeotiff-1.4.0.tar.gz
cd libgeotiff-1.4.0
./configure --prefix=/usr && make && sudo make install
cd $TRAVIS_BUILD_DIR

echo "$(tmstamp) *** before_install::apt-get finished $(date) ***"
gcc --version
clang --version
