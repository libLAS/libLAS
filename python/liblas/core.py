import atexit, os, re, sys
import ctypes
from ctypes.util import find_library

from ctypes import PyDLL

class LASException(Exception):
    "libLAS exception, indicates a libLAS-related error."
    pass


def check_return(result, func, cargs):
    "Error checking for LASError calls"
    if result != 0:
        msg = 'LASError in "%s": %s' % (func.__name__, las.LASError_GetLastErrorMsg() )
        las.LASError_Reset()
        raise LASException(msg)
    return True

def check_void(result, func, cargs):
    "Error checking for void* returns"
    if not bool(result):
        msg = 'LASError in "%s": %s' % (func.__name__, las.LASError_GetLastErrorMsg() )
        las.LASError_Reset()
        raise LASException(msg)
    return result

def check_void_done(result, func, cargs):
    "Error checking for void* returns that might be empty with no error"
    if not bool(result):
        if las.LASError_GetErrorCount():
            msg = 'LASError in "%s": %s' % (func.__name__, las.LASError_GetLastErrorMsg() )
            las.LASError_Reset()
            raise LASException(msg)
    return result
def check_value(result, func, cargs):
    "Error checking proper value returns"
    count = las.LASError_GetErrorCount()
    if count != 0:
        msg = 'LASError in "%s": %s' % (func.__name__, las.LASError_GetLastErrorMsg() )
        las.LASError_Reset()
        raise LASException(msg)
    return result

def check_value_free(result, func, cargs):
    "Error checking proper value returns"
    count = las.LASError_GetErrorCount()
    if count != 0:
        msg = 'LASError in "%s": %s' % (func.__name__, las.LASError_GetLastErrorMsg() )
        las.LASError_Reset()
        raise LASException(msg)
    retval = ctypes.string_at(result)[:]
    free(result)
    return retval

try:
    from numpy import array, ndarray
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False

if os.name == 'nt':
    # stolen from Shapely
    # http://trac.gispython.org/projects/PCL/browser/Shapely/trunk/shapely/geos.py
    lib_name = 'liblas0.9.6.dll'
    try:
        local_dlls = os.path.abspath(os.__file__ + "../../../DLLs")
        original_path = os.environ['PATH']
        os.environ['PATH'] = "%s;%s" % (local_dlls, original_path)
        las = ctypes.CDLL(lib_name)
        def free(m):
            try:
                free = ctypes.cdll.msvcrt.free(m)
            except WindowsError:
                pass
    except (ImportError, WindowsError):
        raise


elif os.name == 'posix':
    platform = os.uname()[0]
    if platform == 'Darwin':
        lib_name = 'liblas.dylib'
        free = ctypes.CDLL(find_library('libc')).free
    else:
        lib_name = 'liblas.so'
        free = ctypes.CDLL(find_library('libc.so.6')).free
    las = ctypes.CDLL(lib_name)
else:
    raise LASException('Unsupported OS "%s"' % os.name)



def get_version():
    func = las.LAS_GetVersion
    size = ctypes.c_int()
    def errcheck(result, func, argtuple):
        retval = ctypes.string_at(result, size.value)[:]
        free(result)
        return result
    func.errcheck = errcheck
    return func()

#version = las.LAS_GetVersion()
version = get_version()

las.LASError_GetLastErrorNum.restype = ctypes.c_int

las.LASError_GetLastErrorMsg.restype = ctypes.c_char_p

las.LASError_GetLastErrorMethod.restype = ctypes.c_char_p

las.LASError_GetErrorCount.restype=ctypes.c_int

las.LASReader_Create.argtypes = [ctypes.c_char_p]
las.LASReader_Create.restype = ctypes.c_void_p
las.LASReader_Create.errcheck = check_void

las.LASReader_GetNextPoint.restype=ctypes.c_void_p
las.LASReader_GetNextPoint.argtypes = [ctypes.c_void_p]
las.LASReader_GetNextPoint.errcheck = check_void_done

las.LASReader_GetPointAt.restype = ctypes.c_void_p
las.LASReader_GetPointAt.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
las.LASReader_GetPointAt.errcheck = check_void_done

las.LASPoint_GetX.restype = ctypes.c_double
las.LASPoint_GetX.argtypes = [ctypes.c_void_p]
las.LASPoint_GetX.errcheck = check_value
las.LASPoint_SetX.restype = ctypes.c_int
las.LASPoint_SetX.argtypes = [ctypes.c_void_p, ctypes.c_double]
las.LASPoint_SetX.errcheck = check_return

