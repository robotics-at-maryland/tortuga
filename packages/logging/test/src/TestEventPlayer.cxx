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
#include <string>

#include <unistd.h>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "logging/include/EventPlayer.h"
#include "logging/include/Serialize.h"

#include "core/include/Events.h"
#include "core/include/EventHub.h"
#include "core/include/Logging.h"

#include "math/test/include/MathChecks.h"

// Bring Vector3, and other math classes into scope
using namespace ram::math;
using namespace ram;

namespace bf = boost::filesystem;

class TestEventPlayer : public logging::EventPlayer
{
    double timeOfDay;
    double sleptTime;
    
protected:
    virtual double getTimeOfDay()
    {
        return timeOfDay;
    }
    
    virtual void eventSleep(double seconds)
    {
        sleptTime = seconds;
    }
};

SUITE(EventPlayer) {
    
typedef std::vector<ram::core::EventPtr> EventList;

struct Fixture
{
    Fixture() :
        eventHub(new core::EventHub()),
        filename(""),
        config("")
    {
        std::stringstream ss;
        ss << "EventPlayerTestLog" << "_" << getpid() << ".log";
        filename = ss.str();

        std::stringstream ss2;
        ss2 << "{ 'fileName' : '../../" << filename << "'}";
        config = ss2.str();

        eventHub->subscribeToAll(boost::bind(&Fixture::handleEvent, this, _1));
    }

    ~Fixture()
    {
        // Remove movie file
        bf::path logFile(filename);
	if (bf::exists(logFile))
	    bf::remove(logFile);
    }

    void handleEvent(core::EventPtr event)
    {
        publishedEvents.push_back(event);
    }

    void insertEvent(core::Event::EventType type,
                     core::EventPublisher* sender,
                     double timeStamp,
                     EventList& events,
                     boost::archive::text_oarchive& oa)
    {
        core::EventPtr event(new ram::core::Event());
        event->type = type;
        event->sender = sender;
        event->timeStamp = timeStamp;
        events.push_back(event);

        oa << event;
    }
    
    EventList writeOutTestEvents()
    {    
        std::ofstream ofs;
        ofs.open(filename.c_str(), std::ios::out | std::ios::app |
                 std::ios::binary);

        boost::archive::text_oarchive oa(ofs);
        ram::logging::registerTypes(oa);

        EventList events;
        insertEvent("0", 0, 0, events, oa);
        insertEvent("1", 0, 0.3, events, oa);
        insertEvent("2", 0, 0.8, events, oa);
        insertEvent("3", 0, 1.5, events, oa);

        ofs.close();
        
        return events;
    }

    EventList publishedEvents;
    core::EventHubPtr eventHub;
    std::string filename;
    std::string config;
};

TEST_FIXTURE(Fixture, BasicPlayer)
{
    // The events we wrote to the file
    EventList events = writeOutTestEvents();
    
    // Create our logger
    logging::EventPlayer* player =
        new logging::EventPlayer(core::ConfigNode::fromString(config),
                                 boost::assign::list_of(eventHub));

    // Go through and update to make sure all the events are published
    for (unsigned int i = 0; i < events.size(); ++i)
        player->update(0);

    // Check to make sure we got the right number of events
    CHECK_EQUAL(events.size(), publishedEvents.size());

    for (unsigned int i = 0; i < events.size(); ++i)
    {
        CHECK_EQUAL(events[i]->type, publishedEvents[i]->type);
        CHECK_EQUAL(events[i]->sender, publishedEvents[i]->sender);
        CHECK_EQUAL(events[i]->timeStamp, publishedEvents[i]->timeStamp);
    }
    
/*    
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
        boost::dynamic_pointer_cast<core::StringEvent>(results[1])->string);*/
}

} // SUITE(EventLogger)
