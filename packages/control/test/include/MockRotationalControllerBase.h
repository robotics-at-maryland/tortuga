/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockRotationalControllerBase.h
 */

#ifndef RAM_CONTROL_TEST_MOCKROTATIONALCONTROLLERBASE_H_03_08_2009
#define RAM_CONTROL_TEST_MOCKROTATIONALCONTROLLERBASE_H_03_08_2009

// Project Includes
#include "control/include/RotationalControllerBase.h"

class MockRotationalControllerBase :
    public ram::control::RotationalControllerBase
{
public:
    MockRotationalControllerBase(ram::core::ConfigNode config) :
        ram::control::RotationalControllerBase(config) {}
    
    virtual ram::math::Vector3 rotationalUpdate(
        double timestep,
        ram::estimation::IStateEstimatorPtr estimator,
        ram::control::DesiredStatePtr desiredState)
        {
            ram::control::RotationalControllerBase::rotationalUpdate(
                timestep, estimator, desiredState);
            return ram::math::Vector3::ZERO;
        }
};

#endif // RAM_CONTROL_TEST_MOCKROTATIONALCONTROLLERBASE_H_03_15_2009
