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
#include "vision/include/Events.h"

RAM_CORE_EVENT_TYPE(ram::vision::EventType, LIGHT_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, LIGHT_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, LIGHT_ALMOST_HIT);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, RED_LIGHT_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, RED_LIGHT_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPE_CENTERED);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPE_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPE_DROPPED);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPELINE_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPELINE_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, GATE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, GATE_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, GATE_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BINS_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_DROPPED);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_CENTERED);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, MULTI_BIN_ANGLE);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DUCT_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DUCT_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DUCT_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DUCT_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DOWN_DUCT_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DOWN_DUCT_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, SAFE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, SAFE_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, SAFE_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, SAFE_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, TARGET_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, TARGET_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, TARGET_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, TARGET_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BARBED_WIRE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BARBED_WIRE_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BARBED_WIRE_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BARBED_WIRE_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, HEDGE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, HEDGE_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, HEDGE_DETECTOR_ON);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, HEDGE_DETECTOR_OFF);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, VELOCITY_UPDATE);

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS)

#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::vision::RedLightEvent>
RAM_VISION_REDLIGHTEVENT;

static ram::core::SpecificEventConverter<ram::vision::PipeEvent>
RAM_VISION_PIPEEVENT;

static ram::core::SpecificEventConverter<ram::vision::BinEvent>
RAM_VISION_BINEVENT;

static ram::core::SpecificEventConverter<ram::vision::DuctEvent>
RAM_VISION_DUCTEVENT;

static ram::core::SpecificEventConverter<ram::vision::SafeEvent>
RAM_VISION_SAFEEVENT;

static ram::core::SpecificEventConverter<ram::vision::TargetEvent>
RAM_VISION_TARGETEVENT;

static ram::core::SpecificEventConverter<ram::vision::BarbedWireEvent>
RAM_VISION_BARBED_WIREEVENT;

static ram::core::SpecificEventConverter<ram::vision::HedgeEvent>
RAM_VISION_HEDGEEVENT;

#endif // RAM_WITH_WRAPPERS

namespace ram {
namespace vision {

core::EventPtr ImageEvent::clone()
{
    ImageEventPtr event = ImageEventPtr(new ImageEvent());
    copyInto(event);
    // Not yet implemetned
    return event;
}

core::EventPtr RedLightEvent::clone()
{
    RedLightEventPtr event = RedLightEventPtr(new RedLightEvent());
    copyInto(event);
    event->azimuth = azimuth;
    event->elevation = elevation;
    event->range = range;
    event->x = x;
    event->y = y;
    event->pixCount = pixCount;
    event->color = color;
    return event;
}

core::EventPtr PipeEvent::clone()
{
    PipeEventPtr event = PipeEventPtr(new PipeEvent());
    copyInto(event);
    event->id = id;
    event->x = x;
    event->y = y;
    event->angle = angle;
    return event;
}

core::EventPtr BinEvent::clone()
{
    BinEventPtr event = BinEventPtr(new BinEvent());
    copyInto(event);
    event->id = id;
    event->x = x;
    event->y = y;
    event->symbol = symbol;
    event->angle = angle;
    return event;
}

core::EventPtr DuctEvent::clone()
{
    DuctEventPtr event = DuctEventPtr(new DuctEvent());
    copyInto(event);
    event->x = x;
    event->y = y;
    event->range = range;
    event->alignment = alignment;
    event->aligned = aligned;
    event->visible = visible;
    return event;
}

core::EventPtr SafeEvent::clone()
{
    SafeEventPtr event = SafeEventPtr(new SafeEvent());
    copyInto(event);
    event->x = x;
    event->y = y;
    return event;
}

core::EventPtr TargetEvent::clone()
{
    TargetEventPtr event = TargetEventPtr(new TargetEvent());
    copyInto(event);
    event->x = x;
    event->y = y;
    event->squareNess = squareNess;
    event->range = range;
    event->color = color;
    return event;
}

core::EventPtr BarbedWireEvent::clone()
{
    BarbedWireEventPtr event = BarbedWireEventPtr(new BarbedWireEvent());
    copyInto(event);
    event->topX = topX;
    event->topY = topY;
    event->topWidth = topWidth;
    event->bottomX = bottomX;
    event->bottomY = bottomY;
    event->bottomWidth = bottomWidth;
    return event;
}

core::EventPtr HedgeEvent::clone()
{
    HedgeEventPtr event = HedgeEventPtr(new HedgeEvent());
    copyInto(event);
    event->x = x;
    event->y = y;
    event->width = width;
    return event;
}


    
} // namespace vision
} // namespace ram
