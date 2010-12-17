/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/test/src/TestNetworkEvents.cxx
 */

#include <iostream>

// STD Includes
#include <time.h>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/date_time/posix_time/posix_time.hpp>

// Project Includes
#include "core/include/EventHub.h"
#include "core/include/Events.h"
#include "core/include/EventConnection.h"
#include "network/include/NetworkPublisher.h"
#include "network/include/NetworkHub.h"

using namespace ram;

static std::string PUBLISHER_CFG = "{ 'port' : 48123 }";

struct NetworkFixture
{
    NetworkFixture()
        : eventHub(new core::EventHub())
        , publisher(core::ConfigNode::fromString(PUBLISHER_CFG), eventHub)
    {
        // Wait for the publisher to start up
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));

        networkHub = new network::NetworkHub("NetworkHub", "localhost", 48123);
        conn = networkHub->subscribeToType(
            "UPDATE", boost::bind(&NetworkFixture::handler, this, _1));
    }

    ~NetworkFixture()
    {
        conn->disconnect();
        delete networkHub;
    }

    void handler(core::EventPtr event)
    {
        lastEvent = event;
    }

    core::EventHubPtr eventHub;
    network::NetworkPublisher publisher;
    network::NetworkHub *networkHub;

    core::EventConnectionPtr conn;
    core::EventPtr lastEvent;
};

TEST_FIXTURE(NetworkFixture, publishEvent)
{
    core::StringEventPtr expected(new core::StringEvent());
    expected->string = "hello, world";
    eventHub->publish("UPDATE", expected);

    // Wait for the network event to get sent (asynchronous operation)
    boost::this_thread::sleep(boost::posix_time::seconds(1));

    core::StringEventPtr actual =
        boost::dynamic_pointer_cast<core::StringEvent>(lastEvent);
    CHECK(actual);
    CHECK_EQUAL(expected->type, actual->type);
    CHECK_EQUAL(expected->timeStamp, actual->timeStamp);
    CHECK_EQUAL(expected->string, actual->string);
}
