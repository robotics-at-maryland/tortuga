/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/test/src/TestEvent.cxx
 */

// STD Includes
#include <string.h>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/Event.h"
#include "core/include/Events.h"

using namespace ram;

TEST(Clone)
{
    // Original event
    core::StringEventPtr original(new core::StringEvent());
    original->type = "greeting";
    original->sender = NULL;
    original->timeStamp = 2.1203;
    original->string = "hello, world";

    // Clone the event
    core::EventPtr cloned(original->clone());

    // Check that the contents are the exact same
    CHECK_EQUAL(original->type, cloned->type);
    CHECK_EQUAL(original->sender, cloned->sender);
    CHECK_EQUAL(original->timeStamp, cloned->timeStamp);
    CHECK_EQUAL(original->string,
                boost::dynamic_pointer_cast<core::StringEvent>(cloned)->string);
    CHECK_EQUAL(0, memcmp(original.get(), cloned.get(),
                          sizeof(core::StringEvent)));

    // Clone a second object
    core::StringEventPtr original2(new core::StringEvent());
    original2->type = "greeting";
    original2->sender = NULL;
    original2->timeStamp = 5.291;
    original2->string = "goodbye";

    // Clone the event
    core::EventPtr cloned2(original2->clone());

    // Check that the contents are the exact same
    CHECK_EQUAL(original2->type, cloned2->type);
    CHECK_EQUAL(original2->sender, cloned2->sender);
    CHECK_EQUAL(original2->timeStamp, cloned2->timeStamp);
    CHECK_EQUAL(original2->string,
                boost::dynamic_pointer_cast<core::StringEvent>(cloned2)->string);
    CHECK_EQUAL(0, memcmp(original2.get(), cloned2.get(),
                          sizeof(core::StringEvent)));
}
