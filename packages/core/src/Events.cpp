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

// This section is only needed when we are compiling the wrappers
// This registers converters to work around some issues with Boost.Python
// automatic down casting
#if defined(RAM_WITH_WRAPPERS)

#include "core/include/Events.h"
#include "core/include/EventConverter.h"

static ram::core::SpecificEventConverter<ram::core::StringEvent>
RAM_MATH_STRINGEVENT;

#endif // RAM_WITH_WRAPPERS
