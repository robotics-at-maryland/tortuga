/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/control/src/Workaround.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "control/include/IController.h"

namespace bp = boost::python;

// Needed because Py++ insists on trying to generate a wrapper for this class
// even though we are doing it manually
void register_SubsystemList_class()
{
}

void registerIControllerPtrs()
{
    bp::register_ptr_to_python< ram::control::IControllerPtr >();
    bp::implicitly_convertible< ram::control::IControllerPtr,
                                ram::core::SubsystemPtr >();
}

