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
p.color.red=255
p.color.green=12
p.color.blue=234
p.time = datetime.datetime.now()

print p.time

def write_file(version, format):
    h = header.Header()
    h.date = datetime.datetime.now()
    h.dataformat_id = format
    h.major_version = 1
    h.minor_version = version
    f = file.File('test_1.%d_pointformat%d.las'%(version,format), mode='w', header=h)
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