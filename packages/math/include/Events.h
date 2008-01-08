/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/include/Events.h
 */

#ifndef RAM_MATH_EVENTS_01_08_2008
#define RAM_MATH_EVENTS_01_08_2008

// Project Includes
#include "core/include/Event.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace math {

class OrientationEvent : public core::Event
{
    Quaternion orientation;
}

class Vector3Event : public core::Event
{
    Vector vector;
}

class NumericEvent : public core::Event
{
    double value;
}
    
} // namespace math
} // namespace ram

#endif // RAM_MATH_EVENTS_01_08_2008
