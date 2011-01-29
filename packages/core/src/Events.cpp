/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/src/Events.cpp
 */

// Project Includes
#include "core/include/Feature.h"
#include "core/include/Events.h"

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS)

#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::core::StringEvent>
RAM_CORE_STRINGEVENT;
static ram::core::SpecificEventConverter<ram::core::IntEvent>
RAM_CORE_INTEVENT;
   
#endif // RAM_WITH_WRAPPERS

namespace ram {
namespace core {

EventPtr StringEvent::clone()
{
    StringEventPtr event = StringEventPtr(new StringEvent());
    copyInto(event);
    event->string = string;
    return event;
}

EventPtr IntEvent::clone()
{
    IntEventPtr event = IntEventPtr(new IntEvent());
    copyInto(event);
    event->data = data;
    return event;
}

EventPtr Plot1DEvent::clone()
{
    Plot1DEventPtr event = Plot1DEventPtr(new Plot1DEvent());
    copyInto(event);
    event->m_plotName = m_plotName;
    event->m_data = m_data;
    event->m_labels = m_labels;

    return event;
}

EventPtr Plot2DEvent::clone()
{
    Plot2DEventPtr event = Plot2DEventPtr(new Plot2DEvent());
    copyInto(event);
    event->m_plotName = m_plotName;
    event->m_xData = m_xData;
    event->m_yData = m_yData;
    event->m_xLabels = m_xLabels;
    event->m_yLabels = m_yLabels;

    return event;
}

} // namespace core
} // namespace ram
