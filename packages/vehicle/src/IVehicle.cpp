/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Vehicle.cpp
 */

// Project Includes
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/IStateEstimator.h"

#include "math/include/Events.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, ORIENTATION_UPDATE);
//RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, LINEAR_ACCEL_UPDATE);
//RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, ANGULAR_RATE_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, DEPTH_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, POSITION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, VELOCITY_UPDATE);

namespace ram {
namespace vehicle {
    
IVehicle::IVehicle(std::string name, core::EventHubPtr eventHub) :
    core::Subsystem(name, eventHub)
{
}

device::IDevicePtr IVehicle::getDevice(std::string name)
{
    return device::IDevicePtr();
}
    
    
IVehicle::~IVehicle()
{    
}

void IVehicle::handleReturn(int flags)
{
    // Sends events to values that have been flagged
    if (flags & device::StateFlag::POS) {
	math::Vector2EventPtr event(new math::Vector2Event());
	event->vector2 = getPosition();
	publish(IVehicle::POSITION_UPDATE, event);
    }

    if (flags & device::StateFlag::VEL) {
	math::Vector2EventPtr event(new math::Vector2Event());
	event->vector2 = getVelocity();
	publish(IVehicle::VELOCITY_UPDATE, event);
    }

    if (flags & device::StateFlag::DEPTH) {
	math::NumericEventPtr event(new math::NumericEvent());
	event->number = getDepth();
	publish(IVehicle::DEPTH_UPDATE, event);
    }

    if (flags & device::StateFlag::ORIENTATION) {
	math::OrientationEventPtr event(new math::OrientationEvent());
	event->orientation = getOrientation();
	publish(IVehicle::ORIENTATION_UPDATE, event);
    }
}

} // namespace vehicle
} // namespace ram
