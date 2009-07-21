/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/Events.h
 */

#ifndef RAM_CONTROL_EVENTS_11_27_2008
#define RAM_CONTROL_EVENTS_11_27_2008

// STD Includes
#include <string>
#include <vector>

// Project Includes
#include "core/include/Event.h"

namespace ram {
namespace control {

struct ParamSetupEvent : public core::Event
{
    std::vector<std::string> labels;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<ParamSetupEvent> ParamSetupEventPtr;

struct ParamUpdateEvent : public core::Event
{
    std::vector<double> values;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<ParamUpdateEvent> ParamUpdateEventPtr;
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_EVENTS_11_27_2008
