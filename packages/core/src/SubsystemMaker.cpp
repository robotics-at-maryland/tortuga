/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/SubsystemMaker.cpp
 */

// Project Includes
#include "core/include/SubsystemMaker.h"

// In this case the static variable is safe
#if RAM_COMPILER == RAM_COMPILER_MSVC
#  pragma warning( disable : 4640 )
#endif

// This must be here so that there is only static registry, otherwise each
// include of the SubsystemMaker.h header will result in a different registry
namespace ram {
namespace pattern {

template<>
core::SubsystemMaker::MakerMap* core::SubsystemMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static core::SubsystemMaker::MakerMap* reg =
        new core::SubsystemMaker::MakerMap();
    return reg;
}

} // namespace pattern
} // namespace ram
