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
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"

namespace ram {
namespace math {

struct OrientationEvent : public core::Event
{
    Quaternion orientation;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<OrientationEvent> OrientationEventPtr;

struct Vector2Event : public core::Event
{
    Vector2 vector2;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<Vector2Event> Vector2EventPtr;
    
struct Vector3Event : public core::Event
{
    Vector3 vector3;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<Vector3Event> Vector3EventPtr;
    
struct NumericEvent : public core::Event
{
    double number;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<NumericEvent> NumericEventPtr;
    
} // namespace math
} // namespace ram

#endif // RAM_MATH_EVENTS_01_08_2008
