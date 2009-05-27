"""
/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Python VLR implementation
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
import ctypes

class VLR(object):
    def __init__(self, owned=True, handle=None):
        if handle:
            self.handle = handle
        else:
            self.handle = core.las.LASVLR_Create()
        self.owned = owned
    def __del__(self):
        if self.owned:
            if self.handle and core:
                core.las.LASVLR_Destroy(self.handle)
    
    def get_userid(self):
        return core.las.LASVLR_GetUserId(self.handle)
    def set_userid(self, value):
        return core.las.LASVLR_SetUserId(self.handle, value)
    userid = property(get_userid, set_userid)

    def get_description(self):
        return core.las.LASVLR_GetDescription(self.handle)
    def set_description(self, value):
        return core.las.LASVLR_SetDescription(self.handle, value)
    description = property(get_description, set_description)

    def get_recordlength(self):
        return core.las.LASVLR_GetRecordLength(self.handle)
    def set_recordlength(self, value):
        return core.las.LASVLR_SetRecordLength(self.handle, value)
    recordlength = property(get_recordlength, set_recordlength)

    def get_recordid(self):
        return core.las.LASVLR_GetRecordId(self.handle)
    def set_recordid(self, value):
        return core.las.LASVLR_SetRecordId(self.handle, value)
    recordid = property(get_recordid, set_recordid)

    def get_reserved(self):
        return core.las.LASVLR_GetReserved(self.handle)
    def set_reserved(self, value):
        return core.las.LASVLR_SetReserved(self.handle, value)
    reserved = property(get_reserved, set_reserved)
    
    def get_data(self):
        length = self.recordlength
        data = (ctypes.c_ubyte * length)()
        core.las.LASVLR_GetData(self.handle, data)
        return data

    def set_data(self, data):
        pdata = ctypes.cast(data, ctypes.POINTER(ctypes.c_ubyte))
        core.las.LASVLR_SetData(self.handle, pdata, self.recordlength)
    data = property(get_data, set_data)

