/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockRotationalController.h
 */

#ifndef RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008
#define RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008

// Project Includes
#include "control/include/IRotationalController.h"
#include "core/include/ConfigNode.h"

class MockRotationalController :
    public ram::control::IRotationalControllerImp
{
public:
    MockRotationalController(ram::core::ConfigNode) :
        timestep(0),
        estimator(ram::estimation::IStateEstimatorPtr()),
        desiredState(ram::control::DesiredStatePtr()),
        torque(0, 0, 0)
        {}
    
    virtual ~MockRotationalController() {}
    
    virtual ram::math::Vector3 rotationalUpdate(
        double timestep_,
        ram::estimation::IStateEstimatorPtr estimator_,
        ram::control::DesiredStatePtr desiredState_)
    {
        timestep = timestep_;
        estimator = estimator_;
        desiredState = desiredState_;
        return torque;
    }

    double timestep;
    ram::estimation::IStateEstimatorPtr estimator;
    ram::control::DesiredStatePtr desiredState;
    ram::math::Vector3 torque;
};

#endif	// RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008
