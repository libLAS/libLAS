#!/usr/bin/env python
##############################################################################
#                                                                             #
# las2tindex.py                                                               #
#                                                                             #
# begin : 2012-03-14                                                          #
# authors: Luca Delucchi                                                      #
# copyright : (C) 2012 by luca delucchi, Fondazione Edmund Mach               #
# email : lucadelucchi at fmach dot it                                        #
#                                                                             #
# All rights reserved.                                                        #
#                                                                             #
# Permission is hereby granted, free of charge, to any person obtaining a     #
# copy of this software and associated documentation files (the "Software"),  #
# to deal in the Software without restriction, including without limitation   #
# the rights to use, copy, modify, merge, publish, distribute, sublicense,    #
# and/or sell copies of the Software, and to permit persons to whom the       #
# Software is furnished to do so, subject to the following conditions:        #
#                                                                             #
# The above copyright notice and this permission notice shall be included     #
# in all copies or substantial portions of the Software.                      #
#                                                                             #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL     #
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         #
##############################################################################

import sys, os
import optparse
import subprocess
import re

try:
    import osgeo.ogr as ogr
    import osgeo.osr as osr
except ImportError:
    try:
        import osgeo.ogr as ogr
        import osgeo.osr as osr
    except ImportError:
        print "GDAL Python is missing"
        sys.exit( 1 )
        
try:
    import liblas
    llas = True
except:
    print "libLas python is missing, using script"
    llas = False

#classes for required options
strREQUIRED = 'required'

class OptionWithDefault(optparse.Option):
    ATTRS = optparse.Option.ATTRS + [strREQUIRED]
    
    def __init__(self, *opts, **attrs):
        if attrs.get(strREQUIRED, False):
            attrs['help'] = '(Required) ' + attrs.get('help', "")
        optparse.Option.__init__(self, *opts, **attrs)

class OptionParser(optparse.OptionParser):
    def __init__(self, **kwargs):
        kwargs['option_class'] = OptionWithDefault
        optparse.OptionParser.__init__(self, **kwargs)
    
    def check_values(self, values, args):
        for option in self.option_list:
            if hasattr(option, strREQUIRED) and option.required:
                if not getattr(values, option.dest):
                    self.error("option %s is required" % (str(option)))
        return optparse.OptionParser.check_values(self, values, args)

def main():
    """Main function"""
    #usage
    usage = "usage: %prog [options] las_files"
    parser = OptionParser(usage=usage)
    #output filename
    parser.add_option("-o", "--output", dest="name", required=True,
                      help="output file")        
    #ogr format
    parser.add_option("-f", "--format", dest="ogr", default='ESRI Shapefile',
                      metavar="OGR", help="OGR format for output file" \
                      + " [default=%default]")
    #epsg code
    parser.add_option("-e", "--epsg", dest="epsg", default=4326,
                      metavar="EPSG", help="EPSG code of input and output file" \
                      + " [default=%default]")
                      
    #return options and argument
    (options, args) = parser.parse_args()
    #test if args[0] it is set
    if len(args) == 0:
        parser.error("You have to pass the las files to create the tile index")
    
    #create the SpatialReference from epsg code
    outSpatial = osr.SpatialReference()
    outSpatial.ImportFromEPSG(int(options.epsg))
    #create the output driver
    outDriver = ogr.GetDriverByName(options.ogr)
    if outDriver is None:
        print "%s driver not available.\n" % options.ogr
        sys.exit( 1 )
    outputDSN = options.name
    #search if the file already exist
    if os.path.exists(outputDSN):
        outDriver.DeleteDataSource(outputDSN)
    #create output datasource
    outDatasource = outDriver.CreateDataSource(outputDSN)
    if outDatasource is None:
        print "Creation of output file failed.\n"
        sys.exit( 1 )    
    #create output layer
    outLayer = outDatasource.CreateLayer('las2tindex', srs=outSpatial, geom_type=ogr.wkbPolygon)
    if outLayer is None:
        print "Layer creation failed.\n"
        sys.exit( 1 )    
    #output field definition
    outField = ogr.FieldDefn("location", ogr.OFTString)
    outField.SetWidth( 32 )
    if outLayer.CreateField ( outField ) != 0:
        print "Creating 'location' field failed.\n"
        sys.exit( 1 )
    #for each file create the polygon of bounding box
    for i in args:
        # if python liblas it is present use it
        if llas:
            lasfile = liblas.file.File(i,mode='r')
            header = lasfile.header
            minimum = header.get_min()
            maximum = header.get_max()
        # else use lasinfo command
        elif not llas:
            command = "lasinfo %s" % i
            lasout = subprocess.Popen(command, shell=True,stdout=subprocess.PIPE).stdout
            bbox = []
            # for each line of output check min and max values
            for line in lasout.readlines():
                if re.search(r'Min X Y Z:', line) is not None or \
                re.search(r'Max X Y Z:', line) is not None:
                    values=line.split(':')[1].lstrip().split()
                    del values[0]
                    bbox.append(values)
                if len(bbox) == 2:
                    break
            minimum = bbox[0]
            maximum = bbox[1]
        #create the points of polygon
        ld = "%s %s" % (minimum[0],minimum[1])
        lu = "%s %s" % (minimum[0],maximum[1])
        ru = "%s %s" % (maximum[0],maximum[1])
        rd = "%s %s" % (maximum[0],minimum[1])
        # create the feature
        feat = ogr.Feature( outLayer.GetLayerDefn())
        feat.SetField( "location", i )
        wkt = 'POLYGON((%s,%s,%s,%s,%s))' % (ld, lu, ru, rd, ld)
        poly = ogr.CreateGeometryFromWkt(wkt, outSpatial)
        feat.SetGeometry(poly)
        #load the feature into vector layer
        if outLayer.CreateFeature(feat) != 0:
            print "Failed to create feature in vector file.\n"
            sys.exit( 1 )
        feat.Destroy()        

    outDatasource = None
    print "%s created" % outputDSN
    
#add options
if __name__ == "__main__":
    main()

