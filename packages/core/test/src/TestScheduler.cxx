/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/SubsystemMaker.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <iostream>

// Project Includes
#include "core/include/Runnable.h"
#include "core/include/Scheduler.h"

using namespace ram;

TEST(basicTest)
{
    core::Scheduler *s = new core::Scheduler();

    s->run();

    for (int i=0;i<1000;i++)
        std::cout << "OTHER THREAD\n";

    s->stop();

    std::cout << "ALL DONE\n";

}
