#!/usr/bin/env python

from liblas import file as lasfile
from liblas import header

import os, sys

class Summary(object):

    def construct_parser(self):
        from optparse import OptionParser, OptionGroup
        usage = "usage: %prog [options] arg"
        parser = OptionParser(usage)
        g = OptionGroup(parser, "Base options", "Basic Translation Options")
        g.add_option("-i", "--input", dest="input",
                          help="Input directory", metavar="INPUT")
        g.add_option("-o", "--output", dest='output',
                          help="ReSTructured text file to write for output", metavar="OUTPUT")
        g.add_option("-r", "--recursive",
                          action="store_false", dest="recursive", 
                          help="recurse down the directory")

        g.add_option("-u", "--url", dest='url',
                          help="URL to base for links to files", metavar="DRIVER")

        g.add_option("-q", "--quiet",
                          action="store_false", dest="verbose", default=False,
                          help="Say what we're doing on stdout")
                          
        parser.add_option_group(g)

        if self.opts:
            g = OptionGroup(parser, "Special Options", "Special options")
            for o in self.opts:
                g.add_option(o)
            parser.add_option_group(g)

        parser.set_defaults(verbose=True, recursive=False)

        self.parser = parser
        
    def __init__(self, arguments, options=None):
        self.input = None
        self.output = None
        
        self.opts = options
        self.construct_parser()
        self.options, self.args = self.parser.parse_args(args=arguments)
    
    def process(self):
        return None

def main():
    import optparse

    options = []
#     o = optparse.make_option("-r", "--remainder", dest="remainder",
#                          type="choice",default='end', 
#                           help="""what to do with the remainder -- place it at the beginning, 
# place it at the end, or evenly distribute it across the segment""",
#                           choices=['end','begin','uniform'])
#     options.append(o)

    d = Summary(sys.argv[1:], options=options)
    d.process()
