/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/vision/src/Workaround.cpp
 */

// Library Includes
//#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
//#include "control/include/IController.h"

//namespace bp = boost::python;

// Needed because Py++ insists on trying to generate a wrapper for this class
// even though we are doing it manually
void register_SubsystemList_class()
{
}

/*void registerVisionSystemPtrs()
{
    bp::register_ptr_to_python< boost::shared >();
    bp::implicitly_convertible< ram::control::IControllerPtr,
                                ram::core::SubsystemPtr >();
}
*/

