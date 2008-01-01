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
#include <boost/bind.hpp>

// Project Includes
#include "core/test/include/MockSubsystem.h"
#include "core/test/include/Reciever.h"
#include "core/include/EventHub.h"

struct MockSub1 : public MockSubsystem 
{
	MockSub1(ram::core::ConfigNode config) : 
		MockSubsystem(config, ram::core::SubsystemList()) {}
};

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
	MockSub1* mock1 = new MockSub1(config);
	
	config.set("name", "Mock2");
	MockSub2* mock2 = new MockSub2(config);
	
	config.set("name", "Mock3");
	MockSub3* mock3 = new MockSub3(config);
	
	ram::core::SubsystemList subsystemList = boost::assign::list_of(
            ram::core::SubsystemPtr(mock1))
            (ram::core::SubsystemPtr(mock2))
            (ram::core::SubsystemPtr(mock3));

        ram::core::SubsystemPtr sub2 =
            ram::core::Subsystem::getSubsystemOfType<MockSub2>(subsystemList);
	CHECK_EQUAL(mock2, sub2.get());
	CHECK_EQUAL("Mock2", sub2->getName());

        ram::core::SubsystemPtr sub3 =
            ram::core::Subsystem::getSubsystemOfType<MockSub3>(subsystemList);
	CHECK_EQUAL(mock3, sub3.get());
	CHECK_EQUAL("Mock3", sub3->getName());

        ram::core::SubsystemPtr sub =
            ram::core::Subsystem::getSubsystemOfType<MockSub1>(subsystemList);
	CHECK_EQUAL(mock1, sub.get());
	CHECK_EQUAL("Mock1", sub->getName());
}

TEST(Events)
{
    // Create a subsystem and give it an event hub
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
	            "{ 'name' : 'Mock1' }"));
    ram::core::EventHubPtr eventHub(new ram::core::EventHub());
    ram::core::SubsystemList subsystemList = boost::assign::list_of(eventHub);
    MockSubsystem mock(config, subsystemList);

    // Test Normal Events
    Reciever recvA;
    mock.subscribe("Type", boost::bind(&Reciever::handler, &recvA, _1));

    ram::core::EventPtr eventA(new ram::core::Event());
    mock.publish("Type", eventA);

    CHECK_EQUAL(1, recvA.calls);
    CHECK_EQUAL(eventA, recvA.events[0]);
    CHECK_EQUAL("Type", recvA.events[0]->type);
    CHECK_EQUAL(&mock, recvA.events[0]->sender);

    // Make sure the event hub is properly working
    Reciever recvB;
    eventHub->subscribeToType("Type",
                              boost::bind(&Reciever::handler, &recvB, _1));

    ram::core::EventPtr eventB(new ram::core::Event());
    mock.publish("Type", eventB);

    CHECK_EQUAL(1, recvB.calls);
    CHECK_EQUAL(eventB, recvB.events[0]);
    CHECK_EQUAL("Type", recvB.events[0]->type);
    CHECK_EQUAL(&mock, recvB.events[0]->sender);
}
