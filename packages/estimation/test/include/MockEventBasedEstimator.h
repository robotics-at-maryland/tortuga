/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/test/include/MockEventBasedEstimator.h
 */

#ifndef RAM_ESTIMATION_MOCKEVENTBASEDESTIMATOR_H
#define RAM_ESTIMATION_MOCKEVENTBASEDESTIMATOR_H

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "estimation/include/StateEstimatorBase.h"

#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IVelocitySensor.h"

#include "core/include/EventHub.h"
#include "core/include/EventConnection.h"
#include "core/include/Event.h"

using namespace ram;

class MockEventBasedEstimator : public estimation::StateEstimatorBase
{
public:
    MockEventBasedEstimator(core::ConfigNode config, core::EventHubPtr eventHub) :
        StateEstimatorBase(config, eventHub),
        rawUpdateCount_IMU(0),
        rawUpdateCount_DVL(0),
        rawUpdateCount_Depth(0),
        updateCount_Vision(0),
        updateCount_Sonar(0)
    {
        // Connect the event listeners to their respective events
        if(eventHub != core::EventHubPtr()){
            updateConnection_IMU = eventHub->subscribeToType(
                vehicle::device::IIMU::RAW_UPDATE,
                boost::bind(&MockEventBasedEstimator::rawUpdate_IMU,this, _1));

            updateConnection_DepthSensor = eventHub->subscribeToType(
                vehicle::device::IDepthSensor::RAW_UPDATE,
                boost::bind(&MockEventBasedEstimator::rawUpdate_DepthSensor,this, _1));

            updateConnection_DVL = eventHub->subscribeToType(
                vehicle::device::IVelocitySensor::RAW_UPDATE,
                boost::bind(&MockEventBasedEstimator::rawUpdate_DVL,this, _1));
        }

    }

    ~MockEventBasedEstimator()
    {
        /* unbind the update functions */
        if(updateConnection_IMU)
            updateConnection_IMU->disconnect();

        if(updateConnection_DVL)
            updateConnection_DVL->disconnect();

        if(updateConnection_DepthSensor)
            updateConnection_DepthSensor->disconnect();

        if(updateConnection_Sonar)
            updateConnection_Sonar->disconnect();
    
        if(updateConnection_Vision)
            updateConnection_Vision->disconnect();

    }

    int rawUpdateCount_IMU;
    int rawUpdateCount_DVL;
    int rawUpdateCount_Depth;
    int updateCount_Vision;
    int updateCount_Sonar;

protected:    
    void rawUpdate_DVL(core::EventPtr event)
    {
        rawUpdateCount_DVL++;
    }
    void rawUpdate_IMU(core::EventPtr event)
    {
        rawUpdateCount_IMU++;
    }
    void rawUpdate_DepthSensor(core::EventPtr event)
    {
        rawUpdateCount_Depth++;
    }
    void update_Vision(core::EventPtr event)
    {
        updateCount_Vision++;
    }
    void update_Sonar(core::EventPtr event)
    {
        updateCount_Sonar++;
    }
};

#endif // RAM_ESTIMATION_MOCKEVENTBASEDESTIMATOR_H
