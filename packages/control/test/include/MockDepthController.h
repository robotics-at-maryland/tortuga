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
        timestep(0),
        estimator(ram::estimation::IStateEstimatorPtr()),
        desiredState(ram::control::DesiredStatePtr()),
        force(0, 0, 0),
        holdCurrentDepthCount(0)
        {}
    
    virtual ~MockDepthController() {}

    virtual ram::math::Vector3 depthUpdate(
        double timestep_,
        ram::estimation::IStateEstimatorPtr estimator_,
        ram::control::DesiredStatePtr desiredState_)
    { 
        timestep = timestep_;
        estimator = estimator_;
        desiredState = desiredState_;
        return force; 
    }

    double timestep;
    ram::estimation::IStateEstimatorPtr estimator;
    ram::control::DesiredStatePtr desiredState;
    ram::math::Vector3 force;
    int holdCurrentDepthCount;
};

#endif	// RAM_CONTROL_TEST_DEPTHCONTROLLER_09_01_2008
