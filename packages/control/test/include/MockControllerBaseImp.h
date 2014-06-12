/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockControlBaseImp.h
 */


#ifndef RAM_CONTROL_TEST_MOCKCONTROLLERBASEIMP_02_28_2009
#define RAM_CONTROL_TEST_MOCKCONTROLLERBASEIMP_02_28_2009

// STD Includes
#include <assert.h>

// Project Includes
#include "core/include/EventHub.h"
#include "control/include/ControllerBase.h"
#include "core/include/ConfigNode.h"

class MockControllerBaseImp : public ram::control::ControllerBase
{
public:
    MockControllerBaseImp(ram::core::EventHubPtr eventHub,
                          ram::vehicle::IVehiclePtr vehicle,
                          ram::estimation::IStateEstimatorPtr estimator,
                          ram::core::ConfigNode config) :
        ram::control::ControllerBase(eventHub, vehicle, estimator, config),
        translationalForceOut(0,0,0),
        rotationalTorqueOut(0,0,0)
    {
    }
    
    virtual ~MockControllerBaseImp() {}
            
    virtual void doUpdate(const double& timestep_,
                          ram::math::Vector3& translationalForceOut_,
                          ram::math::Vector3& rotationalTorqueOut_)
    {
        timestep = timestep_;
        translationalForceOut_ = translationalForceOut;
        rotationalTorqueOut_ = rotationalTorqueOut;
    }

    double timestep;
    ram::math::Vector3 translationalForceOut;
    ram::math::Vector3 rotationalTorqueOut;
};

#endif	// RAM_CONTROL_TEST_MOCKCONTROLLERBASEIMP_02_28_2009
