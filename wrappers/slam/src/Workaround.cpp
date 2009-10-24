/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  wrappers/slam/src/Workaround.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "slam/include/ISlam.h"

namespace bp = boost::python;

// Needed because Py++ insists on trying to generate a wrapper for this class
// even though we are doing it manually
void register_SubsystemList_class()
{
}

void registerISlamPtrs()
{
    bp::register_ptr_to_python< ram::slam::ISlamPtr >();
    bp::implicitly_convertible< ram::slam::ISlamPtr,
                                ram::core::SubsystemPtr >();
}

