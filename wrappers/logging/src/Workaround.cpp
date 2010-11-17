/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  wrappers/logging/src/Workaround.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "logging/include/EventPlayer.h"

namespace bp = boost::python;

// Needed because Py++ insists on trying to generate a wrapper for this class
// even though we are doing it manually
void register_SubsystemList_class()
{
}

void registerLoggingPtrs()
{
    bp::register_ptr_to_python< ram::logging::EventPlayerPtr >();
    bp::implicitly_convertible< ram::logging::EventPlayerPtr,
                                ram::core::SubsystemPtr >();
}
