/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/TrackingTranslationalController.cpp
 */

#include <math.h>
#include <iostream>
#include "control/include/Helpers.h"
#include "math/include/Vector3.h"

namespace ram {
namespace control {

math::Quaternion yawVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    math::Quaternion q_change(math::Degree(degrees), math::Vector3::UNIT_Z);
    return currentOrientation * q_change;
}

math::Quaternion pitchVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    math::Quaternion q_change(math::Degree(degrees), math::Vector3::UNIT_Y);
    return currentOrientation * q_change;
}

math::Quaternion rollVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    math::Quaternion q_change(math::Degree(degrees), math::Vector3::UNIT_X);
    return currentOrientation * q_change;
}

math::Quaternion holdCurrentHeadingHelper(
    const math::Quaternion& currentOrientation)
{
    // find approximation of vehicle yaw based off orientation
    math::Radian yaw = currentOrientation.getYaw();

    // compute "level" quaternion based off yaw value
    math::Quaternion q(math::Radian(yaw),math::Vector3::UNIT_Z);

    return q;
}

} // namespace control
} // namespace ram
