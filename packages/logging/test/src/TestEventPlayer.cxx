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
#include "core/include/TimeVal.h"
#include "core/include/Feature.h"

#ifdef RAM_WITH_MATH
#  include "math/include/Events.h"
#endif // RAM_WITH_MATH

#include "math/test/include/MathChecks.h"

// Bring Vector3, and other math classes into scope
using namespace ram::math;
using namespace ram;

namespace bf = boost::filesystem;

// static boost::filesystem::path getRefrencesDir()
// {
//     boost::filesystem::path root(getenv("RAM_SVN_DIR"));
//     return root / "packages" / "logging" / "test" / "data";
// }

class TestEventPlayer : public logging::EventPlayer
{
public:
    TestEventPlayer(core::ConfigNode config, core::SubsystemList deps,
                    double startTime) :
        logging::EventPlayer(config, deps),
        timeOfDay(startTime),
        sleptTime(-1),
        backgroundCount(0),
        unbackgroundCount(0)
    {
        //m_startTime = startTime;
        startingTime = m_startTime;
        timeOfDay = startingTime;
    }
    
    double timeOfDay;
    double sleptTime;
    double startingTime;
    int backgroundCount;
    int unbackgroundCount;

    virtual void background(int interval)
    {
        backgroundCount++;
    }
    
    virtual void unbackground(bool join = false)
    {
        unbackgroundCount++;
    }
    
protected:
    virtual double getTimeOfDay()
    {
        return timeOfDay;
    }
    
    virtual void eventSleep(double seconds)
    {
        seconds += 0.00001; // To avoid floating point errors
        sleptTime += seconds;
        timeOfDay += seconds;
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
        
        eventHub->subscribeToType(logging::EventPlayer::START,
                                  boost::bind(&Fixture::handleStartEvent, this,
                                              _1));
        eventHub->subscribeToType(logging::EventPlayer::STOP,
                                  boost::bind(&Fixture::handleStopEvent, this,
                                              _1));
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
        bool stopEvent = event->type == logging::EventPlayer::STOP;
        bool startEvent = event->type == logging::EventPlayer::START;
        bool setupEvent = event->type == logging::EventPlayer::PLAYER_SETUP;
        bool updateEvent = event->type == logging::EventPlayer::PLAYER_UPDATE;
        // Don't record events the player itself generates
        if (!stopEvent && !startEvent && !setupEvent && !updateEvent)
            publishedEvents.push_back(event);
    }

    void handleStartEvent(core::EventPtr event)
    {
        startEvents.push_back(event);
    }

    void handleStopEvent(core::EventPtr event)
    {
        stopEvents.push_back(event);
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

        logging::writeEvent(event, oa);
    }
    
    EventList writeOutTestEvents(double startTime = 0.0)
    {    
        std::ofstream ofs;
        ofs.open(filename.c_str(), std::ios::out | std::ios::app |
                 std::ios::binary);

        boost::archive::text_oarchive oa(ofs);

        EventList events;
        insertEvent("0", 0, startTime + 0, events, oa);
        insertEvent("1", 0, startTime + 0.3, events, oa);
        insertEvent("2", 0, startTime + 0.8, events, oa);
        insertEvent("3", 0, startTime + 1.5, events, oa);

        // If math library enabled, test events from another archive
#ifdef RAM_WITH_MATH
        math::NumericEventPtr numberEvent(new math::NumericEvent());
        numberEvent->number = 5;
        numberEvent->type = "4";
        numberEvent->sender = 0;
        numberEvent->timeStamp = startTime + 2.2;
        events.push_back(numberEvent);
        logging::writeEvent(numberEvent, oa);
#endif // RAM_WITH_MATH

        ofs.close();
        
        return events;
    }

    EventList publishedEvents;
    EventList startEvents;
    EventList stopEvents;
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
        CHECK_CLOSE(events[i]->timeStamp, player->currentTime(), 0.0001);
    }

    // Check to make sure we got the right number of events
    CHECK_EQUAL(events.size(), publishedEvents.size());

    for (unsigned int i = 0; i < events.size(); ++i)
    {
        CHECK_EQUAL(events[i]->type, publishedEvents[i]->type);
        CHECK_EQUAL(events[i]->sender, publishedEvents[i]->sender);
        CHECK_CLOSE(events[i]->timeStamp + player->startingTime, publishedEvents[i]->timeStamp, 0.0001);
    }

