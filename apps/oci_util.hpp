#ifndef OCI_UTIL_HPP_INCLUDED
#define OCI_UTIL_HPP_INCLUDED


#include "oci_wrapper.h"
#include <liblas/export.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include <boost/cstdint.hpp>
#include <boost/concept_check.hpp>


LAS_DLL bool EnableTracing(OWConnection* connection);
LAS_DLL bool IsGeographic(OWConnection* connection, long srid);
LAS_DLL OWStatement* RunSQL(OWConnection* connection, std::ostringstream& command);
LAS_DLL bool Cleanup(OWConnection* connection, std::string tableName);
LAS_DLL bool CreateBlockTable(OWConnection* connection, std::string tableName);
LAS_DLL bool BlockTableExists(OWConnection* connection, std::string tableName);
LAS_DLL bool CreateBlockIndex(  OWConnection* connection, 
                        std::string tableName, 
                        long srid, 
                        bool bUse3d);

LAS_DLL bool WipeBlockTable(   OWConnection* connection, 
                    std::string tableName, 
                    std::string cloudTableName, 
                    std::string cloudColumnName);
LAS_DLL std::string to_upper(const std::string&);
LAS_DLL std::string ReadSQLData(std::string filename);
LAS_DLL std::istream* OpenInput(std::string const& filename, bool bEnd) ;

#ifdef _WIN32
#define compare_no_case(a,b,n)  _strnicmp( (a), (b), (n) )
#else
#define compare_no_case(a,b,n)  strncasecmp( (a), (b), (n) )
#endif

#endif // OCI_UTIL_HPP_INCLUDED

