/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/logging/test/src/TestEventLogger.cxx
 */

// STD Includes
#include <sstream>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>
#include <boost/archive/text_oarchive.hpp>

// Project Includes
#include "logging/include/EventLogger.h"
#include "logging/include/Serialize.h"

#include "core/include/Events.h"
#include "core/include/EventHub.h"
#include "core/include/Logging.h"

#include "math/test/include/MathChecks.h"

// Bring Vector3, and other math classes into scope
using namespace ram::math;
using namespace ram;

SUITE(EventLogger) {

typedef std::vector<ram::core::EventPtr> EventList;

struct Fixture
{
    Fixture() :
      eventHub(new core::EventHub())
    {
    }

    core::EventHubPtr eventHub;

    EventList readBackAllEvents()
    {
        std::string fileName = "event.log";
        std::string filePath = (core::Logging::getLogDir() / fileName).string();
	std::ifstream ifs;
        ifs.open(filePath.c_str());

        // get length of file:
        ifs.seekg (0, std::ios::end);
        int fileLength = ifs.tellg();
        ifs.seekg (0, std::ios::beg);
        
        // Create the archive
        boost::archive::text_iarchive iar(ifs);
        ram::logging::registerTypes(iar);

	// Read in all the events
	EventList events;
	while (ifs.tellg() < fileLength)
	{
            // Read in the event
            ram::core::EventPtr event;
            iar >> event;
            events.push_back(event);
        }

        return events;
    }
};

TEST_FIXTURE(Fixture, BasicLogging)
{
    // Create our logger
    logging::EventLogger* logger = 
        new logging::EventLogger(core::ConfigNode::fromString("{}"),
				 boost::assign::list_of(eventHub));

    // Where the results go
    EventList expected;
    
    // A named publisher so that it can properly be recorded
    ram::core::EventPublisher publisher(eventHub, "PublisherName");
    ram::core::EventPublisher unNamedPublisher(eventHub);
    
    // Send an event through the publisher
    core::StringEventPtr event1(new ram::core::StringEvent());
    event1->string = "Test";
    publisher.publish("Bob", event1);
    expected.push_back(event1);

    core::StringEventPtr event2(new ram::core::StringEvent());
    event2->string = "My Other String";
    unNamedPublisher.publish("Other", event2);
    expected.push_back(event2);

    // Close down the logger and force the events to disk
    delete logger;

    // Compare
    EventList results =  readBackAllEvents();
    CHECK_EQUAL(expected.size(), results.size());

    CHECK_EQUAL(expected[0]->type, results[0]->type);
    CHECK_EQUAL(expected[0]->timeStamp, results[0]->timeStamp);
    CHECK_EQUAL(expected[0]->sender, results[0]->sender);
    CHECK_EQUAL(
        boost::dynamic_pointer_cast<core::StringEvent>(expected[0])->string,
        boost::dynamic_pointer_cast<core::StringEvent>(results[0])->string);

    CHECK_EQUAL(expected[1]->type, results[1]->type);
    CHECK_EQUAL(expected[1]->timeStamp, results[1]->timeStamp);
    CHECK(!results[1]->sender);
    CHECK_EQUAL(
        boost::dynamic_pointer_cast<core::StringEvent>(expected[1])->string,
        boost::dynamic_pointer_cast<core::StringEvent>(results[1])->string);
}

} // SUITE(EventLogger)
