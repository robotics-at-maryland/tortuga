/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestApplication.cxx
 */



/*
I don't know enough about your application to give you that kind of
advice. One way might be to create three threads. The first thread, at
lowest priority, spins on code that does this: acquire mutex, increment
count, release mutex. The second thread, at higher priority, spins on code
that acquires a different mutex, increments a different counter, and
releases the mutex. The third thread, at highest priority, sleeps for 10
seconds, grabs both mutexes, and dumps both counts. One would expect the
counter for the higher priority thread to be larger than for the lowest
priority thread. One would expect the highest priority thread to be able to
get the mutex from the lower priority threads.
*/

// STD Includes
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/Updatable.h"

#ifdef RAM_LINUX
// Linux Includes
#include <sys/syscall.h>
#include <unistd.h>
#define gettid(NOT_USED) ((pid_t)syscall(SYS_gettid))

class TestTID : public ram::core::Updatable
{
public:
    TestTID() : tid(-1), run(-1) {}

    ~TestTID()
        {
            unbackground(true);
        }
    
    virtual void update(double)
        {
            tid = gettid();
            run = 0;
            unbackground(false);
        }

    int tid;
    int run;
};

#endif // RAM_LINUX



class Spinner : public ram::core::Updatable
{
public:
    Spinner() : tid(-1) {}

    ~Spinner()
        {
            unbackground(true);
        }
    
    virtual void update(double)
        {
            tid += 1;
        }

    int tid;
};

SUITE(Updatable)
{

TEST(setPriority)
{
    Spinner test1;
    test1.setPriority(ram::core::IUpdatable::LOW_PRIORITY);
    test1.background(-1);
    test1.unbackground(true);
    CHECK_EQUAL(test1.getPriority(), ram::core::IUpdatable::LOW_PRIORITY);
}

#ifdef RAM_LINUX
TEST(getTID)
{
/*    int myTID = gettid();

    // Start up a background task to grab a TID of a thread
    TestTID testTID;
    testTID.background();
    while (testTID.run == -1) {}

    // Ensure its difference from my TID
    int otherTID = testTID.tid;
    CHECK(myTID != otherTID);*/
}
#endif // RAM_LINUX

} // SUITE(Updatable)
