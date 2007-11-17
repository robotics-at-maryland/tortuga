/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/EventPublisher.cxx
 */

// STD Includes
#include <string>
#include <vector>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

// Project Includes
#include "core/include/EventPublisher.h"

struct Reciever
{
    Reciever() : calls(0) {}

    void handler(ram::core::EventPtr event_)
     {
         calls++;
         events.push_back(event_);
     }
    
    int calls;
    std::vector<ram::core::EventPtr> events;
};

TEST(Subscribe)
{
    Reciever recv;
    ram::core::EventPublisher publisher;
    publisher.subscribe("Type", boost::bind(&Reciever::handler, &recv, _1));
}

TEST(Publish)
{
    Reciever recv;
    ram::core::EventPublisher publisher;
    publisher.subscribe("Type", boost::bind(&Reciever::handler, &recv, _1));

    ram::core::EventPtr event(new ram::core::Event());
    publisher.publish("Type", event);

    CHECK_EQUAL(1, recv.calls);
    CHECK_EQUAL(event, recv.events[0]);
    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisher, recv.events[0]->sender);
}

TEST(EmptyPublish)
{
    ram::core::EventPublisher publisher;
    publisher.publish("Type", ram::core::EventPtr(new ram::core::Event()));
}

TEST(MultipleSubscribers)
{
    Reciever recv;
    ram::core::EventPublisher publisher;

    // Bind Many
    publisher.subscribe("Type", boost::bind(&Reciever::handler, &recv, _1));
    publisher.subscribe("Type", boost::bind(&Reciever::handler, &recv, _1));

    ram::core::EventPtr event(new ram::core::Event());
    publisher.publish("Type", event);

    CHECK_EQUAL(2, recv.calls);
    CHECK_EQUAL(event, recv.events[0]);
    CHECK_EQUAL(event, recv.events[1]);

    CHECK_EQUAL("Type", recv.events[0]->type);
    CHECK_EQUAL(&publisher, recv.events[0]->sender);
    CHECK_EQUAL("Type", recv.events[1]->type);
    CHECK_EQUAL(&publisher, recv.events[1]->sender);
}

TEST(MultipleEventTypes)
{
    Reciever recvA;
    Reciever recvB;
    ram::core::EventPublisher publisher;

    // Bind Many
    publisher.subscribe("TypeA", boost::bind(&Reciever::handler, &recvA, _1));
    publisher.subscribe("TypeB", boost::bind(&Reciever::handler, &recvB, _1));

    ram::core::EventPtr eventA(new ram::core::Event());
    publisher.publish("TypeA", eventA);
    ram::core::EventPtr eventB(new ram::core::Event());
    publisher.publish("TypeB", eventB);

    CHECK_EQUAL(1, recvA.calls);
    CHECK_EQUAL(eventA, recvA.events[0]);

    CHECK_EQUAL(1, recvB.calls);
    CHECK_EQUAL(eventB, recvB.events[0]);

    CHECK_EQUAL("TypeA", recvA.events[0]->type);
    CHECK_EQUAL(&publisher, recvA.events[0]->sender);
    CHECK_EQUAL("TypeB", recvB.events[0]->type);
    CHECK_EQUAL(&publisher, recvB.events[0]->sender);
}

// Helper functions for the threading test
void threadFunc(boost::barrier* barrier, ram::core::EventPublisher* publisher)
{
    // Wait untill all threads are ready to go
    barrier->wait();
    
    for (int i = 0; i < 100; ++i)
        publisher->publish("Type", ram::core::EventPtr(new ram::core::Event()));
}

void threadCount(int* calls, ram::core::EventPtr event)
{
    *calls = (*calls) + 1;
}

TEST(Threads)
{
    ram::core::EventPublisher publisher;
    boost::barrier barrier(3);
    int calls = 0;
    
    // Bind are function
    publisher.subscribe("Type", boost::bind(&threadCount, &calls, _1));
    
    // Create threads (they will not run unitl the barrier is released)
    boost::thread threadA(boost::bind(&threadFunc, &barrier, &publisher));
    boost::thread threadB(boost::bind(&threadFunc, &barrier, &publisher));

    // Set off all threads, then wait for them to finish
    barrier.wait();
    threadA.join();
    threadB.join();

    CHECK_EQUAL(200, calls);
}
