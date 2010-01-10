/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/device/IStateEstimatorDevice.h
 */

// STD Includes
#include <cassert>
#include <iostream>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IStateEstimator.h"
#include "vehicle/include/device/IStateEstimatorDevice.h"
#include "vehicle/include/IVehicle.h"

namespace ram {
namespace vehicle {
namespace device {

IStateEstimatorDevice::IStateEstimatorDevice(core::EventHubPtr eventHub,
					     std::string name) :
    IDevice(eventHub, name),
    m_stateEstimator(IStateEstimatorPtr())
{
}

void IStateEstimatorDevice::setStateEstimator(IStateEstimatorPtr estimator)
{
    // Assigning a device to multiple state estimator should not be done
    // Causes a crash so this won't silently happen
    assert(m_stateEstimator == NULL &&
	   "A device can only be monitored by one state estimator");

    m_stateEstimator = estimator;
}

void IStateEstimatorDevice::setVehicle(IVehiclePtr vehicle)
{
    // Check that the device exists
    if (vehicle->hasDevice("StateEstimator")) {
	IStateEstimatorPtr estimator =
	    IDevice::castTo<IStateEstimator>(
	        vehicle->getDevice("StateEstimator"));
	setStateEstimator(estimator);
    } else {
	std::cout <<
	    "WARNING: Vehicle object does not have a state estimator"
		  << std::endl;
    }
}

IStateEstimatorPtr IStateEstimatorDevice::getStateEstimator()
{
    return m_stateEstimator;
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
