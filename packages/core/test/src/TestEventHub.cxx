/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestEventHub.cxx
 */


// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

// Project Includes
#include "core/include/EventHub.h"
#include "core/include/EventPublisher.h"
#include "core/include/EventConnection.h"
#include "core/test/include/Reciever.h"

struct EventHubFixture {
    Reciever recv;
    ram::core::EventHubPtr eventHub;
    ram::core::EventPublisher publisherA;
    ram::core::EventPublisher publisherB;

    EventHubFixture() :
        eventHub(new ram::core::EventHub()),
        publisherA(eventHub),
        publisherB(eventHub)
    {}
};

TEST_FIXTURE(EventHubFixture, subscribe)
{
    Reciever recvB;

    // Subscribe to just events from publisherA
    ram::core::EventConnectionPtr connectionA =
        eventHub->subscribe("Type", &publisherA,
                            boost::bind(&Reciever::handler, &recv, _1));
    
    // Subscribe to just events from publisherB
    ram::core::EventConnectionPtr connectionB =
        eventHub->subscribe("Type", &publisherB,
                            boost::bind(&Reciever::handler, &recvB, _1));
    
    CHECK_EQUAL(0, recv.calls);
    CHECK_EQUAL(0, recvB.calls);    
    
    // Make sure messages from the first publisher gets through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisherA, recv.events[0]->sender);
    CHECK_EQUAL(0, recvB.calls);    
    
    // Make sure messages from the second publisher gets through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recvB.calls);
    CHECK_EQUAL("Type", recvB.events[0]->type);
    CHECK_EQUAL(&publisherB, recvB.events[0]->sender);
    CHECK_EQUAL(1, recv.calls);    
    
    // Now for disconnection
    connectionA->disconnect();
    
    // Make sure messages from the first publisher don't get through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(1, recvB.calls);

    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(2, recvB.calls);

    connectionB->disconnect();
    
    // Make sure messages from the second publisher don't get through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(2, recvB.calls);
}

TEST_FIXTURE(EventHubFixture, subscribeToType)
{
    ram::core::EventConnectionPtr connection =
        eventHub->subscribeToType("Type",
                                  boost::bind(&Reciever::handler, &recv, _1));
    CHECK_EQUAL(0, recv.calls);    
    
    // Make sure messages from the first publisher gets through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisherA, recv.events[0]->sender);
    
    // Make sure messages from the second publisher gets through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(2, recv.calls);
    CHECK_EQUAL("Type", recv.events[1]->type);
    CHECK_EQUAL(&publisherB, recv.events[1]->sender);

    publisherA.publish("AnotherType",
                       ram::core::EventPtr(new ram::core::Event()));
    publisherB.publish("AnotherType",
                       ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(2, recv.calls);
    
    // Now for disconnection
    connection->disconnect();
    
    // Make sure messages from the first publisher don't get through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(2, recv.calls);
    
    // Make sure messages from the second publisher don't get through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(2, recv.calls);
}

TEST_FIXTURE(EventHubFixture, subscribeToAll)
{
    ram::core::EventConnectionPtr connection =
        eventHub->subscribeToAll(boost::bind(&Reciever::handler, &recv, _1));
    CHECK_EQUAL(0, recv.calls);    
    
    // Make sure messages from the first publisher gets through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisherA, recv.events[0]->sender);
    
    // Make sure messages from the second publisher gets through
    publisherB.publish("Type2", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(2, recv.calls);
    CHECK_EQUAL("Type2", recv.events[1]->type);
    CHECK_EQUAL(&publisherB, recv.events[1]->sender);

    // Now for disconnection
    connection->disconnect();
    
    // Make sure messages from the first publisher don't get through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(2, recv.calls);
    
    // Make sure messages from the second publisher don't get through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(2, recv.calls);
}
