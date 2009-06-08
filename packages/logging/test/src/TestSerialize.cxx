/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 John Edmonds <pocketcookies2@gmail.com>
 * All rights reserved.
 *
 * Author: John Edmonds <pocketcookies2@gmail.com>
 * File:  packages/logging/test/src/TestSerialize.cxx
 */

// STD Includes
#include <sstream>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "logging/include/Serialize.h"

#include "vision/include/Events.h"

#include "math/test/include/MathChecks.h"

// Bring Vector3, and other math classes into scope
using namespace ram::math;



SUITE(Serialization) {

struct Fixture
{
    std::ostringstream ofs;
    std::istringstream ifs;

    void writeOut(ram::core::EventPtr event)
    {
        boost::archive::text_oarchive oa(ofs);
        ram::logging::registerTypes(oa);
        oa << event;
    }

    ram::core::EventPtr readBack()
    {
        // Create the archive
        ifs.str(ofs.str());
        boost::archive::text_iarchive iar(ifs);
        ram::logging::registerTypes(iar);

        // Read in the event
        ram::core::EventPtr event;        
        iar >> event;

        return event;
    }

    template<class T>
    boost::shared_ptr<T> serializeDeSerialize(ram::core::EventPtr event)
    {
        writeOut(event);
        return boost::dynamic_pointer_cast<T>(readBack());
    }
};

TEST_FIXTURE(Fixture, Event)
{
    // A named publisher so that it can properly be recorded
    ram::core::EventPublisher publisher(ram::core::EventHubPtr(),
                                        "PublisherName");
    
    // Create the event and fill it with data
    ram::core::EventPtr event(new ram::core::Event());
    event->type = "Bob";
    event->sender = &publisher;
    // timestamp already filled by the constructor

    // Write and read back the event
    ram::core::EventPtr result = serializeDeSerialize<ram::core::Event>(event);
    
    // Check to make sure everything made it back
    CHECK_EQUAL(event->type, result->type);
    CHECK_EQUAL(event->sender, result->sender);
    CHECK_EQUAL(event->timeStamp, result->timeStamp);
}

TEST_FIXTURE(Fixture, RedLightEvent)
{
    // Create the event and load it with data
  ram::vision::RedLightEventPtr redLightEvent(
      new ram::vision::RedLightEvent(0, 0));

  redLightEvent->x = 1.2;
  redLightEvent->y = 2.7;
  redLightEvent->azimuth = Degree(3.2);
  redLightEvent->elevation = Degree(3.3);
  redLightEvent->range = 3.4;
  redLightEvent->pixCount = 3;

  // Write out and read back the event, then downcast to the proper type
  ram::vision::RedLightEventPtr result(
      serializeDeSerialize<ram::vision::RedLightEvent>(redLightEvent));
  
  // Check the data
  CHECK_EQUAL(result->x, redLightEvent->x);
  CHECK_EQUAL(result->y, redLightEvent->y);
  CHECK_EQUAL(result->azimuth, redLightEvent->azimuth);
  CHECK_EQUAL(result->elevation, redLightEvent->elevation);
  CHECK_EQUAL(result->range, redLightEvent->range);
  CHECK_EQUAL(result->pixCount, redLightEvent->pixCount);
}

} // SUITE(Serialization)
