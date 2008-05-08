import core
import datetime
import guid

class Header(object):
    def __init__(self, owned=True, handle=None):
        if handle:
            self.handle = handle
        else:
            self.handle = core.las.LASHeader_Create()
        self.owned = owned
    def __del__(self):
        if self.owned:
            if self.handle and core:
                core.las.LASHeader_Destroy(self.handle)
    
    def get_filesignature(self):
        """Returns the file signature for the file.  It should always be LASF"""
        return core.las.LASHeader_GetFileSignature(self.handle)
    file_signature = property(get_filesignature)
    
    def get_filesourceid(self):
        return core.las.LASHeader_GetFileSourceId(self.handle)
    filesource_id = property(get_filesourceid)

    def get_projectid(self):
        """Returns the ProjectID/GUID for the file.  libLAS does not currently support setting this value from Python"""
        return core.las.LASHeader_GetProjectId(self.handle)
    project_id = property(get_projectid)
    
    def get_guid(self):
        """Returns the GUID for the file as a liblas.guid.GUID"""
        return guid.GUID(handle=core.las.LASHeader_GetGUID(self.handle))
    def set_guid(self, value):
        """Sets the GUID for the file.  It must be a liblas.guid.GUID"""
        return core.las.LASHeader_SetGUID(self.handle, value.handle)
    guid = property(get_guid, set_guid)
    
    def get_majorversion(self):
        """Returns the major version for the file.  Expect this value to always be 1"""
        return core.las.LASHeader_GetVersionMajor(self.handle)
    def set_majorversion(self, value):
        """Sets the major version for the file.  Only the value 1 is accepted at this time"""
        return core.las.LASHeader_SetVersionMajor(self.handle, value)
    major_version = property(get_majorversion, set_majorversion)

    def get_minorversion(self):
        """Returns the minor version of the file.  Expect this value to always be 0 or 1"""
        return core.las.LASHeader_GetVersionMinor(self.handle)
    def set_minorversion(self, value):
        """Sets the minor version of the file.  The value should be 0 for 1.0 LAS files and 1 for 1.1 LAS files"""
        return core.las.LASHeader_SetVersionMinor(self.handle, value)
    minor_version = property(get_minorversion, set_minorversion)

    def get_systemid(self):
        """Returns the system identifier specified in the file"""
        return core.las.LASHeader_GetSystemId(self.handle)
    def set_systemid(self, value):
        """Sets the system identifier.  The value is truncated to 31 characters"""
        return core.las.LASHeader_SetSystemId(self.handle, value[0:31])
    system_id = property(get_systemid, set_systemid)

    def get_softwareid(self):
        """Returns the software identifier specified in the file"""
        return core.las.LASHeader_GetSoftwareId(self.handle)
    def set_softwareid(self, value):
        """Sets the software identifier. 
        
        The value is truncated to 31 characters.

        Parameters
        ----------
        value : a string that will automatically be truncated to 31 characters
        
        Example
        -------
        >>> h.software_id
        'libLAS 1.0'
        >>> h.software_id = 'hobu'
        >>> h.software_id
        'hobu'
        >>> h.software_id = 'hobu'*9
        >>> h.software_id
        'hobuhobuhobuhobuhobuhobuhobuhob'        
        """
        return core.las.LASHeader_SetSoftwareId(self.handle, value[0:31])
    software_id = property(get_softwareid, set_softwareid)

    def get_date(self):
        """Return the header's date as a datetime.datetime.  If no date is set in the 
        header, None is returned.
        
        Note that dates in LAS headers are not transitive because the header
        only stores the year and the day number.
        """
        day = core.las.LASHeader_GetCreationDOY(self.handle)
        year = core.las.LASHeader_GetCreationYear(self.handle)
        
        if year == 0 and day == 0:
            return None
        
        return datetime.datetime(year,1,1)+datetime.timedelta(day)
    def set_date(self, value=datetime.datetime.now()):
        """Set the header's date from a datetime.datetime instance.

        Parameters
        ----------
        value : datetime.datetime instance or none to use the current time

        Example
        -------
        >>> t = datetime.datetime(2008,3,19)
        >>> h.date = t
        >>> h.date
        datetime.datetime(2008, 3, 19, 0, 0)
        """
        delta = value - datetime.datetime(value.year,1,1)
        core.las.LASHeader_SetCreationDOY(self.handle, delta.days)
        core.las.LASHeader_SetCreationYear(self.handle, value.year)
    date = property(get_date, set_date)
    
    def get_headersize(self):
        """Return the size of the header block of the LAS file in bytes.  
        
        Should not be needed in Python land
        """
        return core.las.LASHeader_GetHeaderSize(self.handle)
    header_size = property(get_headersize)
    
    def get_dataoffset(self):
        """Returns the location in bytes of where the data block of the LAS file starts"""
        return core.las.LASHeader_GetDataOffset(self.handle)
    data_offset = property(get_dataoffset)
    
    def get_recordscount(self):
        """Returns the number of user-defined header records in the header.  
        User-defined records are not really supported at this time in libLAS"""
        return core.las.LASHeader_GetRecordsCount(self.handle)
    records_count = property(get_recordscount)
    
    def get_dataformatid(self):
        """Returns the data format id.  
        
        A value of 1 means the format is 1.1 compatible and includes a time 
        value on the points.  A value of 0 means the format is 1.0 compatible.
        """
        return core.las.LASHeader_GetDataFormatId(self.handle)
    def set_dataformatid(self, value):
        """Sets the data format id for the file.  
        
        It can only be 1 (for 1.1 compatible) or 0 (for 1.0 compatible).
        """
        if value not in [1,0]:
            raise core.LASException("Format ID must be 1 or 0")
        return core.las.LASHeader_SetDataFormatId(self.handle, value)
    dataformat_id = property(get_dataformatid, set_dataformatid)
    
    def get_datarecordlength(self):
        return core.las.LASHeader_GetDataRecordLength(self.handle)
    data_record_length = property(get_datarecordlength)
    
    def get_pointrecordscount(self):
        """Returns the expected number of point records in the file.  
        
        Note that this value can be grossly out of sync with the 
        actual number of points in the file.  Do not depend on it for 
        anything significant (like picking iteration locations in the file :)
        """
        return core.las.LASHeader_GetPointRecordsCount(self.handle)
    def set_pointrecordscount(self, value):
        """Sets the number of point records expected in the file"""
        return core.las.LASHeader_SetPointRecordsCount(self.handle, value)
    point_records_count = property(get_pointrecordscount, set_pointrecordscount)
    
    def get_pointrecordsbyreturncount(self):
        """Gets the histogram of point records by return number for returns 0...8

        Example
        -------

        >>> h.point_return_count
        [0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L]
        
        """
        output = []
        for i in range(8):
            output.append(core.las.LASHeader_GetPointRecordsByReturnCount(self.handle, i))
        return output
    def set_pointrecordsbyreturncount(self, value):
        """Sets the histogram of point records by return number from a list of returns 0..8

        Example
        -------

        >>> h.point_return_count = [1341235L, 3412341222L, 0L, 0L, 4321L, 0L, 0L, 0L]
        >>> h.point_return_count 
        [1341235L, 3412341222L, 0L, 0L, 4321L, 0L, 0L, 0L]

        """
        for i in value[0:7]:
            core.las.LASHeader_SetPointRecordsByReturnCount(self.handle, value.index(i), i)
    point_return_count = property(get_pointrecordsbyreturncount, set_pointrecordsbyreturncount)
    
    def get_scale(self):
        """Gets the scale factors in [x, y, z] for the point data.  
        
        libLAS uses this header value when reading/writing raw point data to the file.  
        If you change it in the middle of writing data, expect the unexpected.
        
        Example
        -------
        
        >>> h.scale
        [0.01, 0.01, 0.01]
        """
        x = core.las.LASHeader_GetScaleX(self.handle)
        y = core.las.LASHeader_GetScaleY(self.handle)
        z = core.las.LASHeader_GetScaleZ(self.handle)
        return [x,y,z]
    def set_scale(self, value):
        """Sets the scale factors in [x, y, z] for the point data.

        Example
        -------
        
        >>> h.scale = [0.5, 0.5, 0.001]
        >>> h.scale
        [0.5, 0.5, 0.001]
        """
        return core.las.LASHeader_SetScale(self.handle, value[0], value[1], value[2])
    scale = property(get_scale, set_scale)

    def get_offset(self):
        """Gets the offset factors in [x, y, z] for the point data.  
        
        libLAS uses this header value when reading/writing raw point data to the file.  
        If you change it in the middle of writing data, expect the unexpected.
        
        Example
        -------
        
        >>> h.offset
        [0.0, 0.0, 0.0]
        """
        x = core.las.LASHeader_GetOffsetX(self.handle)
        y = core.las.LASHeader_GetOffsetY(self.handle)
        z = core.las.LASHeader_GetOffsetZ(self.handle)
        return [x,y,z]
    def set_offset(self, value):
        """Sets the offset factors in [x, y, z] for the point data.

        Example
        -------
        
        >>> h.offset = [32, 32, 256]
        >>> h.offset
        [32.0, 32.0, 256.0]
        """
        return core.las.LASHeader_SetOffset(self.handle, value[0], value[1], value[2])
    offset = property(get_offset, set_offset)

    def get_min(self):
        """Gets the minimum values of [x, y, z] for the data.

        Example
        -------
        
        >>> h.min
        [0.0, 0.0, 0.0]
        """
        x = core.las.LASHeader_GetMinX(self.handle)
        y = core.las.LASHeader_GetMinY(self.handle)
        z = core.las.LASHeader_GetMinZ(self.handle)
        return [x,y,z]
    def set_min(self, value):
        """Sets the minimum values of [x, y, z] for the data.

        Example
        -------
        
        >>> h.min = [33452344.2333, 523442.344, -90.993]
        >>> h.min
        [33452344.2333, 523442.34399999998, -90.992999999999995]
        """
        return core.las.LASHeader_SetMin(self.handle, value[0], value[1], value[2])
    min = property(get_min, set_min)

    def get_max(self):
        x = core.las.LASHeader_GetMaxX(self.handle)
        y = core.las.LASHeader_GetMaxY(self.handle)
        z = core.las.LASHeader_GetMaxZ(self.handle)
        return [x,y,z]
    def set_max(self, value):
        """Sets the maximum values of [x, y, z] for the data.

        Example
        -------
        
        >>> h.max = [33452344.2333, 523442.344, -90.993]
        >>> h.max
        [33452344.2333, 523442.34399999998, -90.992999999999995]
        """
        return core.las.LASHeader_SetMax(self.handle, value[0], value[1], value[2])
    max = property(get_max, set_max)

