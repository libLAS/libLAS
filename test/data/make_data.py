#!/usr/bin/env python

from liblas import file
from liblas import header
from liblas import point
from liblas import color
import datetime

h = header.Header()
h.date = datetime.datetime.now()
h.dataformat_id = 1
h.proj4 = '+proj=utm +zone=17 +ellps=WGS84 +datum=WGS84 +units=m +no_defs '

p = point.Point()
p.flightline_edge = 0
p.return_number = 1
p.classification = 0
p.scan_angle = -13
p.x = 42.0000
p.y = -93.00
p.z = 16.0
c = color.Color()

c.red=255
c.green=12
c.blue=234
p.color = c
p.time = datetime.datetime.now()

print p.time

def write_file(version, format):
    h = header.Header()
    h.date = datetime.datetime.now()
    h.dataformat_id = format
    h.major_version = 1
    h.minor_version = version
    f = file.File('1.%d_%d.las'%(version,format), mode='w', header=h)
    f.write(p)
    f.close()

write_file(0,0)
write_file(0,1)
write_file(1,0)
write_file(1,1)
write_file(2,0)
write_file(2,1)
write_file(2,2)
write_file(2,3)