#ifdef RAM_WITH_MATH
    // Check the contents of the last event
    math::NumericEventPtr numberEvent =
        boost::dynamic_pointer_cast<math::NumericEvent>(publishedEvents.back());
    CHECK(numberEvent);

    CHECK_EQUAL(5, numberEvent->number);
#endif // RAM_WITH_MATH

    delete player;
}

TEST_FIXTURE(Fixture, Sleeping)
{
    static const double RECORDED_TIME = 12;
    static const double PLAYBACK_TIME = 124;
    
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
    CHECK_CLOSE(events[0]->timeStamp - RECORDED_TIME + 
                player->startingTime,
                publishedEvents[0]->timeStamp, 0.0001);

    // The next update (with no time advacement)
    player->sleptTime = 0;
    player->update(0);
    CHECK_CLOSE(0.3, player->sleptTime, 0.0001);
    CHECK_EQUAL(events[1]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[1]->type, publishedEvents[1]->type);
    CHECK_EQUAL(events[1]->sender, publishedEvents[1]->sender);
    CHECK_CLOSE(events[1]->timeStamp - RECORDED_TIME + 
                player->startingTime, 
                publishedEvents[1]->timeStamp, 0.0001);

    // Now another update that should have no sleep in it
    player->timeOfDay = PLAYBACK_TIME + 1 + player->startingTime;
    player->sleptTime = -1;
    player->update(0);

    CHECK_CLOSE(-1, player->sleptTime, 0.0001);
    CHECK_EQUAL(events[2]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[2]->type, publishedEvents[2]->type);
    CHECK_EQUAL(events[2]->sender, publishedEvents[2]->sender);
    CHECK_CLOSE(events[2]->timeStamp - RECORDED_TIME + 
                player->startingTime,
                publishedEvents[2]->timeStamp, 0.0001);

    delete player;
}

TEST_FIXTURE(Fixture, StartStop)
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

    CHECK_EQUAL(0, player->unbackgroundCount);
    CHECK_EQUAL(0, player->backgroundCount);
    CHECK_EQUAL(0u, startEvents.size());
    CHECK_EQUAL(0u, stopEvents.size());

    
    // Do the first update
    player->update(0);
    CHECK_EQUAL(-1, player->sleptTime);
    CHECK_EQUAL(events[0]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[0]->type, publishedEvents[0]->type);
    CHECK_EQUAL(events[0]->sender, publishedEvents[0]->sender);
    CHECK_CLOSE(events[0]->timeStamp - RECORDED_TIME + 
                player->startingTime,
                publishedEvents[0]->timeStamp, 0.0001);

    // Now stop for 10 seconds
    player->stop();
    CHECK_EQUAL(1, player->unbackgroundCount);
    CHECK_EQUAL(0, player->backgroundCount);
    CHECK_EQUAL(0u, startEvents.size());
    CHECK_EQUAL(1u, stopEvents.size());
    
    player->timeOfDay = PLAYBACK_TIME + 10 + player->startingTime;
    
    player->start();
    CHECK_EQUAL(1, player->unbackgroundCount);
    CHECK_EQUAL(1, player->backgroundCount);
    CHECK_EQUAL(1u, startEvents.size());
    CHECK_EQUAL(1u, stopEvents.size());
    
    // The next update (with no time advacement) should still sleep
    player->sleptTime = 0;
    player->update(0);
    CHECK_CLOSE(0.3, player->sleptTime, 0.0001);
    CHECK_EQUAL(events[1]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[1]->type, publishedEvents[1]->type);
    CHECK_EQUAL(events[1]->sender, publishedEvents[1]->sender);
    CHECK_CLOSE(events[1]->timeStamp + OFFSET + 
                player->startingTime + 10,
                publishedEvents[1]->timeStamp, 0.0001);
    
    delete player;
}

