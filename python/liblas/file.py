"""
/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Python LASFile implementation
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
import header as lasheader
import point

import os
import types

files = {'append':[], 'write':[],'read':[]}
import sys

class File(object):
    def __init__(self, filename, header=None, mode='r'):
        self.filename = os.path.abspath(filename)
        self._header = header
        self.handle = None
        self._mode = mode
        
        # print self.filename
        # print sys.stderr.write('%s %s %s'%(files, self.filename, mode))
        # sys.stderr.flush()
        #check in the registry if we already have the file open
        if mode=='r':
            for f in files['write'] + files['append']:
                if f == self.filename:
                    raise core.LASException("File %s is already open for write.  Close the file or delete the reference to it" % filename)
        else:
            for f in files['read']:
                if f == self.filename:
                    raise core.LASException("File %s is already open for read. Close the file or delete the reference to it" % filename)
        self.open()
        
    def open(self):
        if self._mode == 'r' or self._mode =='rb':
            self.handle = core.las.LASReader_Create(self.filename)
            self.mode = 0
            self._header = lasheader.Header(handle = core.las.LASReader_GetHeader(self.handle))
            files['read'].append(self.filename)
        if self._mode == 'w' and '+' not in self._mode:
            if not self._header:
                self._header = lasheader.Header(handle = core.las.LASHeader_Create())
            self.handle = core.las.LASWriter_Create(self.filename, self._header.handle, 1)
            self.mode = 1
            files['write'].append(self.filename)
        if '+' in self._mode and 'r' not in self._mode:
            if not self._header:
                reader = core.las.LASReader_Create(self.filename)
                self._header = lasheader.Header(handle = core.las.LASReader_GetHeader(reader))
                core.las.LASReader_Destroy(reader)
            self.handle = core.las.LASWriter_Create(self.filename, self._header.handle, 2)
            self.mode = 2
            files['append'].append(self.filename)
    def __del__(self):
        if not self.handle or not core: return
        self.close()

    def close(self):
        if self.mode == 0 :
            core.las.LASReader_Destroy(self.handle)
            files['read'].remove(self.filename)
        else:
            try:
                files['append'].remove(self.filename)
            except:
                files['write'].remove(self.filename)
            core.las.LASWriter_Destroy(self.handle)
        
        self._header = None
        self.handle = None
    
    def set_srs(self, value):
        if self.mode == 0 :
            return core.las.LASReader_SetSRS(self.handle, value.handle)
        else:
            return core.las.LASWriter_SetSRS(self.handle, value.handle)
            
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

    def read(self, location):
        if self.mode == 0:
            return point.Point(handle=core.las.LASReader_GetPointAt(self.handle, location), owned= False, copy=True)

    def __iter__(self):
        if self.mode == 0:
            self.at_end = False
            p = core.las.LASReader_GetNextPoint(self.handle)
            while p and not self.at_end:
                yield point.Point(handle=p, owned= False, copy=True)
                p = core.las.LASReader_GetNextPoint(self.handle)
                if not p:
                    self.at_end = True
            else:
                self.close()
                self.open()
    
    def __len__(self):
        return self.header.point_records_count
    
    def write(self, pt):
        if not isinstance(pt, point.Point):
            raise core.LASException('cannot write %s, it must be of type liblas.point.Point' % pt)
        if self.mode == 1 or self.mode == 2:
            core.las.LASWriter_WritePoint(self.handle, pt.handle)

    