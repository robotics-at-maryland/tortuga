/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/math/src/Events.cpp
 */

// Project Includes
#include "core/include/Feature.h"
#include "math/include/Events.h"

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS) && defined(RAM_WITH_CORE)

#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::math::NumericEvent>
RAM_MATH_NUMERICEVENT;

static ram::core::SpecificEventConverter<ram::math::Vector2Event>
RAM_MATH_VECTOR2EVENT;

static ram::core::SpecificEventConverter<ram::math::Vector3Event>
RAM_MATH_VECTOR3EVENT;

static ram::core::SpecificEventConverter<ram::math::OrientationEvent>
RAM_MATH_ORIENTATIONEVENT;

#endif // RAM_WITH_WRAPPERS

namespace ram {
namespace math {

core::EventPtr OrientationEvent::clone()
{
    OrientationEventPtr event = OrientationEventPtr(new OrientationEvent());
    copyInto(event);
    event->orientation = orientation;
    return event;
}

core::EventPtr Vector2Event::clone()
{
    Vector2EventPtr event = Vector2EventPtr(new Vector2Event());
    copyInto(event);
    event->vector2 = vector2;
    return event;
}

core::EventPtr Vector3Event::clone()
{
    Vector3EventPtr event = Vector3EventPtr(new Vector3Event());
    copyInto(event);
    event->vector3 = vector3;
    return event;
}

core::EventPtr NumericEvent::clone()
{
    NumericEventPtr event = NumericEventPtr(new NumericEvent());
    copyInto(event);
    event->number = number;
    return event;
}
    
} // namespace math
} // namespace ram
