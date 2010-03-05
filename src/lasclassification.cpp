/******************************************************************************
 * $Id$
 *
 * Project:  libLAS - http://liblas.org - A BSD library for LAS format data.
 * Purpose:  Implementation of LASClassification type.
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ******************************************************************************
 * Copyright (c) 2009, Mateusz Loskot
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following 
 * conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided 
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department 
 *       of Natural Resources nor the names of its contributors may be 
 *       used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 ****************************************************************************/

#include <liblas/lasclassification.hpp>
#include <liblas/cstdint.hpp>
#include <liblas/detail/utility.hpp>
// std
#include <cstddef>
#include <string>

namespace {

static std::string g_class_names[] =
{
    "Created, never classified",
    "Unclassified",
    "Ground",
    "Low Vegetation",
    "Medium Vegetation",
    "High Vegetation",
    "Building",
    "Low Point (noise)",
    "Model Key-point (mass point)",
    "Water",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Overlap Points",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition",
    "Reserved for ASPRS Definition"
};

} // namespace anonymous

namespace liblas {

std::size_t const Classification::class_table_size = detail::static_array_size(g_class_names);

std::string Classification::GetClassName() const
{
    std::size_t const index = GetClass();
    check_class_index(index);
    
    return g_class_names[index];
}

} // namespace liblas
