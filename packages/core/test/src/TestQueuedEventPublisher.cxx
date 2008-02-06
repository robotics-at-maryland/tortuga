/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestEventPublisher.cxx
 */

// STD Includes
#include <string>
#include <vector>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

// Project Includes
#include "core/include/QueuedEventPublisher.h"
#include "core/include/EventConnection.h"
#include "core/test/include/Reciever.h"


struct QueuedEventPublisherFixture {
    Reciever recv;
    ram::core::EventPublisher publisher;
    ram::core::QueuedEventPublisher qpublisher;

    QueuedEventPublisherFixture() :
        qpublisher(&publisher)
        {};
};

TEST_FIXTURE(QueuedEventPublisherFixture, QueueSubscribe)
{
    qpublisher.subscribe("Type", boost::bind(&Reciever::handler, &recv, _1));
}

TEST_FIXTURE(QueuedEventPublisherFixture, QueuePublish)
{
    // Subscribe through the QueuedEventPublisher
    qpublisher.subscribe("Type", boost::bind(&Reciever::handler, &recv, _1));

    // Limit scope to make sure the queue properly holds events
    {
        ram::core::EventPtr event(new ram::core::Event());
        publisher.publish("Type", event);
    }

     // Make sure the message has not arrived
    CHECK_EQUAL(0, recv.calls);

    // Release the message
    qpublisher.publishEvents();

    // Now make sure the message got through
    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisher, recv.events[0]->sender);
}

TEST_FIXTURE(QueuedEventPublisherFixture, disconnect)
{
    ram::core::EventConnectionPtr connection(
        qpublisher.subscribe("Type", boost::bind(&Reciever::handler, &recv,
                                                 _1)) );

    // Standard publish
    publisher.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(0, recv.calls);

    // Unsubscribe then publish
    connection->disconnect();
    publisher.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    qpublisher.publishEvents();
    CHECK_EQUAL(0, recv.calls);
}

TEST_FIXTURE(QueuedEventPublisherFixture, MultipleSubscribers)
{
    // Bind Many
    ram::core::EventConnectionPtr connA =
        qpublisher.subscribe("Type", boost::bind(&Reciever::handler, &recv,
                                                 _1));
    ram::core::EventConnectionPtr connB =
        qpublisher.subscribe("Type", boost::bind(&Reciever::handler, &recv,
                                                 _1));

    {
        ram::core::EventPtr event(new ram::core::Event());
        publisher.publish("Type", event);
    }

    CHECK_EQUAL(0, recv.calls);

    qpublisher.publishEvents();
    qpublisher.publishEvents();
    CHECK_EQUAL(2, recv.calls);

    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisher, recv.events[0]->sender);
    CHECK_EQUAL("Type", recv.events[1]->type);
    CHECK_EQUAL(&publisher, recv.events[1]->sender);

    // Remove one
    connA->disconnect();
    
    publisher.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    qpublisher.publishEvents();
    CHECK_EQUAL(3, recv.calls);

    // Both gone
    connB->disconnect();

    publisher.publish("Type", ram::core::EventPtr(new ram::core::Event()));
    qpublisher.publishEvents();
    CHECK_EQUAL(3, recv.calls);
}

TEST_FIXTURE(QueuedEventPublisherFixture, MultipleEventTypes)
{
    Reciever recvA;
    Reciever recvB;

    // Bind Many
    ram::core::EventConnectionPtr connA =
        qpublisher.subscribe("TypeA", boost::bind(&Reciever::handler, &recvA,
                                                  _1));
    ram::core::EventConnectionPtr connB =
        qpublisher.subscribe("TypeB", boost::bind(&Reciever::handler, &recvB,
                                                  _1));

    ram::core::EventPtr eventA(new ram::core::Event());
    publisher.publish("TypeA", eventA);
    ram::core::EventPtr eventB(new ram::core::Event());
    publisher.publish("TypeB", eventB);

    // Make sure they are not here yet
    CHECK_EQUAL(0, recvA.calls);
    CHECK_EQUAL(0, recvB.calls);
    qpublisher.publishEvents();
    
    CHECK_EQUAL(1, recvA.calls);
    CHECK_EQUAL(eventA, recvA.events[0]);

    CHECK_EQUAL(1, recvB.calls);
    CHECK_EQUAL(eventB, recvB.events[0]);

    CHECK_EQUAL("TypeA", recvA.events[0]->type);
    CHECK_EQUAL(&publisher, recvA.events[0]->sender);
    CHECK_EQUAL("TypeB", recvB.events[0]->type);
    CHECK_EQUAL(&publisher, recvB.events[0]->sender);

    // Now the disconnecting
    connA->disconnect();
    publisher.publish("TypeA", ram::core::EventPtr(new ram::core::Event()));
    publisher.publish("TypeB", ram::core::EventPtr(new ram::core::Event()));

    // Ensure the events are properly queued
    CHECK_EQUAL(1, recvA.calls);
    CHECK_EQUAL(1, recvB.calls);

    // Make sure they are released
    qpublisher.publishEvents();
    CHECK_EQUAL(1, recvA.calls);
    CHECK_EQUAL(2, recvB.calls);

    connB->disconnect();
    publisher.publish("TypeA", ram::core::EventPtr(new ram::core::Event()));
    publisher.publish("TypeB", ram::core::EventPtr(new ram::core::Event()));
    CHECK_EQUAL(1, recvA.calls);
    CHECK_EQUAL(2, recvB.calls);

    qpublisher.publishEvents();
    CHECK_EQUAL(1, recvA.calls);
    CHECK_EQUAL(2, recvB.calls);
}

// Helper functions for the threading test
void publishFunc(boost::barrier* barrier, ram::core::EventPublisher* publisher)
{
    // Wait untill all threads are ready to go
    barrier->wait();
    
    for (int i = 0; i < 100; ++i)
        publisher->publish("Type", ram::core::EventPtr(new ram::core::Event()));
}

void queueThreadCount(int* calls, ram::core::EventPtr event)
{
    *calls = (*calls) + 1;
}

TEST_FIXTURE(QueuedEventPublisherFixture, Threads)
{
    boost::barrier barrier(3);
    int calls = 0;
    
    // Bind are function
    qpublisher.subscribe("Type", boost::bind(&queueThreadCount, &calls, _1));
    
    // Create threads (they will not run unitl the barrier is released)
    boost::thread threadA(boost::bind(&publishFunc, &barrier, &publisher));
    boost::thread threadB(boost::bind(&publishFunc, &barrier, &publisher));

    // Set off all threads, then wait for them to finish
    barrier.wait();

    int publishedEvents = 0;
    while(publishedEvents < 200)
        publishedEvents += qpublisher.waitAndPublishEvents();
    
    threadA.join();
    threadB.join();

    CHECK_EQUAL(200, calls);
}
