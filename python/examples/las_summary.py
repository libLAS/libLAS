#!/usr/bin/env python

from liblas import file as lasfile
from liblas import header

import glob

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
                          action="store_true", dest="recursive", 
                          help="recurse down the directory")

        g.add_option("-u", "--url", dest='url',
                          help="URL to base for links to files", metavar="URL")

        g.add_option("-q", "--quiet",
                          action="store_false", dest="verbose", default=False,
                          help="Don't say what we're doing on stdout")
                          
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
        
        if self.args:
            self.options.input = self.args[0]
        if not self.options.output:
            try:
                self.options.output = self.args[1]
            except IndexError:
                self.options.output = 'output.txt'
        print self.options, self.args
            
    
    def list_files(self):
        def get_files(path):
            output = []
            fn = path+"/*.LAS"
            output = glob.glob(fn)
            fn = path+"/*.las"
            output.extend(glob.glob(fn))
            return output

        if self.options.input:
            self.options.input = os.path.abspath(self.options.input)
            if not os.path.isdir(self.options.input):
                raise self.parser.error("Inputted path '%s' was not a directory" % self.options.input)
        else:
            raise self.parser.error("No input directory was specified")
        
        if self.options.recursive:
            directories = {}
            for root, dirs, files in os.walk(self.options.input):
                directories[root] = root
                for d in dirs:
                    directories[os.path.join(root, d)] = os.path.join(root, d)
            
            self.files = []
            for d in directories:
                print d
                files = get_files(d)
                print files
                if files:
                    self.files.extend(files)

        else:
            get_files(self.options.input)
        
        print self.files
            
    def process(self):
        self.list_files()
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

if __name__=='__main__':
    main()