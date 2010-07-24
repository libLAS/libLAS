#!/usr/bin/env python

"""
/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Pull color from a GDAL raster into an LAS file
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Howard Butler
 * Copyright (c) 2010, Aaron Reyna 
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following
 * conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department
 *       of Natural Resources nor the names of its contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************************/
 """
 
import optparse
import os
import sys

from liblas import header
from liblas import color
from liblas import file as lasfile
from liblas import schema
from liblas import srs

from osgeo import gdal
gdal.UseExceptions()

class Colorizer(object):

    def construct_parser(self):
        from optparse import OptionParser, OptionGroup
        usage = "usage: %prog [options] arg"
        parser = OptionParser(usage)
        g = OptionGroup(parser, "Base options", "Basic Translation Options")
        g.add_option("-c", "--connection", dest="connection",
                          help="OCI connection string", metavar="CONNECTION")
        g.add_option("-o", "--output", dest='output',
                          help="LAS file to write", metavar="OUTPUT")
        g.add_option("-i", "--input", dest='input',
                          help="LAS file to read", metavar="INPUT")
        g.add_option("-r", "--image", dest='image',
                          help="GDAL raster image to use for color ", metavar="IMAGE")
        g.add_option("-b", "--bands", dest='bands',
                          help="Comma-separated list of bands numbers to use for R,G,B ", metavar="BANDS")

        g.add_option("-s", "--scale", dest='scale',
                          help="Scale the color values to 16 bit by multiplying by 256 ", metavar="SCALE")
                          
        g.add_option("-t", "--target-srs", dest='target_srs',
                          help="Output SRS for LAS file", metavar="TARGET_SRS")

        g.add_option("-a", "--assign-srs", dest='assign_srs',
                          help="Assign specified SRS for input LAS file", metavar="ASSIGN_SRS")
                          
        g.add_option("-w", "--overwrite",
                          action="store_true", dest="overwrite", 
                          help="overwrite the existing LAS output file")

        g.add_option("-q", "--quiet",
                          action="store_false", dest="verbose", default=False,
                          help="Don't say what we're doing on stdout")
                          
        parser.add_option_group(g)

        if self.opts:
            g = OptionGroup(parser, "Special Options", "Special options")
            for o in self.opts:
                g.add_option(o)
            parser.add_option_group(g)
            
        parser.set_defaults(verbose=True, precision = 6)

        self.parser = parser
        
    def __init__(self, arguments, options=None):
        self.las = None
        self.image = None
        self.output = None
        self.sql = None
        
        self.opts = options
        self.construct_parser()
        self.options, self.args = self.parser.parse_args(args=arguments)
        

        if not self.options.image:
            try:
                self.options.image = self.args[1]
                self.options.image = gdal.Open(self.options.image)
            except IndexError:
                raise self.parser.error("No input image was selected to use for color values!")   
            except Exception as inst:
                raise self.parser.error("Unable to open GDAL image: %s" % inst.args[0])

        if not self.options.bands:
            self.options.bands = (1,2,3)
        else:
            self.options.bands = [int(i) for i in self.options.bands.split(',')]

        if not self.options.output:
            try:
                self.options.output = self.args[2]
            except IndexError:
                self.options.output = 'output.las'

        if self.options.output:
            self.options.output = os.path.abspath(self.options.output)
            if os.path.isdir(self.options.output):
                raise self.parser.error("Output '%s' is a directory, not a file " % self.options.output)
            
            if os.path.exists(self.options.output):
                if not self.options.overwrite:
                    raise self.parser.error("Output file '%s' exists, but you have not selected the --overwrite option" % self.options.output)
        else:
            raise self.parser.error("No output was specified")
        
        if self.options.target_srs:
            s = srs.SRS()
            s.set_userinput(self.options.target_srs)
            self.options.target_srs = s
            
        if self.options.assign_srs:
            s = srs.SRS()
            s.set_userinput(self.options.assign_srs)
            self.options.assign_srs = s

        if self.args:
            self.options.las = self.args[0]
            try:
                f = self.options.las
                self.options.las = lasfile.File(f, mode='r')
                if self.options.assign_srs:
                    # if we're assigning an SRS, take the header from the 
                    # file, grab it, and then stick our SRS on it and use it
                    # to read the file
                    h = self.options.las.header
                    h.srs = self.options.assign_srs
                    
                    self.options.las = lasfile.File(f, header=h)
            except Exception as inst:
                raise self.parser.error("Unable to open input LAS file: %s" % inst.args[0])
            
    def create_output(self):
        # Use the same header as before, but just add support for color
        # and adjust our version to 1.2
        h = self.options.las.header
        s = h.schema
        s.color = True
        h.schema = s
        h.version = '1.2'
        if self.options.target_srs:
            h.srs = self.options.target_srs
        self.options.output = lasfile.File(self.options.output, header=h, mode='w')
        if self.options.target_srs:
            self.options.output.set_srs(self.options.target_srs)

    def process(self):
        rows = self.options.image.RasterYSize
        cols = self.options.image.RasterXSize
        bands = self.options.image.RasterCount

        # get georeference info
        transform = self.options.image.GetGeoTransform()
        xOrigin = transform[0]
        yOrigin = transform[3]
        pixelWidth = transform[1]
        pixelHeight = transform[5]
        
        red_band = self.options.image.GetRasterBand(self.options.bands[0]) # 1-based index
        green_band = self.options.image.GetRasterBand(self.options.bands[1])
        blue_band = self.options.image.GetRasterBand(self.options.bands[1])
        
        for p in self.options.las[0:400]:

            # compute pixel offset
            xOffset = int((p.x - xOrigin) / pixelWidth)
            yOffset = int((p.y - yOrigin) / pixelHeight)

            red = red_band.ReadAsArray(xOffset, yOffset, 1, 1)
            green = green_band.ReadAsArray(xOffset, yOffset, 1, 1)
            blue = blue_band.ReadAsArray(xOffset, yOffset, 1, 1)
                
            red, green, blue = int(red), int(green), int(blue)
                
            if self.options.scale:
                red =  red * 256
                green = green * 256
                blue = blue * 256

            c = color.Color(red, blue, green)
            p.color = c
            self.options.output.write(p)
