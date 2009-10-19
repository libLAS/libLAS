#!/usr/bin/env python

from liblas import file as lasfile
from liblas import header
from liblas import point

import glob
import struct

import os, sys


import cx_Oracle as oci

# big-endian DOUBLE, DOUBLE, DOUBLE, LONG, LONG
format = '>dddll'
ptsize = struct.calcsize(format)



class Translator(object):

    def construct_parser(self):
        from optparse import OptionParser, OptionGroup
        usage = "usage: %prog [options] arg"
        parser = OptionParser(usage)
        g = OptionGroup(parser, "Base options", "Basic Translation Options")
        g.add_option("-c", "--connection", dest="connection",
                          help="OCI connection string", metavar="CONNECTION")
        g.add_option("-o", "--output", dest='output',
                          help="LAS file to write", metavar="OUTPUT")
        g.add_option("-s", "--sql", dest='sql',
                          help="SQL to select the point cloud ", metavar="SQL")
        g.add_option("-l", "--column", dest='column',
                          help="Column name containing the point cloud object ", metavar="COLUMN")

        g.add_option("-p", "--precision", dest='precision',
                          help="Numeric precision (# of digits) to maintain for the output file ", metavar="PRECISION")
                          
        g.add_option("-w", "--overwrite",
                          action="store_true", dest="overwrite", 
                          help="overwrite the existing file")

        g.add_option("-m", "--min-offset",
                          action="store_false", dest="offset", 
                          help="Use the minimum values as base for offset")
                          
        g.add_option("-q", "--quiet",
                          action="store_false", dest="verbose", default=False,
                          help="Don't say what we're doing on stdout")
                          
        parser.add_option_group(g)

        if self.opts:
            g = OptionGroup(parser, "Special Options", "Special options")
            for o in self.opts:
                g.add_option(o)
            parser.add_option_group(g)
            
        parser.set_defaults(verbose=True, recursive=False, precision = 6, offset = False)

        self.parser = parser
        
    def __init__(self, arguments, options=None):
        self.connection = None
        self.output = None
        self.sql = None
        
        self.opts = options
        self.construct_parser()
        self.options, self.args = self.parser.parse_args(args=arguments)
        
        if self.args:
            self.options.connection = self.args[0]
            
        if not self.options.output:
            try:
                self.options.output = self.args[1]
            except IndexError:
                self.options.output = 'output.las'
                
        if not self.options.sql:
            try:
                self.options.sql = self.args[2]
            except IndexError:
                raise self.parser.error("No SQL was provided to select the point cloud!")           

        if self.options.output:
            self.options.output = os.path.abspath(self.options.output)
            if os.path.isdir(self.options.output):
                raise self.parser.error("Output '%s' is a directory, not a file " % self.options.output)
            
            if os.path.exists(self.options.output):
                if not self.options.overwrite:
                    raise self.parser.error("Output file '%s' exists, but you have not selected the --overwrite option" % self.options.output)
        else:
            raise self.parser.error("No output was specified")

        try:
            self.options.precision = int(self.options.precision)
            if not self.options.precision:
                raise self.parser.error("Precision cannot be 0")
        except:
            raise self.parser.error("Precision was not an number")
            
        self.min = point.Point()
        self.max = point.Point()
        self.count = 0
        self.first_point = True
    def print_options(self):
        print self.options
        
    def connect(self):
        self.con = oci.Connection(self.options.connection)

    def get_clouds(self):
        self.cur = self.con.cursor()
        self.cur.execute(self.options.sql)
        clouds = []
        res = self.cur.fetchall()
        for row in res:
            for column in row:
                try:
                    column.BASE_TABLE_COL
                    clouds.append(column)
                except AttributeError:
                    # This column isn't a cloud
                    pass
        
        return clouds
        
    def get_points(self, cloud):
        points = []
        
        # fetch the point data from the block table
        self.cur.execute('SELECT NUM_POINTS, POINTS FROM %s'% cloud.BLK_TABLE)
        blocks = self.cur.fetchall()
        
        for block in blocks:
            num, blk = block
            data = blk.read()
            for i in xrange(num):
                rng = ptsize*i,ptsize*(i+1)
                d = struct.unpack(format,data[ptsize*i:ptsize*(i+1)])
                x, y, z, blk_id, pt_id = d
                p = point.Point()
                p.x = x; p.y = y; p.z = z
                points.append(p)
        return points
    
    def summarize_files(self):
        pass
    
    def open_output(self):
        h = header.Header()
        
        prec = 10**-(self.options.precision-1)
        h.scale = [prec, prec, prec]
        output = lasfile.File(self.options.output,mode='w',header=h)
        return output
    
    def write_points(self, points):
        
        for p in points:
            
            if self.first_point:
                self.min.x = p.x
                self.min.y = p.y
                self.max.x = p.x
                self.max.y = p.y
                self.first_point = False
            # cumulate min/max for the header
            self.min.x = min(self.min.x, p.x)
            self.max.x = max(self.max.x, p.x)
            
            self.min.y = min(self.min.y, p.y)
            self.max.y = max(self.max.y, p.y)
            
            self.min.z = min(self.min.z, p.z)
            self.max.z = max(self.max.z, p.z)
            
            self.count += 1
            
            self.output.write(p)
    
    def rewrite_header(self):
        h = self.output.header
        self.output.close()
        h.min = [self.min.x, self.min.y, self.min.z]
        h.max = [self.max.x, self.max.y, self.max.z]

        rc = h.point_return_count
        rc[0] = self.count
        h.point_return_count = rc
        
        import pdb;pdb.set_trace()
                
        self.output = lasfile.File(self.options.output, mode='w+', header=h)
        self.output.close()
    def process(self):
        self.print_options()
        self.connect()
        
        self.output = self.open_output()
        clouds = self.get_clouds()
        
        print clouds
        for cloud in clouds:
            pts = self.get_points(cloud)
            print len(pts)
            self.write_points(pts)
        
        self.rewrite_header()
        

def main():
    import optparse

    options = []
#     o = optparse.make_option("-r", "--remainder", dest="remainder",
#                          type="choice",default='end', 
#                           help="""what to do with the remainder -- place it at the beginning, 
# place it at the end, or evenly distribute it across the segment""",
#                           choices=['end','begin','uniform'])
#     options.append(o)
    
    d = Translator(sys.argv[1:], options=options)
    d.process()

if __name__=='__main__':
    main()