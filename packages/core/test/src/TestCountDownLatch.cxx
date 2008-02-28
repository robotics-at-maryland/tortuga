/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestCountDownLatch.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/TimeVal.h"
#include "core/include/CountDownLatch.h"

SUITE(CountDownLatch) {

TEST(timedAwait)
{
    ram::core::CountDownLatch latch(1);
    
    // Wait for one second
    boost::xtime timeout = {0, 0};
    timeout.sec = 1;

    ram::core::TimeVal start(ram::core::TimeVal::timeOfDay());
    bool result = latch.await(timeout);
    ram::core::TimeVal duration(ram::core::TimeVal::timeOfDay() - start);

    double actual = duration.get_double();
    CHECK_EQUAL(false, result);
    CHECK_CLOSE(1, actual, 0.02);
}
    
} // SUITE(CountDownLatch)
