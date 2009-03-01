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
    MockDepthController(ram::core::ConfigNode) :
        depth(0),
        depthSet(0),
        estimatedDepth(0),
        estimatedDepthDot(0),
        atDepthValue(false),
        updateDepth(0),
        orientation(ram::math::Quaternion::IDENTITY),
        force(0, 0, 0)
        {}
    
    virtual ~MockDepthController() {}

    virtual void setDepth(double depth) { depthSet = depth; }

    virtual double getDepth() { return depth; }
    
    virtual double getEstimatedDepth() { return estimatedDepth; }
    
    virtual double getEstimatedDepthDot() { return estimatedDepthDot; }
    
    virtual bool atDepth() { return atDepthValue; }

    virtual ram::math::Vector3 depthUpdate(double depth,
                                           ram::math::Quaternion orientation_)
        {
        updateDepth = depth;
        orientation = orientation_;
        return force; }

    double depth;
    double depthSet;
    double estimatedDepth;
    double estimatedDepthDot;
    double atDepthValue;
    double updateDepth;
    ram::math::Quaternion orientation;
    ram::math::Vector3 force;
};

#endif	// RAM_CONTROL_TEST_DEPTHCONTROLLER_09_01_2008
