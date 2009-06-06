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
#include "core/include/QueuedEventHub.h"
#include "core/include/EventPublisher.h"
#include "core/include/EventConnection.h"
#include "core/test/include/Reciever.h"

struct QueuedEventHubFixture {
    Reciever recv;
    ram::core::EventHubPtr eventHub;
    ram::core::QueuedEventHubPtr queuedEventHub;
    ram::core::EventPublisher publisherA;
    ram::core::EventPublisher publisherB;

    QueuedEventHubFixture() :
        eventHub(new ram::core::EventHub()),
        queuedEventHub(new ram::core::QueuedEventHub(eventHub)),
        publisherA(eventHub),
        publisherB(eventHub)
    {}
};

TEST_FIXTURE(QueuedEventHubFixture, subscribe)
{
    Reciever recvB;

    // Subscribe to just events from publisherA
    ram::core::EventConnectionPtr connectionA =
        queuedEventHub->subscribe("Type", &publisherA,
                                  boost::bind(&Reciever::handler, &recv, _1));
    
    // Subscribe to just events from publisherB
    ram::core::EventConnectionPtr connectionB =
        queuedEventHub->subscribe("Type", &publisherB,
                                  boost::bind(&Reciever::handler, &recvB, _1));
    
    CHECK_EQUAL(0, recv.calls);
    CHECK_EQUAL(0, recvB.calls);    
    
    // Make sure the message is queued
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(0, recv.calls);
    CHECK_EQUAL(0, recvB.calls);
    queuedEventHub->publishEvents();
    
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisherA, recv.events[0]->sender);
    CHECK_EQUAL(0, recvB.calls);    
    
    // Make sure messages from the second publisher gets through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(0, recvB.calls);
    queuedEventHub->publishEvents();
    
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
          
    queuedEventHub->publishEvents();
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(1, recvB.calls);

    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(1, recvB.calls);
    
    queuedEventHub->publishEvents();
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(2, recvB.calls);

    connectionB->disconnect();
    
    // Make sure messages from the second publisher don't get through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));

    queuedEventHub->publishEvents();
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(2, recvB.calls);
}


TEST_FIXTURE(QueuedEventHubFixture, subscribeToType)
{
    ram::core::EventConnectionPtr connection =
        queuedEventHub->subscribeToType("Type",
            boost::bind(&Reciever::handler, &recv, _1));
    CHECK_EQUAL(0, recv.calls);    
    
    // Make sure messages from the first publisher gets through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(0, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisherA, recv.events[0]->sender);
    
    // Make sure messages from the second publisher gets through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
    CHECK_EQUAL("Type", recv.events[1]->type);
    CHECK_EQUAL(&publisherB, recv.events[1]->sender);

    publisherA.publish("AnotherType",
                       ram::core::EventPtr(new ram::core::Event()));
    publisherB.publish("AnotherType",
                       ram::core::EventPtr(new ram::core::Event()));
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
    
    // Now for disconnection
    connection->disconnect();
    
    // Make sure messages from the first publisher don't get through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
    
    // Make sure messages from the second publisher don't get through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
}

