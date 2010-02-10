"""
/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Python LASPointFormat implementation
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Howard Butler
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

class Format(object):
    def __init__(self, major=1, minor=0, time=False, color = False, size=0, handle=None):
        self.handle = None

        if handle:
            self.handle = handle
        else:
            self.handle = core.las.LASPointFormat_Create(major, minor, size, time, color)
            
    def __del__(self):
        if self.handle and core:
            core.las.LASPointFormat_Destroy(self.handle)
    
    def get_major(self):
        return core.las.LASPointFormat_GetVersionMajor(self.handle)
    
    def set_major(self, value):
        return core.las.LASPointFormat_SetVersionMajor(self.handle, int(value))
        
    major = property(get_major, set_major)
    
    def get_minor(self):
        return core.las.LASPointFormat_GetVersionMinor(self.handle)
    
    def set_minor(self, value):
        return core.las.LASPointFormat_SetVersionMinor(self.handle, int(value))
        
    minor = property(get_minor, set_minor)

    def get_size(self):
        return core.las.LASPointFormat_GetByteSize(self.handle)
    
    def set_size(self, value):
        return core.las.LASPointFormat_SetByteSize(self.handle, int(value))
        
    size = property(get_size, set_size)

    def get_color(self):
        return bool(core.las.LASPointFormat_HasColor(self.handle))
    
    def set_color(self, value):
        return core.las.LASPointFormat_SetColor(self.handle, bool(value))
        
    color = property(get_color, set_color)

    def get_time(self):
        return bool(core.las.LASPointFormat_HasTime(self.handle))
    
    def set_time(self, value):
        return core.las.LASPointFormat_SetTime(self.handle, bool(value))
        
    time = property(get_time, set_time)  
    
    