las.LASPoint_GetY.restype = ctypes.c_double
las.LASPoint_GetY.argtypes = [ctypes.c_void_p]
las.LASPoint_GetY.errcheck = check_value
las.LASPoint_SetY.restype = ctypes.c_int
las.LASPoint_SetY.argtypes = [ctypes.c_void_p, ctypes.c_double]
las.LASPoint_SetY.errcheck = check_return

las.LASPoint_GetZ.restype = ctypes.c_double
las.LASPoint_GetZ.argtypes = [ctypes.c_void_p]
las.LASPoint_GetZ.errcheck = check_value
las.LASPoint_SetZ.restype = ctypes.c_int
las.LASPoint_SetZ.argtypes = [ctypes.c_void_p, ctypes.c_double]
las.LASPoint_SetZ.errcheck = check_return

las.LASPoint_GetIntensity.restype = ctypes.c_short
las.LASPoint_GetIntensity.argtypes = [ctypes.c_void_p]
las.LASPoint_GetIntensity.errcheck = check_value
las.LASPoint_SetIntensity.restype = ctypes.c_int
las.LASPoint_SetIntensity.argtypes = [ctypes.c_void_p, ctypes.c_short]
las.LASPoint_SetIntensity.errcheck = check_return

las.LASPoint_GetReturnNumber.restype = ctypes.c_ushort
las.LASPoint_GetReturnNumber.argtypes = [ctypes.c_void_p]
las.LASPoint_GetReturnNumber.errcheck = check_value
las.LASPoint_SetReturnNumber.restype = ctypes.c_int
las.LASPoint_SetReturnNumber.argtypes = [ctypes.c_void_p, ctypes.c_ushort]
las.LASPoint_SetReturnNumber.errcheck = check_return

las.LASPoint_GetNumberOfReturns.restype = ctypes.c_ushort
las.LASPoint_GetNumberOfReturns.argtypes = [ctypes.c_void_p]
las.LASPoint_GetNumberOfReturns.errcheck = check_value
las.LASPoint_SetNumberOfReturns.restype = ctypes.c_int
las.LASPoint_SetNumberOfReturns.argtypes = [ctypes.c_void_p, ctypes.c_ushort]
las.LASPoint_SetNumberOfReturns.errcheck = check_return

las.LASPoint_GetScanDirection.restype = ctypes.c_ushort
las.LASPoint_GetScanDirection.argtypes = [ctypes.c_void_p]
las.LASPoint_GetScanDirection.errcheck = check_value
las.LASPoint_SetScanDirection.restype = ctypes.c_int
las.LASPoint_SetScanDirection.argtypes = [ctypes.c_void_p, ctypes.c_ushort]
las.LASPoint_SetScanDirection.errcheck = check_return

las.LASPoint_GetFlightLineEdge.restype = ctypes.c_ushort
las.LASPoint_GetFlightLineEdge.argtypes = [ctypes.c_void_p]
las.LASPoint_GetFlightLineEdge.errcheck = check_value
las.LASPoint_SetFlightLineEdge.restype = ctypes.c_int
las.LASPoint_SetFlightLineEdge.argtypes = [ctypes.c_void_p, ctypes.c_ushort]
las.LASPoint_SetFlightLineEdge.errcheck = check_return

las.LASPoint_GetScanFlags.restype = ctypes.c_ubyte
las.LASPoint_GetScanFlags.argtypes = [ctypes.c_void_p]
las.LASPoint_GetScanFlags.errcheck = check_value
las.LASPoint_SetScanFlags.restype = ctypes.c_int
las.LASPoint_SetScanFlags.argtypes = [ctypes.c_void_p, ctypes.c_ubyte]
las.LASPoint_SetScanFlags.errcheck = check_return

las.LASPoint_GetClassification.restype = ctypes.c_ubyte
las.LASPoint_GetClassification.argtypes = [ctypes.c_void_p]
las.LASPoint_GetClassification.errcheck = check_value
las.LASPoint_SetClassification.restype = ctypes.c_int
las.LASPoint_SetClassification.argtypes = [ctypes.c_void_p, ctypes.c_ubyte]
las.LASPoint_SetClassification.errcheck = check_return

