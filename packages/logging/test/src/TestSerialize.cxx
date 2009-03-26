/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/src/TestChecks.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "logging/include/serialize.h"
#include <iostream>

#include "vision/include/Events.h"

// Bring Vector3, and other math classes into scope
using namespace ram::math;

TEST(ChangeMe)
{
  printf("Doing a test.\n");
  std::ofstream ofs("LoggingTestChangeMeOutput");
  const ram::vision::DuctEventPtr myDuctEventPointer(new ram::vision::DuctEvent(1.0, 1.0, 1.0, 1.0, true, true));
  {
    boost::archive::text_oarchive oa(ofs);
    oa << myDuctEventPointer;
  }
  ofs.close();
  double a = 1;
  double b = 1.2;
  CHECK_CLOSE(a, b, 0.6);
}

