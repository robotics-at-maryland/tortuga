/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/SubsystemMaker.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>

// Project Includes
#include "core/test/include/MockSubsystem.h"

struct MockSub2 : public MockSubsystem 
{
	MockSub2(ram::core::ConfigNode config) : 
		MockSubsystem(config, ram::core::SubsystemList()) {}
};

struct MockSub3 : public MockSubsystem 
{
	MockSub3(ram::core::ConfigNode config) : 
		MockSubsystem(config, ram::core::SubsystemList()) {}
};

TEST(getName)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'name' : 'Mock' }"));
	
	MockSubsystem mock(config, ram::core::SubsystemList());
	CHECK_EQUAL("Mock", mock.getName());
}

TEST(GetSubsystem)
{
	ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
	            "{ 'name' : 'Mock1' }"));
	MockSubsystem mock1(config, ram::core::SubsystemList());
	
	config.set("name", "Mock2");
	MockSub2 mock2(config);
	
	config.set("name", "Mock3");
	MockSub3 mock3(config);
	
	ram::core::SubsystemList subsystemList = 
		boost::assign::list_of(&mock1)(&mock2)(&mock3);
	
	ram::core::Subsystem* sub = 
		ram::core::Subsystem::getSubsystemOfType<MockSub2>(subsystemList);
	CHECK_EQUAL(&mock2, sub);
	CHECK_EQUAL("Mock2", sub->getName());
	
	sub = ram::core::Subsystem::getSubsystemOfType<MockSub3>(subsystemList);
	CHECK_EQUAL(&mock3, sub);
	CHECK_EQUAL("Mock3", sub->getName());
	
	sub = ram::core::Subsystem::getSubsystemOfType<MockSubsystem>(subsystemList);
	CHECK_EQUAL(&mock1, sub);
	CHECK_EQUAL("Mock1", sub->getName());
}
