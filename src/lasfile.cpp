#include <liblas/lasfile.hpp>
#include <fstream>
#include <stdexcept>

LASFile::LASFile(std::string filename, int mode) :
    m_filename(filename), m_mode(mode)
{
    // if (filename == "stdin") {
    //     m_strm = &std::cin;
    // }
    // 
    // if (filename == "stdout" ) {
    //     m_strm = &std::cout;
    // }
    
    if (mode == LASFile::eRead) {
        m_strm = new std::ifstream(m_filename.c_str(), std::ios::in | std::ios::binary);
        if (!(m_strm->good())) {
            throw std::runtime_error("Input stream for read operation was not good");
        } 
    }
    if (mode == LASFile::eWrite) {
        m_strm = new std::ofstream(m_filename.c_str(), std::ios::out | std::ios::binary);
        if (!(m_strm->good())) {
            throw std::runtime_error("Output stream for write operation was not good");
        } 

    }
}

LASFile::~LASFile() {
   // m_strm->close();
}