TEST_FIXTURE(Fixture, TimeSeeking)
{
    static const double RECORDED_TIME = 12;
    static const double PLAYBACK_TIME = 124;
    
    // The events we wrote to the file (starting at 12 seconds)
    EventList events = writeOutTestEvents(RECORDED_TIME);
    
    // Create our logger
    TestEventPlayer* player =
        new TestEventPlayer(core::ConfigNode::fromString(config),
                            boost::assign::list_of(eventHub),
                            PLAYBACK_TIME);

    CHECK_EQUAL(0, player->unbackgroundCount);
    CHECK_EQUAL(0, player->backgroundCount);
    CHECK_EQUAL(0u, startEvents.size());
    CHECK_EQUAL(0u, stopEvents.size());

    
    // Do the first update
    player->update(0);
    CHECK_EQUAL(-1, player->sleptTime);
    CHECK_EQUAL(events[0]->timeStamp - RECORDED_TIME, player->currentTime());
    CHECK_EQUAL(events[0]->type, publishedEvents[0]->type);
    CHECK_EQUAL(events[0]->sender, publishedEvents[0]->sender);
    CHECK_CLOSE(events[0]->timeStamp - RECORDED_TIME + 
                player->startingTime,
                publishedEvents[0]->timeStamp, 0.0001);

    // Do the second update
    player->update(0);
    
    player->seekToTime(0.3);
    // See if we get the same event on the third update
    player->update(0);
    CHECK_EQUAL(publishedEvents[2]->timeStamp, publishedEvents[1]->timeStamp);
    CHECK_EQUAL(publishedEvents[2]->type, publishedEvents[1]->type);
    CHECK_EQUAL(publishedEvents[2]->sender, publishedEvents[1]->sender);
    
    delete player;
}

// TEST_FIXTURE(Fixture, ActualLog)
// {
//     static const double RECORDED_TIME = 1247642287.418153;
//     static const double PLAYBACK_TIME = core::TimeVal::timeOfDay().get_double();
//     static const double OFFSET = PLAYBACK_TIME - RECORDED_TIME;
    
//     // Config to read the file
//     bf::path filePath = getRefrencesDir() / "ref.log";
//     std::string logcfg = "{'fileName' :'" + filePath.string() + "'}";
    
//     // Create our logger
//     TestEventPlayer* player =
//         new TestEventPlayer(core::ConfigNode::fromString(logcfg),
//                             boost::assign::list_of(eventHub),
//                             PLAYBACK_TIME);

//     // Do the first update
//     player->update(0);
//     CHECK_EQUAL(-1, player->sleptTime);
//     CHECK_EQUAL(1247642287.418153 - RECORDED_TIME, player->currentTime());
// //    CHECK_EQUAL(events[0]->type, publishedEvents[0]->type);
// //    CHECK_EQUAL(events[0]->sender, publishedEvents[0]->sender);
//     CHECK_CLOSE(1247642287.418153 + OFFSET,
//                 publishedEvents[0]->timeStamp, 0.0001);

//     // Second update
//     player->sleptTime = 0;
//     double expectedSleep = 1247642287.4182119 - RECORDED_TIME;
//     player->update(0);
//     CHECK_EQUAL(expectedSleep, player->sleptTime);
//     CHECK_EQUAL(1247642287.4182119 - RECORDED_TIME, player->currentTime());
// //    CHECK_EQUAL(events[0]->type, publishedEvents[0]->type);
// //    CHECK_EQUAL(events[0]->sender, publishedEvents[0]->sender);
//     CHECK_CLOSE(1247642287.4182119 + OFFSET,
//                 publishedEvents[1]->timeStamp, 0.0001);

//     // Third update
//     player->sleptTime = 0;
//     expectedSleep = 1247642288.5074999 - RECORDED_TIME;
//     player->update(0);
//     CHECK_CLOSE(expectedSleep, player->sleptTime, 0.0001);
//     CHECK_EQUAL(1247642288.5074999 - RECORDED_TIME, player->currentTime());
// //    CHECK_EQUAL(events[0]->type, publishedEvents[0]->type);
// //    CHECK_EQUAL(events[0]->sender, publishedEvents[0]->sender);
//     CHECK_CLOSE(1247642288.5074999 + OFFSET,
//                 publishedEvents[2]->timeStamp, 0.0001);

//     // Fourth update (this one is a dummy update!, it should not actually
//     // publish anything)
// //    player->update(0);
// //    CHECK_EQUAL(3u, publishedEvents.size());

//     // Real Fourth Update
//     player->sleptTime = 0;
//     expectedSleep = 1247642288.507683 - RECORDED_TIME;
//     player->update(0);
//     // Not working for some reason?
// //    CHECK_CLOSE(expectedSleep, player->sleptTime, 0.0001);
// //    CHECK_EQUAL(1247642288.507683 - RECORDED_TIME, player->currentTime());
// //    CHECK_EQUAL(events[0]->type, publishedEvents[0]->type);
// //    CHECK_EQUAL(events[0]->sender, publishedEvents[0]->sender);
//     CHECK_CLOSE(1247642288.507683 + OFFSET,
//                 publishedEvents[3]->timeStamp, 0.001);

    
//     delete player;
// }

} // SUITE(EventLogger)
