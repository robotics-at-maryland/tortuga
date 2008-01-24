/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/src/SubsystemConverter.cpp
 */

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/SubsystemConverter.h"

namespace bp = boost::python;

namespace ram {
namespace core {

template<>
SubsystemConverter::ConverterRegistry* SubsystemConverter::getConverterRegistry()
{
    static SubsystemConverter::ConverterRegistry subsystemConverterRegistry;
    return &subsystemConverterRegistry;
}

} // namespace core
} // namespace ram
