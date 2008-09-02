/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockDepthController.h
 */

#ifndef RAM_CONTROL_TEST_DEPTHCONTROLLER_09_01_2008
#define RAM_CONTROL_TEST_DEPTHCONTROLLER_09_01_2008

// Project Includes
#include "control/include/IDepthController.h"
#include "core/include/ConfigNode.h"

class MockDepthController : public ram::control::IDepthControllerImp
{
public:
    MockDepthController(ram::core::ConfigNode)
        {}
    
    virtual ~MockDepthController() {}

    virtual void setDepth(double) { return; }

    virtual double getDepth() { return 0; }
    
    virtual double getEstimatedDepth() { return 0; }
    
    virtual double getEstimatedDepthDot() { return 0; }
    
    virtual bool atDepth() { return true; }

    virtual ram::math::Vector3 depthUpdate(ram::math::Quaternion ) {
        return ram::math::Vector3::ZERO; }
};

#endif	// RAM_CONTROL_TEST_DEPTHCONTROLLER_09_01_2008
