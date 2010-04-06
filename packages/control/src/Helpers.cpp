/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/TrackingTranslationalController.cpp
 */

#include <math.h>
#include "control/include/Helpers.h"

namespace ram {
namespace control {

math::Matrix2 bRn(double radians)
{
    double r_cos = cos(radians), r_sin = sin(radians);
    return math::Matrix2(r_cos,-r_sin,r_sin,r_cos);
}

math::Matrix2 nRb(double radians)
{
    double r_cos = cos(radians), r_sin = sin(radians);
    return math::Matrix2(r_cos,r_sin,-r_sin,r_cos);
}

} // namespace control
} // namespace ram
