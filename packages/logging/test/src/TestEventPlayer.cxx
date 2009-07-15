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
public:
    TestEventPlayer(core::ConfigNode config, core::SubsystemList deps,
                    double startTime) :
        logging::EventPlayer(config, deps),
        timeOfDay(startTime),
        sleptTime(-1)
    {
        m_startTime = startTime;
    }
    
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
        ss2 << "{ 'fileName' : '" << filename << "'}";
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
    
    EventList writeOutTestEvents(double startTime = 0.0)
    {    
        std::ofstream ofs;
        ofs.open(filename.c_str(), std::ios::out | std::ios::app |
                 std::ios::binary);

        boost::archive::text_oarchive oa(ofs);
        ram::logging::registerTypes(oa);

        EventList events;
        insertEvent("0", 0, startTime + 0, events, oa);
        insertEvent("1", 0, startTime + 0.3, events, oa);
        insertEvent("2", 0, startTime + 0.8, events, oa);
        insertEvent("3", 0, startTime + 1.5, events, oa);

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
    TestEventPlayer* player =
        new TestEventPlayer(core::ConfigNode::fromString(config),
                            boost::assign::list_of(eventHub), 0);

    // Go through and update to make sure all the events are published
    for (unsigned int i = 0; i < events.size(); ++i)
    {
        player->update(0);
        // Make sure the current time updates properly
        CHECK_EQUAL(events[i]->timeStamp, player->currentTime());
    }

    // Check to make sure we got the right number of events
    CHECK_EQUAL(events.size(), publishedEvents.size());

    for (unsigned int i = 0; i < events.size(); ++i)
    {
        CHECK_EQUAL(events[i]->type, publishedEvents[i]->type);
        CHECK_EQUAL(events[i]->sender, publishedEvents[i]->sender);
        CHECK_EQUAL(events[i]->timeStamp, publishedEvents[i]->timeStamp);
    }
}

TEST_FIXTURE(Fixture, Sleeping)
{
    static const double RECORDED_TIME = 12;
    static const double PLAYBACK_TIME = 124;
    static const double OFFSET = PLAYBACK_TIME - RECORDED_TIME;
    
    // The events we wrote to the file (starting at 12 seconds)
    EventList events = writeOutTestEvents(RECORDED_TIME);
    
    // Create our logger
    TestEventPlayer* player =
        new TestEventPlayer(core::ConfigNode::fromString(config),
                            boost::assign::list_of(eventHub),
                            PLAYBACK_TIME);

    
    // Do the first update
    player->update(0);
    CHECK_EQUAL(-1, player->sleptTime);
    CHECK_EQUAL(events[0]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[0]->type, publishedEvents[0]->type);
    CHECK_EQUAL(events[0]->sender, publishedEvents[0]->sender);
    CHECK_CLOSE(events[0]->timeStamp + OFFSET,
                publishedEvents[0]->timeStamp, 0.0001);

    // The next update (with no time advacement)
    player->update(0);
    CHECK_CLOSE(0.3, player->sleptTime, 0.0001);
    CHECK_EQUAL(events[1]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[1]->type, publishedEvents[1]->type);
    CHECK_EQUAL(events[1]->sender, publishedEvents[1]->sender);
    CHECK_CLOSE(events[1]->timeStamp + OFFSET,
                publishedEvents[1]->timeStamp, 0.0001);

    // Now another update that should have no sleep in it
    player->timeOfDay = PLAYBACK_TIME + 1;
    player->sleptTime = -1;
    player->update(0);

    CHECK_CLOSE(-1, player->sleptTime, 0.0001);
    CHECK_EQUAL(events[2]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[2]->type, publishedEvents[2]->type);
    CHECK_EQUAL(events[2]->sender, publishedEvents[2]->sender);
    CHECK_CLOSE(events[2]->timeStamp + OFFSET,
                publishedEvents[2]->timeStamp, 0.0001);

}

} // SUITE(EventLogger)
