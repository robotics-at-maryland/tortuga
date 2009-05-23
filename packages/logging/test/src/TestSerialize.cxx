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
#include <sstream>

#include "vision/include/Events.h"

// Bring Vector3, and other math classes into scope
using namespace ram::math;

TEST(ChangeMe)
{
  std::ostringstream ofs;
  ram::vision::RedLightEventPtr myRedLightEventPointer(new ram::vision::RedLightEvent(1.2, 2.7));
  //ram::vision::RedLightEvent *testEvent=myRedLightEventPointer.get();
  myRedLightEventPointer->azimuth=Degree(3.2);
  myRedLightEventPointer->elevation=Degree(3.3);
  myRedLightEventPointer->range=3.4;
  myRedLightEventPointer->pixCount=3;
  ram::core::EventPtr myEventPtr=myRedLightEventPointer;
  {
    boost::archive::text_oarchive oa(ofs);
    oa.register_type(static_cast<ram::vision::RedLightEvent*>(NULL));
    oa << myEventPtr;
  }
  std::istringstream ifs(ofs.str());
  ram::vision::RedLightEventPtr myRedLightEventPointer2(new ram::vision::RedLightEvent(1.5, 2.5));
  {
    boost::archive::text_iarchive iar(ifs);
    iar.register_type(static_cast<ram::vision::RedLightEvent*>(NULL));
    iar >> myRedLightEventPointer2;
  }
  CHECK_EQUAL(myRedLightEventPointer2->x, myRedLightEventPointer->x);
  CHECK_EQUAL(myRedLightEventPointer2->y, myRedLightEventPointer->y);
  CHECK_EQUAL(myRedLightEventPointer2->azimuth, myRedLightEventPointer->azimuth);
  CHECK_EQUAL(myRedLightEventPointer2->elevation, myRedLightEventPointer->elevation);
  CHECK_EQUAL(myRedLightEventPointer2->range, myRedLightEventPointer->range);
  CHECK_EQUAL(myRedLightEventPointer2->pixCount, myRedLightEventPointer->pixCount);

}

