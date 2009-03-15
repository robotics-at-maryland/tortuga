/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/DepthControllerTests.h
 */

#ifndef RAM_CONTROL_TEST_DEPTHCONTROLLERTESTS_03_14_2009
#define RAM_CONTROL_TEST_DEPTHCONTROLLERTESTS_03_14_2009

// Library Includes
#include <boost/function.hpp>

// Project Includes
#include "control/include/IDepthController.h"
#include "core/test/include/Macros.h"

/** Tests whether the IDepthController::atDepth function is working
 *
 *  Note: this assumes the threshold is 0.5.
 */
TEST_UTILITY_FWD(atDepth,
                 (ram::control::IDepthController* controller,
                  boost::function<void(double)> setDepth,
                  boost::function<void (void)> update));

#endif // RAM_CONTROL_TEST_DEPTHCONTROLLERTESTS_03_14_2009
