#ifndef LIBLAS_LASERROR_HPP_INCLUDED
#define LIBLAS_LASERROR_HPP_INCLUDED

//std
#include <iosfwd>
#include <string>

namespace liblas {
    
class LASError
{
public:

    LASError(int code, std::string message, std::string method);
    LASError(LASError const& other);
    LASError& operator=(LASError const& rhs);

    std::string GetMethod() const { return m_method; };
    std::string GetMessage() const { return m_message; };
    int GetCode() const {return m_code; };

private:
    int m_code;
    std::string m_message;
    std::string m_method;


};

} // namespace liblas

#endif