/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestEventPublisherRegistry.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/EventPublisherRegistry.h"
#include "core/include/EventPublisher.h"

using namespace ram;

SUITE(EventPublisherRegistry) {

TEST(AllInclusiveTest)
{
    // Create a publisher which is automatically registered
    core::EventPublisher* publisher =
        new core::EventPublisher(core::EventHubPtr(), "MyPublisher");

    // Make sure we can look it up
    CHECK_EQUAL(publisher,
                core::EventPublisherRegistry::lookupByName("MyPublisher"));

    // Make sure it goes away after the EventPublisher is destroy
    delete publisher;
    CHECK_EQUAL((core::EventPublisher*)0,
                core::EventPublisherRegistry::lookupByName("MyPublisher"));
}

} // SUITE(EventPublisherRegistry)
