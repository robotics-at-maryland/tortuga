/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/math/test/src/TestImplicitSurface.cxx
 */

// STD Includes
#include <vector>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/include/ImplicitSurface.h"
#include "math/include/SphericalPrimitive.h"

using namespace ram::math;

SUITE(ImplicitSurfaceTest) {

TEST(construct)
{
    IPrimitive3DPtr p1 = IPrimitive3DPtr(
        new SphericalPrimitive(Vector3(0,0,0), 1.0));

    std::vector<IPrimitive3DPtr> primitives;
    primitives.push_back(p1);

    ImplicitSurface surf = ImplicitSurface(primitives, 10.0);

    CHECK(surf.implicitFunctionValue(Vector3(0,0,0)) < 1);
    CHECK(surf.implicitFunctionValue(Vector3(4,2,3)) > 1);
    CHECK(surf.implicitFunctionValue(Vector3(1,0,0)) == 1);
}

}
