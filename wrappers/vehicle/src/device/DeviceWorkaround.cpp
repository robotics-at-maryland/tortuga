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
#include "vehicle/include/device/Common.h"
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IThruster.h"
#include "vehicle/include/device/IPowerSource.h"
#include "vehicle/include/device/ITempSensor.h"

namespace bp = boost::python;

void registerIDevicePtrs()
{
    bp::register_ptr_to_python< ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IThrusterPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IThrusterPtr,
                                ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IPowerSourcePtr >();
    bp::implicitly_convertible< ram::vehicle::device::IPowerSourcePtr,
                                ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::ITempSensorPtr >();
    bp::implicitly_convertible< ram::vehicle::device::ITempSensorPtr,
                                ram::vehicle::device::IDevicePtr >();
}
