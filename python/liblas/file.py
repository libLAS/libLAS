import core
import header

class File(object):
    def __init__(self, filename, header=None, mode='r'):
        self.filename = filename
        self._header = None
        
        if mode == 'r' or mode =='rb':
            self.handle = core.las.LASReader_Create(self.filename)
            self.mode = 0
            self._header = header.Header(handle =core.las.LASReader_GetHeader(self.handle))
        if mode == 'w' and '+' not in mode:
            if not header:
                header = header.Header(handle =core.las.LASHeader_Create())
            else:
                self._header = header
            self._handle = core.las.LASWriter_Create(self.filename, header, 1)
            self.mode = 1
        if '+' in mode and 'r' not in mode:
            if not header:
                reader = core.las.LASReader_Create(self.filename)
                self._header = header.Header(handle =core.las.LASReader_GetHeader(reader))
                core.las.LASReader_Destroy(reader)
            self.handle = core.las.LASWriter_Create(self.filename, header, 2)
            self.mode = 2
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