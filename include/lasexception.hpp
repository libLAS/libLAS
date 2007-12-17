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

#ifndef __INCLUDE_LASEXCEPTION_HPP
#define __INCLUDE_LASEXCEPTION_HPP

#include <exception>
namespace liblas {
class file_not_found : public virtual std::runtime_error {};
class unsupported_format : public virtual std::runtime_error {};
class unsupported_version : public virtual std::runtime_error {};
}; /* end namespace liblas */

#endif /* ndef __INCLUDE_LASEXCEPTION_HPP */
