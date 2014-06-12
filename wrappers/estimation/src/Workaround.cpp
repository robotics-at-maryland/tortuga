/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  wrappers/estimation/src/Workaround.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "estimation/include/IStateEstimator.h"

namespace bp = boost::python;

// Needed because Py++ insists on trying to generate a wrapper for this class
// even though we are doing it manually
void register_SubsystemList_class()
{
}

void registerIStateEstimatorPtrs()
{
    bp::register_ptr_to_python< ram::estimation::IStateEstimatorPtr >();
    bp::implicitly_convertible< ram::estimation::IStateEstimatorPtr,
                                ram::core::SubsystemPtr >();
}

