"""
 """
import core
import datetime
import time
import math
import color
import ctypes

class Point(object):
    def __init__(self, owned=True, handle=None, copy=False):
        if handle:
            if copy:
                self.handle = core.las.LASPoint_Copy(handle)
                self._owned = True
            else:
                self.handle = handle
                self._owned = False
        else:
            self.handle = core.las.LASPoint_Create()
            self._owned = True
    def __del__(self):
        if self._owned:
            if self.handle and core:
                core.las.LASPoint_Destroy(self.handle)
    
    def get_x(self):
        """Returns the X coordinate of the LAS point. 
        
        You are expected to properly de-scale the point according to the offset
        and the X scale if it is a free-standing point.  When points are 
        written to a LAS file, they will be scaled according to the header 
        parameters
        """
        return core.las.LASPoint_GetX(self.handle)
    def set_x(self, value):
        """Sets the X coordinate of the LAS point to a floating point value."""
        return core.las.LASPoint_SetX(self.handle, value)
    x = property(get_x, set_x)

    def get_y(self):
        """Returns the Y coordinate of the LAS point. 
        
        You are expected to properly de-scale the point according to the offset
        and the Y scale if it is a free-standing point.  When points are 
        written to a LAS file, they will be scaled according to the header 
        parameters
        """
        return core.las.LASPoint_GetY(self.handle)
    def set_y(self, value):
        """Sets the Y coordinate of the LAS point to a floating point value."""
        return core.las.LASPoint_SetY(self.handle, value)
    y = property(get_y, set_y)

    def get_z(self):
        """Returns the Z coordinate of the LAS point. 
        
        You are expected to properly de-scale the point according to the offset
        and the Z scale if it is a free-standing point.  When points are 
        written to a LAS file, they will be scaled according to the header 
        parameters
        """
        return core.las.LASPoint_GetZ(self.handle)
    def set_z(self, value):
        """Sets the Z coordinate of the LAS point to a floating point value."""
        return core.las.LASPoint_SetZ(self.handle, value)
    z = property(get_z, set_z)
    
    def get_return_number(self):
        """Returns the return number of the point"""
        return core.las.LASPoint_GetReturnNumber(self.handle)
    def set_return_number(self, value):
        """Sets the return number of the point to an integer"""
        core.las.LASPoint_SetReturnNumber(self.handle, value)
    return_number = property(get_return_number, set_return_number)
    
    def get_number_of_returns(self):
        """Returns the number of returns for the point"""
        return core.las.LASPoint_GetNumberOfReturns(self.handle)
    def set_number_of_returns(self, value):
        """Sets the number of returns for the point"""
        core.las.LASPoint_SetNumberOfReturns(self.handle, value)
    number_of_returns = property(get_number_of_returns, set_number_of_returns)
    
    def get_scan_direction(self):
        """Returns the scan direction angle for the point"""
        return core.las.LASPoint_GetScanDirection(self.handle)
    def set_scan_direction(self, value):
        """Sets the scan direction angle as an integer for the point"""
        core.las.LASPoint_SetScanDirection(self.handle, value)
    scan_direction = property(get_scan_direction, set_scan_direction)
    
    def get_flightline_edge(self):
        """Denotes whether the point is a flight line edge"""
        return core.las.LASPoint_GetFlightLineEdge(self.handle)
    def set_flightline_edge(self, value):
        """Sets the flightline edge as an integer for the point.  Must be 0 (not an edge) or 1 (an edge)"""
        core.las.LASPoint_SetFlightLineEdge(self.handle, value)
    flightline_edge = property(get_flightline_edge, set_flightline_edge)
    
    def get_scan_flags(self):
        """Returns the raw scan flags for the point.  
        See the LAS 1.0 or 1.1 specification for information how to interpret
        """
        return core.las.LASPoint_GetScanFlags(self.handle)
    def set_scan_flags(self, value):
        """Sets the raw scan flags for the point.
        See the LAS 1.0 or 1.1 specification for information how to interpret or 
        use the convenience functions like flightline_edge, scan_direction, etc.
        """
        core.las.LASPoint_SetScanFlags(self.handle, value)
    scan_flags = property(get_scan_flags, set_scan_flags)
    
    def get_classification(self):
        return core.las.LASPoint_GetClassification(self.handle)
    def set_classification(self, value):
        core.las.LASPoint_SetClassification(self.handle, value)
    classification = property(get_classification, set_classification)
    
    def get_scan_angle_rank(self):
        """Returns the scan angle rank of the point.  It will be between -90 and 90"""
        return core.las.LASPoint_GetScanAngleRank(self.handle)
    def set_scan_angle_rank(self, value):
        """Sets the scan angle rank of the point.  It must be between -90 and 90"""
        core.las.LASPoint_SetScanAngleRank(self.handle, value)
    scan_angle = property(get_scan_angle_rank, set_scan_angle_rank)

    def get_user_data(self):
        return core.las.LASPoint_GetUserData(self.handle)
    def set_user_data(self, value):
        core.las.LASPoint_SetUserData(self.handle, value)
    user_data = property(get_user_data, set_user_data)

    def get_intensity(self):
        return core.las.LASPoint_GetIntensity(self.handle)
    def set_intensity(self, value):
        core.las.LASPoint_SetIntensity(self.handle, value)
    intensity = property(get_intensity, set_intensity)

    def get_time(self):
        t = core.las.LASPoint_GetTime(self.handle)
        floor = math.floor(t)
        ms = float(t) - floor
        
        # clip to 999999
        ms = int(round(ms* 1000000 ))
        if ms > 999999:
            ms = 999999

        lt = time.gmtime(t)
        value = datetime.datetime(lt[0],lt[1],lt[2],lt[3],lt[4],lt[5],ms)
        return value
        
    def set_time(self, value):
        """
        Sets the time of the point from a datetime value
        value : datetime.datetime instance or none to use the current time

        
        >>> td = datetime.timedelta(hours=6) # my timezone is -6
        >>> t = datetime.datetime(2008,3,19) -td
        >>> p.time = t
        >>> p.time
        datetime.datetime(2008, 3, 19, 0, 0)
        """
        t = time.mktime(value.timetuple())
        
        ms = value.microsecond
        t = float(t) + ms * 0.000001
        core.las.LASPoint_SetTime(self.handle,t)
    time = property(get_time, set_time)

    def get_color(self):
        return color.Color(handle=core.las.LASPoint_GetColor(self.handle))
    
    def set_color(self, value):
        return core.las.LASPoint_SetColor(self.handle, value.handle)
    color = property(get_color, set_color)

    def descale(self, header):
        self.x = (self.x - header.offset[0]) / header.scale[0]
        self.y = (self.y - header.offset[1]) / header.scale[1]
        self.z = (self.z - header.offset[2]) / header.scale[2]
    
    def scale(self, header):
        self.x = (self.x * header.scale[0]) + header.offset[0]
        self.y = (self.y * header.scale[1]) + header.offset[1]
        self.z = (self.z * header.scale[2]) + header.offset[2]
    
    def get_data(self):
        l = ctypes.pointer(ctypes.c_int())
        d = ctypes.pointer(ctypes.c_ubyte())
        core.las.LASPoint_GetExtraData(self.handle, ctypes.byref(d), l)
        
        d2 = ctypes.cast(d, ctypes.POINTER(ctypes.c_ubyte * l.contents.value))
        s = (ctypes.c_ubyte * l.contents.value)()
        for i in range(l.contents.value):
            s[i] = d2.contents[i]
        p_d = ctypes.cast(d, ctypes.POINTER(ctypes.c_char_p))
        # core.las.LASString_Free(p_d)

        return s

    def set_data(self, data):
                                                              
        d =  ctypes.cast(data, ctypes.POINTER(ctypes.c_ubyte))

        core.las.LASPoint_SetExtraData(self.handle, d, len(data))
        
    data = property(get_data, set_data)