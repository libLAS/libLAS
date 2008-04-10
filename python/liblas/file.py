import core
import header as lasheader
import point

files = []

class File(object):
    def __init__(self, filename, header=None, mode='r'):
        self.filename = filename
        self._header = None
        self.handle = None
        
        #check in the registry if we already have the file open
        for f in files:
            if f.filename == filename:
                if f.mode == mode:
                    return f
                else:
                    raise core.LASException("File %s is already open.  Close the file or delete the reference to it", filename)
                    # # close up the file
                    # files.remove(f)
                    # f.__del__()
                    # f.handle = None
                    
        if mode == 'r' or mode =='rb':
            self.handle = core.las.LASReader_Create(self.filename)
            self.mode = 0
            self._header = lasheader.Header(handle = core.las.LASReader_GetHeader(self.handle))
            files.append(self)
        if mode == 'w' and '+' not in mode:
            if not header:
                self._header = lasheader.Header(handle = core.las.LASHeader_Create())
            else:
                self._header = header
            self.handle = core.las.LASWriter_Create(self.filename, self._header.handle, 1)
            self.mode = 1
            files.append(self)
        if '+' in mode and 'r' not in mode:
            if not header:
                reader = core.las.LASReader_Create(self.filename)
                self._header = lasheader.Header(handle = core.las.LASReader_GetHeader(reader))
                core.las.LASReader_Destroy(reader)
            else:
                self._header = header
            self.handle = core.las.LASWriter_Create(self.filename, self._header.handle, 2)
            self.mode = 2
            files.append(self)
    def __del__(self):
        if not self.handle: return
        self.close()

    def close(self):
        if self.mode == 0 :
            core.las.LASReader_Destroy(self.handle)
        else:
            core.las.LASWriter_Destroy(self.handle)
        self._header = None
        self.handle = None
        
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

    