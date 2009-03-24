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
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPE_CENTERED);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, PIPE_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, GATE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_DROPPED);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BIN_CENTERED);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, MULTI_BIN_ANGLE);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DUCT_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DUCT_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DOWN_DUCT_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, DOWN_DUCT_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, SAFE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, SAFE_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, TARGET_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, TARGET_LOST);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BARBED_WIRE_FOUND);
RAM_CORE_EVENT_TYPE(ram::vision::EventType, BARBED_WIRE_LOST);

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

#endif // RAM_WITH_WRAPPERS
