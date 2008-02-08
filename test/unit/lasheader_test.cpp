// $Id$
#include <liblas/lasheader.hpp>
#include <liblas/guid.hpp>
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
        using liblas::LASHeader;

        ensure_equals("wrong default file signature",
            m_default.GetFileSignature(), LASHeader::FileSignature);

        ensure_equals("wrong default file source id",
                      m_default.GetFileSourceId(), 0);
        ensure_equals("wrong default reserved value",
                      m_default.GetReserved(), 0);

        liblas::guid g;
        ensure_equals("wrong default project guid",
                      m_default.GetProjectId(), g);
        
        ensure_equals("wrong default major version",
                      m_default.GetVersionMajor(), 1);
        ensure_equals("wrong default minor version",
                       m_default.GetVersionMinor(), 0);
        
        ensure_equals("wrong default system id",
                      m_default.GetSystemId(), LASHeader::SystemIdentifier);
        ensure_equals("wrong default software id",
                      m_default.GetSoftwareId(), LASHeader::SoftwareIdentifier);

        ensure_equals("wrong default creation day-of-year",
                      m_default.GetCreationDOY(), 0);
        ensure_equals("wrong default creation year",
                      m_default.GetCreationYear(), 0);
        ensure_equals("wrong default header size",
                      m_default.GetHeaderSize(), 227);
        ensure_equals("wrong default data offset",
                      m_default.GetDataOffset(), 229);
        ensure_equals("wrong default records count",
                      m_default.GetRecordsCount(), 0);
        ensure_equals("wrong default data format id",
                      m_default.GetDataFormatId(), LASHeader::ePointFormat0);
        ensure_equals("wrong default data record length",
                      m_default.GetDataRecordLength(), LASHeader::ePointSize0);
        ensure_equals("wrong default point records count",
                      m_default.GetPointRecordsCount(), 0);

        ensure_equals("wrong default X scale",
                      m_default.GetScaleX(), double(0.01));
        ensure_equals("wrong default Y scale",
                      m_default.GetScaleY(), double(0.01));
        ensure_equals("wrong default Z scale",
                      m_default.GetScaleZ(), double(0.01));
        
        ensure_equals("wrong default X offset",
                      m_default.GetOffsetX(), double(0));
        ensure_equals("wrong default Y offset",
                      m_default.GetOffsetY(), double(0));
        ensure_equals("wrong default Z offset",
                      m_default.GetOffsetZ(), double(0));
        
        ensure_equals("wrong default min X",
                      m_default.GetMinX(), double(0));
        ensure_equals("wrong default max X",
                      m_default.GetMaxX(), double(0));
        ensure_equals("wrong default min Y",
                      m_default.GetMinY(), double(0));
        ensure_equals("wrong default max Y",
                      m_default.GetMaxY(), double(0));
        ensure_equals("wrong default min Z",
                      m_default.GetMinZ(), double(0));
        ensure_equals("wrong default max Z",
                      m_default.GetMaxZ(), double(0));
    }
};
