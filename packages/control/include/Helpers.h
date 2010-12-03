/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/TrackingTranslationalController.cpp
 */

#include "math/include/Matrix2.h"
#include "math/include/Quaternion.h"
#include "math/include/Helpers.h"

namespace ram {
namespace control {

math::Quaternion yawVehicleHelper(const math::Quaternion& currentOrientation, double degrees);
math::Quaternion rollVehicleHelper(const math::Quaternion& currentOrientation, double degrees);
math::Quaternion pitchVehicleHelper(const math::Quaternion& currentOrientation, double degrees);
math::Quaternion holdCurrentHeadingHelper(const math::Quaternion& currentOrientation);

} // namespace control
} // namespace ram
