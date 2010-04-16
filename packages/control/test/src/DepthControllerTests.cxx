/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/DepthControllerTests.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/test/include/DepthControllerTests.h"

using namespace ram;

// TEST_UTILITY_IMP(setGetDepth, (ram::control::IDepthController* controller))
// {
//     double depth = 6.7;
//     controller->setDepth(depth);
//     CHECK_EQUAL(depth, controller->getDepth());
// }

// TEST_UTILITY_IMP(atDepth,
//                  (control::IDepthController* controller,
//                   boost::function<void(double)> setDepth,
//                   boost::function<void (void)> update))
// {
//     // This assumes the default threshold for depth is 0.5
//     setDepth(4);
//     update();
    
//     controller->setDepth(5);
//     CHECK_EQUAL(false, controller->atDepth());

//     controller->setDepth(3);
//     CHECK_EQUAL(false, controller->atDepth());

//     controller->setDepth(4.3);
//     CHECK_EQUAL(true, controller->atDepth());

//     controller->setDepth(3.7);
//     CHECK_EQUAL(true, controller->atDepth());
    
//     controller->setDepth(4);
//     CHECK(controller->atDepth());
// }

// TEST_UTILITY_IMP(holdCurrentDepth,
//                  (ram::control::IDepthController* controller,
//                   boost::function<void(double)> setDepth,
//                   boost::function<void (void)> update))
// {
//     // Lock in current depth
//     setDepth(4);
//     update();

//     // Check Normall setting
//     controller->setDepth(5);
//     CHECK_EQUAL(5, controller->getDepth());

//     // Hold and make sure we have changed
//     controller->holdCurrentDepth();
//     CHECK_EQUAL(4, controller->getDepth());
// }
