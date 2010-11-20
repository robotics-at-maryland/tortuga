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
    //use Helpers.cpp
    using namespace ram::math;

    //create rotation quaternion based on user input
    double rotationQuaternion[4];
    double rollAxis[3] = {0,0,1};
    quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
    //rotate the quaternion and store in a new place
    math::Quaternion newQuaternion;
    quaternionCrossProduct(currentOrientation.ptr(), rotationQuaternion,
                           (double*)newQuaternion.ptr());

    return newQuaternion;
}

math::Quaternion pitchVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    //use Helpers.cpp
    using namespace ram::math;

    //create rotation quaternion based on user input
    double rotationQuaternion[4];
    double rollAxis[3] = {0,1,0};
    quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
    //rotate the quaternion and store in a new place
    math::Quaternion newQuaternion;
    quaternionCrossProduct(currentOrientation.ptr(), rotationQuaternion,
                           (double*)newQuaternion.ptr());

    return newQuaternion;
    
}

math::Quaternion rollVehicleHelper(
    const math::Quaternion& currentOrientation, double degrees)
{
    //use Helpers.cpp
    using namespace ram::math;

    //create rotation quaternion based on user input
    double rotationQuaternion[4];
    double rollAxis[3] = {1,0,0};
    quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
    //rotate the quaternion and store in a new place
    math::Quaternion newQuaternion;
    quaternionCrossProduct(currentOrientation.ptr(), rotationQuaternion,
                           (double*)newQuaternion.ptr());

    return newQuaternion;

}

math::Quaternion holdCurrentHeadingHelper(
    const math::Quaternion& currentOrientation)
{
    //find approximation of vehicle yaw based off orientation
    math::Radian yaw = currentOrientation.getYaw();

    //compute "level" quaternion based off yaw value
    math::Quaternion q(math::Radian(yaw),math::Vector3::UNIT_Z);

    return q;
}

} // namespace control
} // namespace ram
