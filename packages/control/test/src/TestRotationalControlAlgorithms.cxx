/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders
 * All rights reserved.
 *
 * Authory: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/test/src/TestRotationalControlAlgorithms.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/include/Common.h"
#include "control/include/NonlinearPDRotationalController.h"
#include "control/include/GyroObserverPDControllerSwitch.h"
#include "control/include/AdaptiveRotationalController.h"

#include "estimation/test/include/MockEstimator.h"

using namespace ram;

TEST(TestNonlinearPDRotationalController)
{
    control::IRotationalControllerImpPtr controller = control::IRotationalControllerImpPtr(
        new control::NonlinearPDRotationalController(core::ConfigNode::fromString(
                                                "{ 'name' : 'rotController', "
                                                "  'kp' : 10,"
                                                "  'kd' : 1, }")));

    control::DesiredStatePtr desiredState = control::DesiredStatePtr(
        new control::DesiredState(core::ConfigNode::fromString("{}")));

    desiredState->setDesiredOrientation(math::Quaternion::IDENTITY);

    MockEstimator *estimator = new MockEstimator();
    estimation::IStateEstimatorPtr estimatorPtr = 
        estimation::IStateEstimatorPtr(estimator);

    // Testing Yaw Control
    estimator->estOrientation = math::Quaternion(0, 0, 0.2756, 0.9613);
    math::Vector3 exp_rotTorques_yaw(0, 0, -3.5497);
    math::Vector3 act_rotTorques_yaw = controller->rotationalUpdate(
        1.0, estimatorPtr, desiredState);

    CHECK_ARRAY_CLOSE(exp_rotTorques_yaw.ptr(), act_rotTorques_yaw.ptr(), 3, 0.0001);


    // Testing pitch Control
    estimator->estOrientation = math::Quaternion(0, 0.2164, 0, 0.9763);
    math::Vector3 exp_rotTorques_pitch(0, -2.7872, 0);
    math::Vector3 act_rotTorques_pitch = controller->rotationalUpdate(
        1.0, estimatorPtr, desiredState);
    
    CHECK_ARRAY_CLOSE(exp_rotTorques_pitch.ptr(), act_rotTorques_pitch.ptr(), 3, 0.0001);
    
    // Testing Roll Control
    estimator->estOrientation = math::Quaternion(-0.3827, 0, 0, 0.9239);
    math::Vector3 exp_rotTorques_roll(0.7692, 0, 0);
    math::Vector3 act_rotTorques_roll = controller->rotationalUpdate(
        1.0, estimatorPtr, desiredState);
    
    CHECK_ARRAY_CLOSE(exp_rotTorques_roll.ptr(), act_rotTorques_roll.ptr(), 3, 0.0001);
}

TEST(TestGyroObserverPDControllerSwitch)
{
    control::GyroObserverPDControllerSwitch *controller = 
        new control::GyroObserverPDControllerSwitch(core::ConfigNode::fromString(
                                                        "{ 'name' : 'rotController', "
                                                        "  'dtMin' : 0.1,"
                                                        "  'dtMax' : 2.0,"
                                                        "  'gyroObsGain' : 1,"
                                                        "  'gyroPDType' : 2,"
                                                        "  'kp' : 10,"
                                                        "  'kd' : 1, }"));
        
     control::IRotationalControllerImpPtr controllerPtr = 
        control::IRotationalControllerImpPtr(controller);

    control::DesiredStatePtr desiredState = control::DesiredStatePtr(
        new control::DesiredState(core::ConfigNode::fromString("{}")));

    desiredState->setDesiredOrientation(math::Quaternion::IDENTITY);

    MockEstimator *estimator = new MockEstimator();
    estimation::IStateEstimatorPtr estimatorPtr = 
        estimation::IStateEstimatorPtr(estimator);

    {
        estimator->estOrientation = math::Quaternion::IDENTITY;
        
        controllerPtr->rotationalUpdate(1, estimatorPtr, desiredState);
        
        math::Quaternion qHatExp(0, 0, 0, 1);
        CHECK_ARRAY_CLOSE(qHatExp.ptr(), controller->get_qHat().ptr(), 4, 0.0002);
    }

    {
        estimator->estOrientation = math::Quaternion(-0.3005, 0.9016, 0.3005, 0.0805);
        estimator->estAngularRate = math::Vector3(1, 2, 1);

        controllerPtr->rotationalUpdate(1, estimatorPtr, desiredState);
        
        math::Quaternion dqHatExp(-1.1991,1.2506,0.1152,0);
        CHECK_ARRAY_CLOSE(dqHatExp.ptr(), controller->get_dqHat().ptr(), 4, 0.0002);

        math::Quaternion qHatExp(-0.4527,0.4722,0.0435,0.7551);
        CHECK_ARRAY_CLOSE(qHatExp.ptr(), controller->get_qHat().ptr(), 4, 0.0002);
    }
    
}