las.LASPoint_GetTime.restype = ctypes.c_double
las.LASPoint_GetTime.argtypes = [ctypes.c_void_p]
las.LASPoint_GetTime.errcheck = check_value
las.LASPoint_SetTime.restype = ctypes.c_int
las.LASPoint_SetTime.argtypes = [ctypes.c_void_p, ctypes.c_double]
las.LASPoint_SetTime.errcheck = check_return

las.LASPoint_GetScanAngleRank.restype = ctypes.c_int8
las.LASPoint_GetScanAngleRank.argtypes = [ctypes.c_void_p]
las.LASPoint_GetScanAngleRank.errcheck = check_value
las.LASPoint_SetScanAngleRank.restype = ctypes.c_int
las.LASPoint_SetScanAngleRank.argtypes = [ctypes.c_void_p, ctypes.c_int8]
las.LASPoint_SetScanAngleRank.errcheck = check_return

las.LASPoint_GetUserData.restype = ctypes.c_ubyte
las.LASPoint_GetUserData.argtypes = [ctypes.c_void_p]
las.LASPoint_GetUserData.errcheck = check_value
las.LASPoint_SetUserData.restype = ctypes.c_int
las.LASPoint_SetUserData.argtypes = [ctypes.c_void_p, ctypes.c_ubyte]
las.LASPoint_SetUserData.errcheck = check_return

las.LASPoint_Create.restype = ctypes.c_void_p
las.LASPoint_Create.errcheck = check_void

las.LASPoint_Copy.restype = ctypes.c_void_p
las.LASPoint_Copy.argtypes = [ctypes.c_void_p]
las.LASPoint_Copy.errcheck = check_void

las.LASReader_GetHeader.restype = ctypes.c_void_p
las.LASReader_GetHeader.argtypes = [ctypes.c_void_p]
las.LASReader_GetHeader.errcheck = check_void

las.LASHeader_Destroy.argtypes = [ctypes.c_void_p]

las.LASHeader_Copy.restype = ctypes.c_void_p
las.LASHeader_Copy.argtypes = [ctypes.c_void_p]
las.LASHeader_Copy.errcheck = check_void

las.LASHeader_Create.restype = ctypes.c_void_p
las.LASHeader_Create.errcheck = check_void

las.LASHeader_GetFileSignature.argtypes = [ctypes.c_void_p]
las.LASHeader_GetFileSignature.errcheck = check_value_free

las.LASHeader_GetFileSourceId.restype = ctypes.c_ushort
las.LASHeader_GetFileSourceId.argtypes = [ctypes.c_void_p]
las.LASHeader_GetFileSourceId.errcheck = check_value

las.LASHeader_GetProjectId.argtypes = [ctypes.c_void_p]
las.LASHeader_GetProjectId.errcheck = check_value_free

las.LASHeader_GetVersionMajor.restype = ctypes.c_ubyte
las.LASHeader_GetVersionMajor.argtypes = [ctypes.c_void_p]
las.LASHeader_GetVersionMajor.errcheck = check_value
las.LASHeader_SetVersionMajor.restype = ctypes.c_int
las.LASHeader_SetVersionMajor.argtypes = [ctypes.c_void_p, ctypes.c_ubyte]
las.LASHeader_SetVersionMajor.errcheck = check_return

las.LASHeader_GetVersionMinor.restype = ctypes.c_ubyte
las.LASHeader_GetVersionMinor.argtypes = [ctypes.c_void_p]
las.LASHeader_GetVersionMinor.errcheck = check_value
las.LASHeader_SetVersionMinor.restype = ctypes.c_int
las.LASHeader_SetVersionMinor.argtypes = [ctypes.c_void_p, ctypes.c_ubyte]
las.LASHeader_SetVersionMinor.errcheck = check_return

las.LASHeader_GetSystemId.argtypes = [ctypes.c_void_p]
las.LASHeader_GetSystemId.errcheck = check_value_free
las.LASHeader_SetSystemId.restype = ctypes.c_int
las.LASHeader_SetSystemId.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
las.LASHeader_SetSystemId.errcheck = check_return

las.LASHeader_GetSoftwareId.argtypes = [ctypes.c_void_p]
las.LASHeader_GetSoftwareId.errcheck = check_value_free
las.LASHeader_SetSoftwareId.restype = ctypes.c_int
las.LASHeader_SetSoftwareId.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
las.LASHeader_SetSoftwareId.errcheck = check_return

