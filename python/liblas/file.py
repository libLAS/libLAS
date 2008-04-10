import core
import header as lasheader
import point

class File(object):
    def __init__(self, filename, header=None, mode='r'):
        self.filename = filename
        self._header = None
        self.handle = None
        
        if mode == 'r' or mode =='rb':
            self.handle = core.las.LASReader_Create(self.filename)
            self.mode = 0
            self._header = lasheader.Header(handle = core.las.LASReader_GetHeader(self.handle))
        if mode == 'w' and '+' not in mode:
            if not header:
                self._header = lasheader.Header(handle = core.las.LASHeader_Create())
            else:
                self._header = header
            self.handle = core.las.LASWriter_Create(self.filename, self._header.handle, 1)
            self.mode = 1
        if '+' in mode and 'r' not in mode:
            if not header:
                reader = core.las.LASReader_Create(self.filename)
                self._header = lasheader.Header(handle = core.las.LASReader_GetHeader(reader))
                core.las.LASReader_Destroy(reader)
            self.handle = core.las.LASWriter_Create(self.filename, header.handle, 2)
            self.mode = 2
    def __del__(self):
        if not self.handle: return
        if self.mode == 0 :
            core.las.LASReader_Destroy(self.handle)
        else:
            core.las.LASWriter_Destroy(self.handle)
    def get_header(self):
        """Returns the liblas.header.Header for the file"""
        return self._header
    def set_header(self, header):
        
        # append mode
        if mode == 2:
            core.las.LASWriter_Destroy(self.handle)
            self.handle = core.las.LASWriter_Create(self.handle, header, 2)
            self._header = header
            return True
        raise core.LASException("The header can only be set after file creation for files in append mode")
    header = property(get_header)

    def get_point(self, location):
        if self.mode == 0:
            return point.Point(handle=core.las.LASReader_GetPointAt(self.handle, location), owned= False, copy=True)

    def __iter__(self):
        if self.mode == 0:
            p = core.las.LASReader_GetNextPoint(self.handle)
            while p:
                yield point.Point(handle=p, owned= False, copy=True)
                p = core.las.LASReader_GetNextPoint(self.handle)
    
    def write_point(self, pt):
        if not isinstance(pt, point.Point):
            raise core.LASException('cannot write %s, it must be of type liblas.core.point.Point' % pt)
        if self.mode == 1 or self.mode == 2:
            core.las.LASWriter_WritePoint(self.handle, pt.handle)

    