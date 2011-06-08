/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/math/test/src/TestPrimitives.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/include/SphericalPrimitive.h"

using namespace ram::math;

SUITE(PrimitivesTest) {

TEST(sphericalPrimitive)
{
    Vector3 center = Vector3(1.0, 2.0, 3.0);
    double radius = 2.0;
    SphericalPrimitive prim = SphericalPrimitive(center, radius);
    
    CHECK_ARRAY_CLOSE(center.ptr(), prim.center().ptr(), 3, 0.0001);
    CHECK_CLOSE(radius, prim.radius(), 0.0001);

    double val1 = prim.implicitFunctionValue(Vector3(1.0, 0, 3.0));
    CHECK_CLOSE(val1, 1.0, 0.0001);

    double val2 = prim.implicitFunctionValue(Vector3(0.5, 0.8, 2.7));
    CHECK(val2 < 1.0);

    double val3 = prim.implicitFunctionValue(Vector3(-3.0, 1.0, 5.0));
    CHECK(val3 > 1.0);
}

}
