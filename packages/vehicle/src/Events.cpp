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

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS)

#include "vehicle/include/Events.h"
#include "core/include/EventConverter.h"

/*static ram::core::SpecificEventConverter<ram::vehicle::PowerSourceEvent>
RAM_VEHICLE_POWERSOURCE_EVENT;

static ram::core::SpecificEventConverter<ram::vehicle::TempSensorEvent>
RAM_VEHICLE_TEMPSENSOR_EVENT;*/

#endif // RAM_WITH_WRAPPERS