TEST_FIXTURE(QueuedEventHubFixture, subscribeToAll)
{
    ram::core::EventConnectionPtr connection =
        queuedEventHub->subscribeToAll(boost::bind(&Reciever::handler, &recv,
                                                   _1));
    CHECK_EQUAL(0, recv.calls);    
    
    // Make sure messages from the first publisher gets through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(0, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisherA, recv.events[0]->sender);
    
    // Make sure messages from the second publisher gets through
    publisherB.publish("Type2", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
    CHECK_EQUAL("Type2", recv.events[1]->type);
    CHECK_EQUAL(&publisherB, recv.events[1]->sender);

    // Now for disconnection
    connection->disconnect();
    
    // Make sure messages from the first publisher don't get through
    publisherA.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
    
    // Make sure messages from the second publisher don't get through
    publisherB.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
}

TEST_FIXTURE(QueuedEventHubFixture, multipleOrder)
{
    queuedEventHub->subscribeToType("A",
        boost::bind(&Reciever::handler, &recv, _1));
    queuedEventHub->subscribeToType("B",
        boost::bind(&Reciever::handler, &recv, _1));
    queuedEventHub->subscribeToType("C",
        boost::bind(&Reciever::handler, &recv, _1));
    queuedEventHub->subscribeToType("D",
        boost::bind(&Reciever::handler, &recv, _1));
    queuedEventHub->subscribeToType("E",
        boost::bind(&Reciever::handler, &recv, _1));
    CHECK_EQUAL(0, recv.calls);

    publisherA.publish("A", ram::core::EventPtr(new ram::core::Event()));
    publisherA.publish("B", ram::core::EventPtr(new ram::core::Event()));
    publisherB.publish("C", ram::core::EventPtr(new ram::core::Event()));
    publisherB.publish("D", ram::core::EventPtr(new ram::core::Event()));
    publisherA.publish("E", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(0, recv.calls);

    queuedEventHub->publishEvents();
    CHECK_EQUAL(5, recv.calls);
    
    CHECK_EQUAL("A", recv.events[0]->type);
    CHECK_EQUAL(&publisherA, recv.events[0]->sender);
    
    CHECK_EQUAL("B", recv.events[1]->type);
    CHECK_EQUAL(&publisherA, recv.events[1]->sender);
    
    CHECK_EQUAL("C", recv.events[2]->type);
    CHECK_EQUAL(&publisherB, recv.events[2]->sender);

    CHECK_EQUAL("D", recv.events[3]->type);
    CHECK_EQUAL(&publisherB, recv.events[3]->sender);

    CHECK_EQUAL("E", recv.events[4]->type);
    CHECK_EQUAL(&publisherA, recv.events[4]->sender);
}

// Test EventHub::publish explicitly
TEST_FIXTURE(QueuedEventHubFixture, publichEventHub)
{
    ram::core::EventConnectionPtr connectionA =
        queuedEventHub->subscribeToType("Type",
                                  boost::bind(&Reciever::handler, &recv, _1));

    CHECK_EQUAL(0, recv.calls);

    // Make sure messages of the desired type get through
    ram::core::EventPtr event(new ram::core::Event());
    event->type = "Type";
    event->sender = &publisherB;
    queuedEventHub->publish(event);

    CHECK_EQUAL(0, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisherB, recv.events[0]->sender);

    connectionA->disconnect();

    // Make sure we get all messages as well
    ram::core::EventConnectionPtr connectionB =
        queuedEventHub->subscribeToAll(boost::bind(&Reciever::handler, &recv,
                                                   _1));

    event = ram::core::EventPtr(new ram::core::Event());
    event->type = "TypeB";
    event->sender = &publisherA;
    queuedEventHub->publish(event);

    CHECK_EQUAL(1, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
    CHECK_EQUAL("TypeB", recv.events[1]->type);
    CHECK_EQUAL(&publisherA, recv.events[1]->sender);
}

// Test EventPublisher::publish explicitly
TEST_FIXTURE(QueuedEventHubFixture, publichEventPublisher)
{
    ram::core::EventConnectionPtr connectionA =
        queuedEventHub->subscribeToType("Type",
                                  boost::bind(&Reciever::handler, &recv, _1));

    CHECK_EQUAL(0, recv.calls);

    // Make sure messages of the desired type get through
    queuedEventHub->publish("Type",
                            ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(0, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(queuedEventHub.get(), recv.events[0]->sender);

    connectionA->disconnect();

    // Make sure we get all messages as well
    ram::core::EventConnectionPtr connectionB =
        queuedEventHub->subscribeToAll(boost::bind(&Reciever::handler, &recv,
                                                   _1));

    queuedEventHub->publish("TypeB",
                            ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recv.calls);
    queuedEventHub->publishEvents();
    CHECK_EQUAL(2, recv.calls);
    CHECK_EQUAL("TypeB", recv.events[1]->type);
    CHECK_EQUAL(queuedEventHub.get(), recv.events[1]->sender);

    connectionB->disconnect();
}
