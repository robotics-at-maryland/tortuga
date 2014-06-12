/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/test/src/IControllerImpTests.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/test/include/IControllerImpTests.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "math/test/include/MathChecks.h"

using namespace ram;


TEST_UTILITY_IMP(changeDepth, (ram::control::IController* controller))
{
    {
        double expDepth = 1.67;
        double expDepthRate = 0.5;
        controller->changeDepth(expDepth, expDepthRate);
        CHECK_EQUAL(expDepth, controller->getDesiredDepth());
        CHECK_EQUAL(expDepthRate, controller->getDesiredDepthRate());
    }

    {
        double expDepth = 6.7;
        double expDepthRate = -0.7;
        controller->changeDepth(expDepth, expDepthRate);
        CHECK_EQUAL(expDepth, controller->getDesiredDepth());
        CHECK_EQUAL(expDepthRate, controller->getDesiredDepthRate());
    }
}

TEST_UTILITY_IMP(rotate, (ram::control::IController* controller))
{
    {
        math::Quaternion orientation(0.2, 1.2, 2.5, 1);
        math::Vector3 angularRate(0.5, -.2, 0);
        controller->rotate(orientation, angularRate);
        
        orientation.normalise();
        CHECK_ARRAY_CLOSE(orientation.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);

        CHECK_ARRAY_CLOSE(angularRate.ptr(),
                          controller->getDesiredAngularRate().ptr(),
                          3, 0.0001);
    }

    /* Test to make sure that the desired orientation is always normalized */
    {
        math::Quaternion unnormalizedQuat(2,5,3,6);
        math::Quaternion normalizedQuat(unnormalizedQuat);
        math::Vector3 angularRate(-1.5, 0, 2);
        normalizedQuat.normalise();
        
        controller->rotate(normalizedQuat, angularRate);
        CHECK_ARRAY_CLOSE(normalizedQuat.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
        
        controller->rotate(unnormalizedQuat, angularRate);
        CHECK_ARRAY_CLOSE(normalizedQuat.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);

        CHECK_ARRAY_CLOSE(angularRate.ptr(),
                          controller->getDesiredAngularRate().ptr(),
                          3, 0.0001);
    }

    {
        const math::Quaternion expQuat(math::Degree(30), math::Vector3::UNIT_X);
        controller->rotate(expQuat, math::Vector3::ZERO);
        const math::Quaternion actQuat(controller->getDesiredOrientation());
        CHECK_ARRAY_CLOSE(expQuat.ptr(), actQuat.ptr(), 4, 0.0001);
    }
}

TEST_UTILITY_IMP(translate, (ram::control::IController* controller))
{
    math::Vector2 expVelocity(2.4,-4.3);
    math::Vector2 expPosition(1.5, 3.4);

    controller->translate(expPosition, expVelocity);

    CHECK_ARRAY_CLOSE(expVelocity.ptr(), controller->getDesiredVelocity().ptr(), 2, 0.0001);
    CHECK_ARRAY_CLOSE(expPosition.ptr(), controller->getDesiredPosition().ptr(), 2, 0.0001);
}


TEST_UTILITY_IMP(yawVehicle, (ram::control::IController* controller))
{
    {
        const double startYaw = controller->getDesiredOrientation().getYaw().valueDegrees();
        const double degreesToYaw = 78.2;

        controller->yawVehicle(degreesToYaw, 0);

        const double middleYaw = controller->getDesiredOrientation().getYaw().valueDegrees();
        CHECK_CLOSE(degreesToYaw, (middleYaw - startYaw), 0.0001);
   
        controller->yawVehicle(-degreesToYaw, 0);

        const double endYaw = controller->getDesiredOrientation().getYaw().valueDegrees();
        CHECK_CLOSE(0, (endYaw - startYaw), 0.0001);
    }

    {
        controller->rotate(math::Quaternion::IDENTITY, math::Vector3::ZERO);
        const double degreesToYaw = 30;
        math::Quaternion expOrientation(math::Degree(degreesToYaw), math::Vector3::UNIT_Z);
        controller->yawVehicle(degreesToYaw, 0);
        CHECK_ARRAY_CLOSE(expOrientation.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
    }
}

TEST_UTILITY_IMP(pitchVehicle, (ram::control::IController* controller))
{
    {
        const double startPitch = controller->getDesiredOrientation().getPitch().valueDegrees();
        const double degreesToPitch = 45.2;

        controller->pitchVehicle(degreesToPitch, 0);

        const double middlePitch = controller->getDesiredOrientation().getPitch().valueDegrees();
        CHECK_CLOSE(degreesToPitch, (middlePitch - startPitch), 0.0001);
   
        controller->pitchVehicle(-degreesToPitch, 0);

        const double endPitch = controller->getDesiredOrientation().getPitch().valueDegrees();
        CHECK_CLOSE(0, (endPitch - startPitch), 0.0001);
    }

    {
        controller->rotate(math::Quaternion::IDENTITY, math::Vector3::ZERO);
        const double degreesToPitch = 30;
        math::Quaternion expOrientation(math::Degree(degreesToPitch), math::Vector3::UNIT_Y);
        controller->pitchVehicle(degreesToPitch, 0);
        CHECK_ARRAY_CLOSE(expOrientation.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
    }
}

TEST_UTILITY_IMP(rollVehicle, (ram::control::IController* controller))
{
    {
        const double startRoll = controller->getDesiredOrientation().getRoll().valueDegrees();
        const double degreesToRoll = 45.2;

        controller->rollVehicle(degreesToRoll, 0);

        const double middleRoll = controller->getDesiredOrientation().getRoll().valueDegrees();
        CHECK_CLOSE(degreesToRoll, (middleRoll - startRoll), 0.0001);
   
        controller->rollVehicle(-degreesToRoll, 0);

        const double endRoll = controller->getDesiredOrientation().getRoll().valueDegrees();
        CHECK_CLOSE(0, (endRoll - startRoll), 0.0001);
    }

    {
        controller->rotate(math::Quaternion::IDENTITY, math::Vector3::ZERO);
        const double degreesToRoll = 30;
        math::Quaternion expOrientation(math::Degree(degreesToRoll), math::Vector3::UNIT_X);
        controller->rollVehicle(degreesToRoll, 0);
        CHECK_ARRAY_CLOSE(expOrientation.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
    }
}
