// $Id$
#include <liblas/lasheader.hpp>
#include <tut/tut.hpp>
namespace tut
{ 
    struct lasheader_data
    {
        liblas::LASHeader m_default;
    };

    typedef test_group<lasheader_data> tg;
    typedef tg::object to;

    tg test_group("liblas::LASHeader");

    // Test default construction
    template<>
    template<>
    void to::test<1>()
    {
        ensure("wrong default file signature",
               m_default.GetFileSignature().empty());

        ensure_equals("wrong default file source id",
                      m_default.GetFileSourceId(), 0);
        ensure_equals("wrong default reserved value",
                      m_default.GetReserved(), 0);
        ensure_equals("wrong default project id 1",
                      m_default.GetProjectId1(), 0);
        ensure_equals("wrong default project id 2",
                      m_default.GetProjectId2(), 0);
        ensure_equals("wrong default project id 3",
                      m_default.GetProjectId3(), 0);
        
        ensure("wrong default project id 4",
               m_default.GetProjectId4().empty());
        
        ensure_equals("wrong default major version",
                      m_default.GetVersionMajor(), 0);
        ensure_equals("wrong default minor version",
                       m_default.GetVersionMinor(), 0);
        
        ensure("wrong default system id",
               m_default.GetSystemId().empty());
        ensure("wrong default software id",
               m_default.GetSoftwareId().empty());

        ensure_equals("wrong default creation day-of-year",
                      m_default.GetCreationDOY(), 0);
        ensure_equals("wrong default creation year",
                      m_default.GetCreationYear(), 0);
        ensure_equals("wrong default header size",
                      m_default.GetHeaderSize(), 0);
        ensure_equals("wrong default data offset",
                      m_default.GetDataOffset(), 0);
        ensure_equals("wrong default records count",
                      m_default.GetRecordsCount(), 0);
        ensure_equals("wrong default data format id",
                      m_default.GetDataFormatId(), 0);
        ensure_equals("wrong default data record length",
                      m_default.GetDataRecordLength(), 0);
        ensure_equals("wrong default point records count",
                      m_default.GetPointRecordsCount(), 0);

        ensure_equals("wrong default X scale",
                      m_default.GetScales().x, double(0));
        ensure_equals("wrong default Y scale",
                      m_default.GetScales().y, double(0));
        ensure_equals("wrong default Z scale",
                      m_default.GetScales().z, double(0));
        
        ensure_equals("wrong default X offset",
                      m_default.GetOffsets().x, double(0));
        ensure_equals("wrong default Y offset",
                      m_default.GetOffsets().y, double(0));
        ensure_equals("wrong default Z offset",
                      m_default.GetOffsets().z, double(0));
        
        ensure_equals("wrong default min X",
                      m_default.GetExtents().min.x, double(0));
        ensure_equals("wrong default max X",
                      m_default.GetExtents().max.x, double(0));
        ensure_equals("wrong default min Y",
                      m_default.GetExtents().min.y, double(0));
        ensure_equals("wrong default max Y",
                      m_default.GetExtents().max.y, double(0));
        ensure_equals("wrong default min Z",
                      m_default.GetExtents().min.z, double(0));
        ensure_equals("wrong default max Z",
                      m_default.GetExtents().max.z, double(0));
    }
};
