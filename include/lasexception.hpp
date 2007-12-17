/***************************************************************************
 * $Id$
 * $Date$
 *
 * Project: libLAS -- C/C++ read/write library for LAS LIDAR data
 * Purpose:  
 * Author:  Phil Vachon <philippe@cowpig.ca>
 *
 ***************************************************************************
 * Copyright (c) 2007, Martin Isenburg <isenburg@cs.unc.edu>
 * Copyright (c) 2007, Phil Vachon <philippe@cowpig.ca> 
 *
 * See LICENSE.txt in this source distribution for more information.
 **************************************************************************/
#ifndef LASEXCEPTION_HPP_INCLUDE
#define LASEXCEPTION_HPP_INCLUDE

#include <stdexcept>

namespace liblas
{

class file_not_found : public virtual std::runtime_error
{
public:
    file_not_found(std::string const& msg)
        : std::runtime_error(msg)
    {}
};

class unsupported_format : public virtual std::runtime_error
{
public:
    unsupported_format(std::string const& msg)
        : std::runtime_error(msg)
    {}
};

class unsupported_version : public virtual std::runtime_error
{
public:
    unsupported_version(std::string const& msg)
        : std::runtime_error(msg)
    {}
};


} /* end namespace liblas */

#endif /* ndef LASEXCEPTION_HPP_INCLUDE */
