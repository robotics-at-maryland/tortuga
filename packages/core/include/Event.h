/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Event.h
 */

#ifndef RAM_CORE_EVENT_H_11_19_2007
#define RAM_CORE_EVENT_H_11_19_2007

// STD Includes
#include <string>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/Forward.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {
    
struct RAM_EXPORT Event
{
    typedef std::string EventType;
    
    EventType type;
    EventPublisher* sender;
};
    
} // namespace core
} // namespace ram

#define RAM_CORE_EVENT_STR(S) # S
#define RAM_CORE_EVENT_TYPE(_class, name) \
    const ram::core::Event::EventType _class  :: name  \
    (std::string(__FILE__) + RAM_CORE_EVENT_STR(name))

#endif // RAM_CORE_EVENT_H_11_19_2007
