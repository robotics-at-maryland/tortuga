/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockTranslationalController.h
 */

#ifndef RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008
#define RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008

// Project Includes
#include "control/include/ITranslationalController.h"
#include "control/include/IController.h"
#include "core/include/ConfigNode.h"

class MockTranslationalController :
    public ram::control::ITranslationalControllerImp
{
public: 
    MockTranslationalController(ram::core::ConfigNode) :
        timestep(0),
        estimator(ram::estimation::IStateEstimatorPtr()),
        desiredState(ram::control::DesiredStatePtr()),
        force(0, 0, 0)
        {}
    
    virtual ~MockTranslationalController() {}

    virtual ram::math::Vector3 translationalUpdate(
        double timestep_,
        ram::estimation::IStateEstimatorPtr estimator_,
        ram::control::DesiredStatePtr desiredState_)
    {
        timestep = timestep_;
        estimator = estimator_;
        desiredState = desiredState_;
        return force;
    }
    
    virtual void setControlMode(ram::control::ControlMode::ModeType mode){}
    virtual ram::control::ControlMode::ModeType getControlMode(){
        return ram::control::ControlMode::OPEN_LOOP;}
    
    double timestep;
    ram::estimation::IStateEstimatorPtr estimator;
    ram::control::DesiredStatePtr desiredState;
    ram::math::Vector3 force;
};

#endif	// RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008
