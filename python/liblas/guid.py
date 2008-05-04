import core

class GUID(object):
    def __init__(self, key=None, handle=None):
        self.handle = None
        if handle:
            self.handle = handle
        else:
            if not key:
                self.handle = core.las.LASGuid_Create()
            else:
                self.handle = core.las.LASGuid_CreateFromString(key)
    def __del__(self):
        if self.handle:
            core.las.LASGuid_Destroy(self.handle)
    
    def __str__(self):
        return core.las.LASGuid_AsString(self.handle)
    
    def __eq__(self, other):
        if isinstance(other, GUID):
            return bool(core.las.LASGuid_Equals(self.handle, other.handle))
        raise core.LASException("GUID can only be compared to other GUIDs, not %s" % type(other))
    
    def __repr__(self):
        return self.__str__()