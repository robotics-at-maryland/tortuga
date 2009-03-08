/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockRotationalControllerBase.h
 */

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
        ram::math::Quaternion orientation,
        ram::math::Vector3 angularRate)
        {
            ram::control::RotationalControllerBase::rotationalUpdate(
                timestep, orientation, angularRate);
            return ram::math::Vector3::ZERO;
        }
};
