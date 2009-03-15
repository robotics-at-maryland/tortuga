/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockDepthControllerBase.h
 */

// Project Includes
#include "control/include/DepthControllerBase.h"

class MockDepthControllerBase :
    public ram::control::DepthControllerBase
{
public:
    MockDepthControllerBase(ram::core::ConfigNode config) :
        ram::control::DepthControllerBase(config) {}
    
    virtual ram::math::Vector3 depthUpdate(double timestep, double depth,
                                           ram::math::Quaternion orientation)
    {
        ram::control::DepthControllerBase::depthUpdate(
            timestep, depth, orientation);
        return ram::math::Vector3::ZERO;
    }
};