def main():

    
    d = Colorizer(sys.argv[1:])
    d.create_output()
    d.process()

if __name__=='__main__':
    main()

# #############################################
# import os, sys
# from liblas import file
# from liblas import header
# from liblas import color
# from liblas import *
# 
# try:
#   from osgeo import ogr, gdal
#   from osgeo.gdalconst import *
#   os.chdir('C:\\crap\\county\\Crook')
# except ImportError:
#   import ogr, gdal
#   from gdalconst import *
#   os.chdir(r'C:\\crap\\county\\Crook')
# print "loaded"
# 
# lassy = 'D:\\aaron_working\\DESCHUTTES\\Decluttered_no_birds\\DES_04593.las'
# 
# gdal.UseExceptions()
# 
# # open the image
# img = gdal.Open('naip_1_1_1n_s_or013_2005_1_C4593.img', GA_ReadOnly)
# if img is None:
#   print 'Could not open aster.img'
#   sys.exit(1)
# print "loaded img"
# # get image size
# rows = img.RasterYSize
# cols = img.RasterXSize
# bands = img.RasterCount
# 
# # get georeference info
# transform = img.GetGeoTransform()
# xOrigin = transform[0]
# yOrigin = transform[3]
# pixelWidth = transform[1]
# pixelHeight = transform[5]
# 
# data=file.File(lassy, mode='r')
# 
# print "creating .LAS file"
# h = header.Header()
# 
# h.dataformat_id = 1
# 
# h.minor_version = 2
# newdata=file.File('D:\\aaron_working\\DESCHUTTES\\Decluttered_no_birds\\DES_04593aaaab.las', mode='w', header=h)
# 
# for p in data:
#     pt = point.Point() 
#     xL = p.x
#     yL = p.y
#     pt.x = p.x
#     pt.y = p.y
#     pt.z = p.z
#     pt.intensity = p.intensity
#     pt.flightline_edge = p.flightline_edge
#     pt.scan_flags = p.scan_flags
#     pt.number_of_returns = p.number_of_returns
#     pt.classification = p.classification
#     pt.scan_angle = p.scan_angle
#     pt.user_data = p.user_data
#     
#     # compute pixel offset
#     xOffset = int((xL - xOrigin) / pixelWidth)
#     yOffset = int((yL - yOrigin) / pixelHeight)
#     # loop through the bands
#     for j in range(bands):
#         band1 = img.GetRasterBand(1) # 1-based index
#         # read data and add the value to the string
#         RED = band1.ReadAsArray(xOffset, yOffset, 1, 1)
# 
#         band2 = img.GetRasterBand(2)
#         GREEN = band1.ReadAsArray(xOffset, yOffset, 1, 1)
# 
#         band3 = img.GetRasterBand(3)
#         BLUE = band1.ReadAsArray(xOffset, yOffset, 1, 1)
#         r16RED = int(RED)* 256
#         r16GREEN = int(GREEN) * 256
#         r16BLUE = int(BLUE) * 256
#         
#         #print r16RED, r16GREEN, r16BLUE
#   
#     pt.color = color.Color(r16RED, r16GREEN, r16BLUE)
#     newdata.write(pt)
#     
# newdata.close()
# data.close()