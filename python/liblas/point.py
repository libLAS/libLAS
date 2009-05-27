"""
/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Python LASPoint implementation
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2008, Howard Butler
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/
 """
import core
import datetime
import time
import math
import color

class Point(object):
    def __init__(self, owned=True, handle=None, copy=False):
        if handle:
            if copy:
                self.handle = core.las.LASPoint_Copy(handle)
            else:
                self.handle = handle
        else:
            self.handle = core.las.LASPoint_Create()
        if not copy:
            self.owned = owned
        else:
            self.owned = True
    def __del__(self):
        if self.owned:
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

        Parameters
        ----------
        value : datetime.datetime instance or none to use the current time

        Example
        -------
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
        self.x = self.x + header.scale[0] + header.offset[0]
        self.y = self.y + header.scale[1] + header.offset[1]
        self.z = self.z + header.scale[2] + header.offset[2]
