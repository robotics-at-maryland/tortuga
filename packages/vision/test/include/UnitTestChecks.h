/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/UnitTestChecks.h
 */

#ifndef RAM_VISION_TEST_UNITTESTCHECKS_H_01_23_2008
#define RAM_VISION_TEST_UNITTESTCHECKS_H_01_23_2008

// Library Includes
#include "UnitTest++/Checks.h"

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

namespace UnitTest {

template<>
void CheckClose(TestResults& results,
                ram::vision::Image const& expected,
                ram::vision::Image const& actual,
                double const& tolerance,
                TestDetails const& details);

template<>
void CheckClose(TestResults& results,
                ram::vision::Image const& expected,
                ram::vision::Image const& actual,
                int const& tolerance,
                TestDetails const& details);

template<>
void CheckClose(TestResults& results,
                ram::vision::OpenCVImage* const& expected,
                ram::vision::OpenCVImage* const& actual,
                double const& tolerance,
                TestDetails const& details);

template<>
void CheckClose(TestResults& results,
                ram::vision::OpenCVImage* const& expected,
                ram::vision::OpenCVImage* const& actual,
                int const& tolerance,
                TestDetails const& details);

    
} // namespace UnitTest

#endif // RAM_VISION_TEST_UNITTESTCHECKS_H_01_23_2008
