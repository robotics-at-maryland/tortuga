/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestSubsystemMaker.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/SubsystemMaker.h"
#include "core/test/include/MockSubsystem.h"

// arguments are (type name, registered name)
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(MockSubsystem, MockSubsystem);

TEST(SubsystemMaker)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'name' : 'Mock', 'type' : 'MockSubsystem' }"));
    ram::core::SubsystemList deps;
    deps.insert(ram::core::SubsystemPtr());
    
    ram::core::SubsystemPtr subsystem =
        ram::core::SubsystemMaker::newObject(std::make_pair(config, deps));
    MockSubsystem* mockSubsystem =
        dynamic_cast<MockSubsystem*>(subsystem.get());
    
    CHECK(mockSubsystem);
    CHECK_EQUAL("Mock", subsystem->getName());
    CHECK_EQUAL(1u, mockSubsystem->dependents.size());
}