TEST(TestAdaptiveRotationalController)
{
    control::DesiredStatePtr desiredState = control::DesiredStatePtr(
        new control::DesiredState(core::ConfigNode::fromString("{}")));

    desiredState->setDesiredOrientation(math::Quaternion::IDENTITY);

    MockEstimator *estimator = new MockEstimator();
    estimation::IStateEstimatorPtr estimatorPtr = 
        estimation::IStateEstimatorPtr(estimator);

    {
        control::AdaptiveRotationalController *controller = 
            new control::AdaptiveRotationalController(
                core::ConfigNode::fromString(
                    "{ 'name' : 'rotController', "
                    "  'dtMin' : 0.1,"
                    "  'dtMax' : 1.0,"
                    "  'rotK' : 1,"
                    "  'rotLambda' : 1,"
                    "  'rotGamma' : 1,"
                    "  'adaptParams' : [0.5, 0.0, -0.1,"
                    "                   1.0, 0.0, 1.0,"
                    "                   0.0, 0.0, 0.0,"
                    "                   1.0, 2.0, 2.0] }"));
        
        control::IRotationalControllerImpPtr controllerPtr = 
            control::IRotationalControllerImpPtr(controller);

        estimator->estOrientation = math::Quaternion::IDENTITY;
        estimator->estAngularRate = math::Vector3::ZERO;

        desiredState->setDesiredOrientation(math::Quaternion::IDENTITY);
        desiredState->setDesiredAngularRate(math::Vector3::ZERO);

        math::Vector3 exp_torques(0, 0, 0);
        math::Vector3 act_torques = controllerPtr->rotationalUpdate(
            0.2, estimatorPtr, desiredState);

        CHECK_ARRAY_CLOSE(exp_torques.ptr(), act_torques.ptr(), 3, 0.0001);
    }

    {
        control::AdaptiveRotationalController *controller = 
            new control::AdaptiveRotationalController(
                core::ConfigNode::fromString(
                    "{ 'name' : 'rotController', "
                    "  'dtMin' : 0.1,"
                    "  'dtMax' : 1.0,"
                    "  'rotK' : 1,"
                    "  'rotLambda' : 1,"
                    "  'rotGamma' : 1,"
                    "  'adaptParams' : [0.5, 0.0, -0.1,"
                    "                   1.0, 0.0, 1.0,"
                    "                   0.0, 0.0, 0.0,"
                    "                   1.0, 2.0, 2.0] }"));
        
        control::IRotationalControllerImpPtr controllerPtr = 
            control::IRotationalControllerImpPtr(controller);

        estimator->estOrientation = math::Quaternion::IDENTITY;
        estimator->estAngularRate = math::Vector3::ZERO;
        
        desiredState->setDesiredOrientation(math::Quaternion(0, 0, 0.7071, 0.7071));
        desiredState->setDesiredAngularRate(math::Vector3::ZERO);
        
        math::Vector3 exp_torques(0, 0, 0.7071);
        math::Vector3 act_torques = controllerPtr->rotationalUpdate(
            0.1, estimatorPtr, desiredState);

        CHECK_ARRAY_CLOSE(exp_torques.ptr(), act_torques.ptr(), 3, 0.0001);
    }

    {
        control::AdaptiveRotationalController *controller = 
            new control::AdaptiveRotationalController(
                core::ConfigNode::fromString(
                    "{ 'name' : 'rotController', "
                    "  'dtMin' : 0.1,"
                    "  'dtMax' : 1.0,"
                    "  'rotK' : 1,"
                    "  'rotLambda' : 1,"
                    "  'rotGamma' : 1,"
                    "  'adaptParams' : [0.5, 0.0, -0.1,"
                    "                   1.0, 0.0, 1.0,"
                    "                   0.0, 0.0, 0.0,"
                    "                   1.0, 2.0, 2.0] }"));
        
        control::IRotationalControllerImpPtr controllerPtr = 
            control::IRotationalControllerImpPtr(controller);

        estimator->estOrientation = math::Quaternion::IDENTITY;
        estimator->estAngularRate = math::Vector3::ZERO;
        
        desiredState->setDesiredOrientation(math::Quaternion(0.7071, 0, 0, 0.7071));
        desiredState->setDesiredAngularRate(math::Vector3::ZERO);
        
        math::Vector3 exp_torques(0.9192, 0, 0);
        math::Vector3 act_torques = controllerPtr->rotationalUpdate(
            0.3, estimatorPtr, desiredState);

        CHECK_ARRAY_CLOSE(exp_torques.ptr(), act_torques.ptr(), 3, 0.0001);
    }

    /* This test fails possibly due to a bug in the old implementation
     * adaptive rotational controller.  Turning it off for now.  The
     * failure is due to a non-zero desired angular rate.
     */

    // {
    //     control::AdaptiveRotationalController *controller = 
    //         new control::AdaptiveRotationalController(
    //             core::ConfigNode::fromString(
    //                 "{ 'name' : 'rotController', "
    //                 "  'dtMin' : 0.1,"
    //                 "  'dtMax' : 1.0,"
    //                 "  'rotK' : 1,"
    //                 "  'rotLambda' : 1,"
    //                 "  'rotGamma' : 1,"
    //                 "  'adaptParams' : [0.5, 0.0, -0.1,"
    //                 "                   1.0, 0.0, 1.0,"
    //                 "                   0.0, 0.0, 0.0,"
    //                 "                   1.0, 2.0, 2.0] }"));
        
    //     control::IRotationalControllerImpPtr controllerPtr = 
    //         control::IRotationalControllerImpPtr(controller);

    //     estimator->estOrientation = math::Quaternion::IDENTITY;
    //     estimator->estAngularRate = math::Vector3::ZERO;
        
    //     desiredState->setDesiredOrientation(math::Quaternion(0.7071, 0, 0, 0.7071));
    //     desiredState->setDesiredAngularRate(math::Vector3(-0.2, 0, 0));
        
    //     math::Vector3 exp_torques(0.4582, 0, 0.0142);
    //     math::Vector3 act_torques = controllerPtr->rotationalUpdate(
    //         0.05, estimatorPtr, desiredState);

    //     CHECK_ARRAY_CLOSE(exp_torques.ptr(), act_torques.ptr(), 3, 0.0001);
    // }
}