las.LASHeader_GetReserved.restype = ctypes.c_short
las.LASHeader_GetReserved.argtypes = [ctypes.c_void_p]
las.LASHeader_GetReserved.errcheck = check_value

las.LASHeader_GetCreationDOY.restype = ctypes.c_short
las.LASHeader_GetCreationDOY.argtypes = [ctypes.c_void_p]
las.LASHeader_GetCreationDOY.errcheck = check_value
las.LASHeader_SetCreationDOY.restype = ctypes.c_int
las.LASHeader_SetCreationDOY.argtypes = [ctypes.c_void_p, ctypes.c_short]
las.LASHeader_SetCreationDOY.errcheck = check_return

las.LASHeader_GetCreationYear.restype = ctypes.c_short
las.LASHeader_GetCreationYear.argtypes = [ctypes.c_void_p]
las.LASHeader_GetCreationYear.errcheck = check_value
las.LASHeader_SetCreationYear.restype = ctypes.c_int
las.LASHeader_SetCreationYear.argtypes = [ctypes.c_void_p, ctypes.c_short]
las.LASHeader_SetCreationYear.errcheck = check_return

las.LASHeader_GetHeaderSize.restype = ctypes.c_ushort
las.LASHeader_GetHeaderSize.argtypes = [ctypes.c_void_p]
las.LASHeader_GetHeaderSize.errcheck = check_value

las.LASHeader_GetDataOffset.restype = ctypes.c_ulong
las.LASHeader_GetDataOffset.argtypes = [ctypes.c_void_p]
las.LASHeader_GetDataOffset.errcheck = check_value

las.LASHeader_GetRecordsCount.restype = ctypes.c_ulong
las.LASHeader_GetRecordsCount.argtypes = [ctypes.c_void_p]
las.LASHeader_GetRecordsCount.errcheck = check_value

las.LASHeader_GetDataFormatId.restype = ctypes.c_ubyte
las.LASHeader_GetDataFormatId.argtypes = [ctypes.c_void_p]
las.LASHeader_GetDataFormatId.errcheck = check_value

las.LASHeader_SetDataFormatId.restype = ctypes.c_int
las.LASHeader_SetDataFormatId.argtypes = [ctypes.c_void_p, ctypes.c_int]
las.LASHeader_SetDataFormatId.errcheck = check_return

las.LASHeader_GetDataRecordLength.restype = ctypes.c_ushort
las.LASHeader_GetDataRecordLength.argtypes = [ctypes.c_void_p]
las.LASHeader_GetDataRecordLength.errcheck = check_value

las.LASHeader_GetPointRecordsCount.restype = ctypes.c_ulong
las.LASHeader_GetPointRecordsCount.argtypes = [ctypes.c_void_p]
las.LASHeader_GetPointRecordsCount.errcheck = check_value
las.LASHeader_SetPointRecordsCount.restype = ctypes.c_int
las.LASHeader_SetPointRecordsCount.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
las.LASHeader_SetPointRecordsCount.errcheck = check_return

las.LASHeader_GetPointRecordsByReturnCount.restype = ctypes.c_ulong
las.LASHeader_GetPointRecordsByReturnCount.argtypes = [ctypes.c_void_p, ctypes.c_int]
las.LASHeader_GetPointRecordsByReturnCount.errcheck = check_value
las.LASHeader_SetPointRecordsByReturnCount.restype = ctypes.c_int
las.LASHeader_SetPointRecordsByReturnCount.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_ulong]
las.LASHeader_SetPointRecordsByReturnCount.errcheck = check_return

las.LASHeader_GetScaleX.restype = ctypes.c_double
las.LASHeader_GetScaleX.argtypes = [ctypes.c_void_p]
las.LASHeader_GetScaleX.errcheck = check_value
las.LASHeader_GetScaleY.restype = ctypes.c_double
las.LASHeader_GetScaleY.argtypes = [ctypes.c_void_p]
las.LASHeader_GetScaleY.errcheck = check_value
las.LASHeader_GetScaleZ.restype = ctypes.c_double
las.LASHeader_GetScaleZ.argtypes = [ctypes.c_void_p]
las.LASHeader_GetScaleZ.errcheck = check_value
las.LASHeader_SetScale.restype = ctypes.c_int
las.LASHeader_SetScale.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
las.LASHeader_SetScale.errcheck = check_return

