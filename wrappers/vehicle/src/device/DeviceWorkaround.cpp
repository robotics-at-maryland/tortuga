/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/vehicle/src/device/DeviceWorkaround.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "vehicle/include/device/Common.h"
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/IStateEstimatorDevice.h"
#include "vehicle/include/device/IThruster.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IPowerSource.h"
#include "vehicle/include/device/ITempSensor.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDVL.h"
#include "vehicle/include/device/ISonar.h"
#include "vehicle/include/device/IPayloadSet.h"
#include "vehicle/include/device/IVelocitySensor.h"
#include "vehicle/include/device/IPositionSensor.h"

namespace bp = boost::python;

void registerIDevicePtrs()
{
    bp::register_ptr_to_python< ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IStateEstimatorPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IStateEstimatorPtr,
	                        ram::vehicle::device::IDevicePtr >();
    
    bp::register_ptr_to_python< ram::vehicle::device::IStateEstimatorDevicePtr >();
    bp::implicitly_convertible< ram::vehicle::device::IStateEstimatorDevicePtr,
	                        ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IThrusterPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IThrusterPtr,
                                ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IDepthSensorPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IDepthSensorPtr,
                                ram::vehicle::device::IStateEstimatorDevicePtr >();
    
    bp::register_ptr_to_python< ram::vehicle::device::IPowerSourcePtr >();
    bp::implicitly_convertible< ram::vehicle::device::IPowerSourcePtr,
                                ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::ITempSensorPtr >();
    bp::implicitly_convertible< ram::vehicle::device::ITempSensorPtr,
                                ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IIMUPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IIMUPtr,
                                ram::vehicle::device::IStateEstimatorDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IDVLPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IDVLPtr,
                                ram::vehicle::device::IStateEstimatorDevicePtr >();    
    
    bp::register_ptr_to_python< ram::vehicle::device::ISonarPtr >();
    bp::implicitly_convertible< ram::vehicle::device::ISonarPtr,
                                ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IPayloadSetPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IPayloadSetPtr,
                                ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IVelocitySensorPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IVelocitySensorPtr,
	                        ram::vehicle::device::IDevicePtr >();

    bp::register_ptr_to_python< ram::vehicle::device::IPositionSensorPtr >();
    bp::implicitly_convertible< ram::vehicle::device::IVelocitySensorPtr,
	                        ram::vehicle::device::IDevicePtr >();
}
