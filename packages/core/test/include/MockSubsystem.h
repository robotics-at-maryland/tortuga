/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/include/MockSubsystem.h
 */

#ifndef RAM_CORE_TEST_MOCKSUBSYSTEM_11_18_2007
#define RAM_CORE_TEST_MOCKSUBSYSTEM_11_18_2007

#include <iostream>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/ConfigNode.h"

class MockSubsystem : public ram::core::Subsystem
{
public:
    MockSubsystem(ram::core::ConfigNode config_, 
    			  ram::core::SubsystemList dependents_) :
    	ram::core::Subsystem(config_["name"].asString()),
        config(config_),
        dependents(dependents_)
    {
    }

    ram::core::ConfigNode config;
    ram::core::SubsystemList dependents;
    
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_CORE_TEST_MOCKSUBSYSTEM_11_18_2007