las.LASHeader_GetOffsetX.restype = ctypes.c_double
las.LASHeader_GetOffsetX.argtypes = [ctypes.c_void_p]
las.LASHeader_GetOffsetX.errcheck = check_value
las.LASHeader_GetOffsetY.restype = ctypes.c_double
las.LASHeader_GetOffsetY.argtypes = [ctypes.c_void_p]
las.LASHeader_GetOffsetY.errcheck = check_value
las.LASHeader_GetOffsetZ.restype = ctypes.c_double
las.LASHeader_GetOffsetZ.argtypes = [ctypes.c_void_p]
las.LASHeader_GetOffsetZ.errcheck = check_value
las.LASHeader_SetOffset.restype = ctypes.c_int
las.LASHeader_SetOffset.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
las.LASHeader_SetOffset.errcheck = check_return

las.LASHeader_GetMinX.restype = ctypes.c_double
las.LASHeader_GetMinX.argtypes = [ctypes.c_void_p]
las.LASHeader_GetMinX.errcheck = check_value
las.LASHeader_GetMinY.restype = ctypes.c_double
las.LASHeader_GetMinY.argtypes = [ctypes.c_void_p]
las.LASHeader_GetMinY.errcheck = check_value
las.LASHeader_GetMinZ.restype = ctypes.c_double
las.LASHeader_GetMinZ.argtypes = [ctypes.c_void_p]
las.LASHeader_GetMinZ.errcheck = check_value
las.LASHeader_SetMin.restype = ctypes.c_int
las.LASHeader_SetMin.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
las.LASHeader_SetMin.errcheck = check_return

las.LASHeader_GetMaxX.restype = ctypes.c_double
las.LASHeader_GetMaxX.argtypes = [ctypes.c_void_p]
las.LASHeader_GetMaxX.errcheck = check_value
las.LASHeader_GetMaxY.restype = ctypes.c_double
las.LASHeader_GetMaxY.argtypes = [ctypes.c_void_p]
las.LASHeader_GetMaxY.errcheck = check_value
las.LASHeader_GetMaxZ.restype = ctypes.c_double
las.LASHeader_GetMaxZ.argtypes = [ctypes.c_void_p]
las.LASHeader_GetMaxZ.errcheck = check_value
las.LASHeader_SetMax.restype = ctypes.c_int
las.LASHeader_SetMax.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
las.LASHeader_SetMax.errcheck = check_return

las.LASWriter_Create.restype = ctypes.c_void_p
las.LASWriter_Create.argtypes = [ctypes.c_char_p, ctypes.c_void_p, ctypes.c_int]
las.LASWriter_Create.errcheck = check_void

las.LASWriter_WritePoint.restype = ctypes.c_int
las.LASWriter_WritePoint.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
las.LASWriter_WritePoint.errcheck = check_return

las.LASWriter_WriteHeader.restype = ctypes.c_int
las.LASWriter_WriteHeader.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
las.LASWriter_WriteHeader.errcheck = check_return

las.LASWriter_Destroy.argtypes = [ctypes.c_void_p]
las.LASWriter_Destroy.errcheck = check_void_done

las.LASGuid_Destroy.argtypes = [ctypes.c_void_p]
las.LASGuid_Destroy.errcheck = check_void_done

las.LASGuid_AsString.argtypes = [ctypes.c_void_p]
las.LASGuid_AsString.errcheck = check_value_free

las.LASGuid_Equals.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
las.LASGuid_Equals.restype = ctypes.c_int
las.LASGuid_Equals.errcheck = check_value

las.LASGuid_CreateFromString.argtypes =[ctypes.c_char_p]
las.LASGuid_CreateFromString.errcheck = check_void
las.LASGuid_CreateFromString.restype = ctypes.c_void_p

las.LASGuid_Create.errcheck = check_void
las.LASGuid_Create.restype = ctypes.c_void_p

las.LASHeader_GetGUID.argtypes = [ctypes.c_void_p]
las.LASHeader_GetGUID.errcheck = check_void
las.LASHeader_GetGUID.restype = ctypes.c_void_p
las.LASHeader_SetGUID.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
las.LASHeader_SetGUID.errcheck = check_value
las.LASHeader_SetGUID.restype = ctypes.c_int

