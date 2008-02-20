// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <liblas/lasheader.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/guid.hpp>
#include <tut/tut.hpp>

namespace tut
{ 
    struct lasheader_data
    {
        liblas::LASHeader m_default;

        void test_default_header(liblas::LASHeader const& h)
        {
            using liblas::LASHeader;

            ensure_equals("wrong default file signature",
                h.GetFileSignature(), LASHeader::FileSignature);

            ensure_equals("wrong default file source id",
                h.GetFileSourceId(), 0);
            ensure_equals("wrong default reserved value",
                h.GetReserved(), 0);

            liblas::guid g;
            ensure_equals("wrong default project guid",
                h.GetProjectId(), g);

            ensure_equals("wrong default major version",
                h.GetVersionMajor(), 1);
            ensure_equals("wrong default minor version",
                h.GetVersionMinor(), 1);

            ensure_equals("wrong default system id",
                h.GetSystemId(), LASHeader::SystemIdentifier);
            ensure_equals("wrong default software id",
                h.GetSoftwareId(), LASHeader::SoftwareIdentifier);

            ensure_equals("wrong default creation day-of-year",
                h.GetCreationDOY(), 0);
            ensure_equals("wrong default creation year",
                h.GetCreationYear(), 0);
            ensure_equals("wrong default header size",
                h.GetHeaderSize(), liblas::uint16_t(227));
            ensure_equals("wrong default data offset",
                h.GetDataOffset(), liblas::uint32_t(229));
            ensure_equals("wrong default records count",
                h.GetRecordsCount(), liblas::uint32_t(0));
            ensure_equals("wrong default data format id",
                h.GetDataFormatId(), LASHeader::ePointFormat0);
            ensure_equals("wrong default data record length",
                h.GetDataRecordLength(), LASHeader::ePointSize0);
            ensure_equals("wrong default point records count",
                h.GetPointRecordsCount(), liblas::uint32_t(0));

            ensure_equals("wrong default X scale", h.GetScaleX(), double(0.01));
            ensure_equals("wrong default Y scale", h.GetScaleY(), double(0.01));
            ensure_equals("wrong default Z scale", h.GetScaleZ(), double(0.01));

            ensure_equals("wrong default X offset", h.GetOffsetX(), double(0));
            ensure_equals("wrong default Y offset", h.GetOffsetY(), double(0));
            ensure_equals("wrong default Z offset", h.GetOffsetZ(), double(0));

            ensure_equals("wrong default min X", h.GetMinX(), double(0));
            ensure_equals("wrong default max X", h.GetMaxX(), double(0));
            ensure_equals("wrong default min Y", h.GetMinY(), double(0));
            ensure_equals("wrong default max Y", h.GetMaxY(), double(0));
            ensure_equals("wrong default min Z", h.GetMinZ(), double(0));
            ensure_equals("wrong default max Z", h.GetMaxZ(), double(0));
        }
    };

    typedef test_group<lasheader_data> tg;
    typedef tg::object to;

    tg test_group_lasheader("liblas::LASHeader");

    // Test default constructor
    template<>
    template<>
    void to::test<1>()
    {
        test_default_header(m_default);
    }

    // Test copy constructor
    template<>
    template<>
    void to::test<2>()
    {
        using liblas::LASHeader;
        
        LASHeader copy_of_default(m_default);
        test_default_header(copy_of_default);

        std::string sig("LASF and garbage");

        LASHeader h1;
        
        h1.SetFileSignature(sig);
        ensure_not(h1.GetFileSignature() == sig);
        ensure_equals(h1.GetFileSignature().size(), 4);
        ensure_equals(h1.GetFileSignature(), LASHeader::FileSignature);

        LASHeader h2(h1);

        ensure_not(h2.GetFileSignature() == sig);
        ensure_equals(h2.GetFileSignature().size(), 4);
        ensure_equals(h2.GetFileSignature(), LASHeader::FileSignature);
    }


    // Test assignment operator
    template<>
    template<>
    void to::test<3>()
    {
        using liblas::LASHeader;
        
        LASHeader copy_of_default;
        copy_of_default = m_default;
        test_default_header(copy_of_default);

        std::string sig("LASF and garbage");

        LASHeader h1;
        h1.SetFileSignature(sig);

        ensure_not(h1.GetFileSignature() == sig);
        ensure_equals(h1.GetFileSignature().size(), 4);
        ensure_equals(h1.GetFileSignature(), LASHeader::FileSignature);

        LASHeader h2;
        h2 = h1;

        ensure_not(h2.GetFileSignature() == sig);
        ensure_equals(h2.GetFileSignature().size(), 4);
        ensure_equals(h2.GetFileSignature(), LASHeader::FileSignature);
    }

    // Test Get/SetSystemId
    template<>
    template<>
    void to::test<4>()
    {
        using liblas::LASHeader;

        std::string sysid1("Short Sys Id"); // 12 bytes
        std::string::size_type const len1 = sysid1.size();
        std::string sysid2("Long System Identifier - and some garbage"); // 41 bytes
        std::string::size_type const len2 = sysid2.size();

        LASHeader h;

        h.SetSystemId(sysid1);
        ensure_equals(h.GetSystemId(), sysid1);
        ensure_equals(h.GetSystemId().size(), len1);
        ensure_equals(h.GetSystemId(true).size(), 32);

        // TODO: Should we allow passing longer identifier than 32 bytes
        //       and truncate it implicitly, so the following test will pass?
        //h.SetSystemId(sysid2);
        //ensure_equals(h.GetSystemId(), sysid2.substr(0, 32));
        //ensure_equals(h.GetSystemId().size(), len2);
        //ensure_equals(h.GetSystemId(true).size(), 32);
    }
}

