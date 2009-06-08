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

// Project Includes
#include "logging/include/EventLogger.h"

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
      /*        std::string fileName = config["fileName"].asString("event.log");
        std::string filePath = (core::Logging::getLogDir() / fileName).string();
	std::ifstream ifs(filePath.c_str());

        // Create the archive
        boost::archive::text_iarchive iar(ifs);
        ram::logging::registerTypes(iar);*/

	// Read in all the events
	EventList events;
	/*	while (!ifs.eof())
	{
	  // Read in the event
	  ram::core::EventPtr event;
	  iar >> event;
	  events.push_bask(event);
	  }*/

        return events;
    }
};

TEST_FIXTURE(Fixture, BasicLogging)
{
    // Create our logger
  /*    logging::EventLogger* logger = 
        new logging::EventLogger(core::ConfigNode::fromString("{}"),
				 boost::assign::list_of(eventHub));

    // A named publisher so that it can properly be recorded
    ram::core::EventPublisher publisher(eventHub, "PublisherName");
    ram::core::EventPublisher unNamedPublisher(eventHub);
    
    // Send an event through the publisher
    core::StringEventPtr event1(new ram::core::StringEvent());
    publisher.publish("Bob", event1);*/

    // Write and read back the event
    //    ram::core::EventPtr result = serializeDeSerialize<ram::core::Event>(event);
    
    // Check to make sure everything made it back
    //    CHECK_EQUAL(event->type, result->type);
    //    CHECK_EQUAL(event->sender, result->sender);
    //    CHECK_EQUAL(event->timeStamp, result->timeStamp);
}

} // SUITE(EventLogger)
