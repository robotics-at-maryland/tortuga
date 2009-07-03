/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/TranslationalControllerTests.h
 */

#ifndef RAM_CONTROL_TEST_TRANSLATIONALCONTROLLERTESTS_03_15_2009
#define RAM_CONTROL_TEST_TRANSLATIONALCONTROLLERTESTS_03_15_2009

// Project Includes
#include "control/include/ITranslationalController.h"
#include "core/test/include/Macros.h"

TEST_UTILITY_FWD(setGetVelocity,
                 (ram::control::ITranslationalController* controller));

TEST_UTILITY_FWD(setGetSpeed,
                 (ram::control::ITranslationalController* controller));

TEST_UTILITY_FWD(setGetSidewaysSpeed,
                 (ram::control::ITranslationalController* controller));

#endif // RAM_CONTROL_TEST_TRANSLATIONALCONTROLLERTESTS_03_15_2009
