/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/control/src/Events.cpp
 */

// Project Includes
#include "core/include/Feature.h"
#include "control/include/Events.h"

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS) && defined(RAM_WITH_CORE)

#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::control::ParamSetupEvent>
RAM_CONTROL_PARAMSETUP;

static ram::core::SpecificEventConverter<ram::control::ParamUpdateEvent>
RAM_CONTROL_PARAMUPDATE;

#endif // RAM_WITH_WRAPPERS

namespace ram {
namespace control {

core::EventPtr ParamSetupEvent::clone()
{
    ParamSetupEventPtr event = ParamSetupEventPtr(new ParamSetupEvent());
    copyInto(event);
    event->labels = labels;
    return event;
}

core::EventPtr ParamUpdateEvent::clone()
{
    ParamUpdateEventPtr event = ParamUpdateEventPtr(new ParamUpdateEvent());
    copyInto(event);
    event->values = values;
    return event;
}
    
} // namespace control
} // namespace ram
