/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IStateEstimator.cpp
 */

// Project Includes
#include "vehicle/include/device/IStateEstimator.h"
#include "math/include/Events.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IStateEstimator, ORIENTATION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IStateEstimator, LINEAR_ACCEL_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IStateEstimator, ANGULAR_RATE_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IStateEstimator, DEPTH_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IStateEstimator, POSITION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IStateEstimator, VELOCITY_UPDATE);

namespace ram {
namespace vehicle {
namespace device {

IStateEstimator::IStateEstimator(core::EventHubPtr eventHub,
                                 std::string name) :
    IDevice(eventHub, name)
{
}

IStateEstimator::~IStateEstimator()
{
}

void IStateEstimator::publishOrientation()
{
    math::OrientationEventPtr oevent(new math::OrientationEvent());
    oevent->orientation = getOrientation();
    publish(ORIENTATION_UPDATE, oevent);
}

void IStateEstimator::publishDepth()
{
    math::NumericEventPtr nevent(new math::NumericEvent());
    nevent->number = getDepth();
    publish(DEPTH_UPDATE, nevent);
}

void IStateEstimator::publishVelocity()
{
    math::Vector2EventPtr vevent(new math::Vector2Event());
    vevent->vector2 = getVelocity();
    publish(VELOCITY_UPDATE, vevent);
}

void IStateEstimator::publishPosition()
{
    math::Vector2EventPtr pevent(new math::Vector2Event());
    pevent->vector2 = getPosition();
    publish(POSITION_UPDATE, pevent);
}

} // namespace device
} // namespace vehicle
} // namespace ram
