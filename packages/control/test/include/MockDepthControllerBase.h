/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockDepthControllerBase.h
 */

#ifndef RAM_CONTROL_TEST_MOCKDEPTHCONTROLLERBASE_H_03_15_2009
#define RAM_CONTROL_TEST_MOCKDEPTHCONTROLLERBASE_H_03_15_2009

// Project Includes
#include "control/include/DepthControllerBase.h"

class MockDepthControllerBase :
    public ram::control::DepthControllerBase
{
public:
    MockDepthControllerBase(ram::core::ConfigNode config) :
        ram::control::DepthControllerBase(config) {}
    
    virtual ram::math::Vector3 depthUpdate(double timestep, double depth,
                                           ram::math::Quaternion orientation,
                                           ram::controltest::DesiredStatePtr desiredState)
    {
        ram::control::DepthControllerBase::depthUpdate(
            timestep, depth, orientation, desiredState);
        return ram::math::Vector3::ZERO;
    }
};

#endif // RAM_CONTROL_TEST_MOCKDEPTHCONTROLLERBASE_H_03_15_2009
