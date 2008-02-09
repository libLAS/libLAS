// $Id$
//
// (C) Copyright Howard Butler 2008
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#ifndef LIBLAS_LASFILE_HPP_INCLUDED
#define LIBLAS_LASFILE_HPP_INCLUDED

#include <string>
#include <iostream>
#include <fstream>

#include <exception>

class LASFile
{
public:

    LASFile(std::string filename, int mode);
    ~LASFile();
    
    std::ios*  GetStream() {return m_strm;}
    std::string GetFilename() {return m_filename;}
    
    enum OpenMode
    {
        eRead = 0,
        eWrite = 1,
        eUpdate = 2
    };

private:
    std::ios* m_strm;
    std::string m_filename;
    int m_mode;
    LASFile();
};

#endif
