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

using namespace ram;


TEST_UTILITY_IMP(setGetDepth, (ram::control::IController* controller))
{
    {
        double expDepth = 1.67;
        controller->setDepth(expDepth);
        CHECK_EQUAL(expDepth, controller->getDepth());
    }

    {
        double expDepth = 6.7;
        controller->setDepth(expDepth);
        CHECK_EQUAL(expDepth, controller->getDepth());
    }
}

TEST_UTILITY_IMP(yawVehicle, (ram::control::IController* controller))
{
    {
        const double startYaw = controller->getDesiredOrientation().getYaw().valueDegrees();
        const double degreesToYaw = 78.2;

        controller->yawVehicle(degreesToYaw);

        const double middleYaw = controller->getDesiredOrientation().getYaw().valueDegrees();
        CHECK_CLOSE(degreesToYaw, (middleYaw - startYaw), 0.0001);
    
        controller->yawVehicle(-degreesToYaw);

        const double endYaw = controller->getDesiredOrientation().getYaw().valueDegrees();
        CHECK_CLOSE(0, (endYaw - startYaw), 0.0001);
    }

    {
        controller->setDesiredOrientation(math::Quaternion::IDENTITY);
        const double degreesToYaw = 30;
        math::Quaternion expOrientation(math::Degree(degreesToYaw), math::Vector3::UNIT_Z);
        controller->yawVehicle(degreesToYaw);
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

        controller->pitchVehicle(degreesToPitch);

        const double middlePitch = controller->getDesiredOrientation().getPitch().valueDegrees();
        CHECK_CLOSE(degreesToPitch, (middlePitch - startPitch), 0.0001);
    
        controller->pitchVehicle(-degreesToPitch);

        const double endPitch = controller->getDesiredOrientation().getPitch().valueDegrees();
        CHECK_CLOSE(0, (endPitch - startPitch), 0.0001);
    }

    {
        controller->setDesiredOrientation(math::Quaternion::IDENTITY);
        const double degreesToPitch = 30;
        math::Quaternion expOrientation(math::Degree(degreesToPitch), math::Vector3::UNIT_Y);
        controller->pitchVehicle(degreesToPitch);
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

        controller->rollVehicle(degreesToRoll);

        const double middleRoll = controller->getDesiredOrientation().getRoll().valueDegrees();
        CHECK_CLOSE(degreesToRoll, (middleRoll - startRoll), 0.0001);
    
        controller->rollVehicle(-degreesToRoll);

        const double endRoll = controller->getDesiredOrientation().getRoll().valueDegrees();
        CHECK_CLOSE(0, (endRoll - startRoll), 0.0001);
    }

    {
        controller->setDesiredOrientation(math::Quaternion::IDENTITY);
        const double degreesToRoll = 30;
        math::Quaternion expOrientation(math::Degree(degreesToRoll), math::Vector3::UNIT_X);
        controller->rollVehicle(degreesToRoll);
        CHECK_ARRAY_CLOSE(expOrientation.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
    }
}

TEST_UTILITY_IMP(setGetDesiredOrientation, (ram::control::IController* controller))
{
    {
        math::Quaternion orientation(0.2, 1.2, 2.5, 1);
        controller->setDesiredOrientation(orientation);
        
        orientation.normalise();
        CHECK_ARRAY_CLOSE(orientation.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
    }

    /* Test to make sure that the desired orientation is always normalized */
    {
        math::Quaternion unnormalizedQuat(2,5,3,6);
        math::Quaternion normalizedQuat(unnormalizedQuat);
        normalizedQuat.normalise();
        
        controller->setDesiredOrientation(normalizedQuat);
        CHECK_ARRAY_CLOSE(normalizedQuat.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
        
        controller->setDesiredOrientation(unnormalizedQuat);
        CHECK_ARRAY_CLOSE(normalizedQuat.ptr(),
                          controller->getDesiredOrientation().ptr(),
                          4, 0.0001);
    }

    {
        const math::Quaternion expQuat(math::Degree(30), math::Vector3::UNIT_X);
        controller->setDesiredOrientation(expQuat);
        const math::Quaternion actQuat(controller->getDesiredOrientation());
        CHECK_ARRAY_CLOSE(expQuat.ptr(), actQuat.ptr(), 4, 0.0001);
    }
}

TEST_UTILITY_IMP(setGetSpeed, (ram::control::IController* controller))
{
    const double MIN_SPEED = -5;
    const double MAX_SPEED = 5;

    {
        const double expSpeed = 2.5;
        controller->setSpeed(expSpeed);
        CHECK_EQUAL(expSpeed, controller->getSpeed());
    }

    {
        const double expSpeed = -2;
        const double angleToYaw = 30;
        // check that speed is invariant upon rotations
        controller->yawVehicle(angleToYaw);
        controller->setSpeed(expSpeed);
        CHECK_EQUAL(expSpeed, controller->getSpeed());
    }

    // make sure speed is clamped at +/- 5
    {
        const double speed = MAX_SPEED + 0.8;
        controller->setSpeed(speed);
        CHECK_EQUAL(MAX_SPEED, controller->getSpeed());
    }

    {
        const double speed = MIN_SPEED - 1.4;
        controller->setSpeed(speed);
        CHECK_EQUAL(MIN_SPEED, controller->getSpeed());
    }
}

TEST_UTILITY_IMP(setGetSidewaysSpeed, (ram::control::IController* controller))
{
    const double MIN_SPEED = -5;
    const double MAX_SPEED = 5;

    {
        const double expSpeed = 2.5;
        controller->setSidewaysSpeed(expSpeed);
        CHECK_EQUAL(expSpeed, controller->getSidewaysSpeed());
    }

    {
        const double expSpeed = -2;
        const double angleToYaw = 30;
        // check that speed is invariant upon rotations
        controller->yawVehicle(angleToYaw);
        controller->setSidewaysSpeed(expSpeed);
        CHECK_EQUAL(expSpeed, controller->getSidewaysSpeed());
    }

    // make sure speed is clamped at +/- 5
    {
        const double speed = MAX_SPEED + 0.8;
        controller->setSidewaysSpeed(speed);
        CHECK_EQUAL(MAX_SPEED, controller->getSidewaysSpeed());
    }

    {
        const double speed = MIN_SPEED - 1.4;
        controller->setSidewaysSpeed(speed);
        CHECK_EQUAL(MIN_SPEED, controller->getSidewaysSpeed());
    }
}

TEST_UTILITY_IMP(setGetDesiredVelocity, (ram::control::IController* controller))
{
    /* Set Inertial, Get Inertial */
    math::Vector2 velocity_n_n(2.4,-4.3);
    controller->setDesiredVelocity(velocity_n_n,
                                   control::IController::INERTIAL_FRAME);
    CHECK_EQUAL(velocity_n_n, controller->getDesiredVelocity(
                    control::IController::INERTIAL_FRAME));

    /* The Following rely on the current orientation, so the vehicle's current orientation
       must be set to a known value and correct results calculated with this value */

    /* Set Inertial, Get Body */

    /* Set Body, Get Body */

    /* Set Body, Get Inertial */
}

TEST_UTILITY_IMP(setGetDesiredPosition, (ram::control::IController* controller))
{
    /* Set Inertial, Get Inertial */
    math::Vector2 position_n_n(2.4,-4.3);
    controller->setDesiredPosition(position_n_n,
                                   control::IController::INERTIAL_FRAME);
    CHECK_EQUAL(position_n_n, controller->getDesiredPosition(
                    control::IController::INERTIAL_FRAME));

    /* The Following rely on the current orientation, so the vehicle's current orientation
       must be set to a known value and correct results calculated with this value */

    /* Set Inertial, Get Body */

    /* Set Body, Get Body */

    /* Set Body, Get Inertial */
}
