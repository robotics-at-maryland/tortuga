/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/test/src/TestStateEstimator.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "estimation/test/include/MockEventBasedEstimator.h"
#include "core/include/EventHub.h"
#include "core/include/Event.h"

#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IVelocitySensor.h"
#include "vehicle/include/Events.h"

using namespace ram;

struct EstimatorFixture
{
    EstimatorFixture() :
        eventHub(core::EventHubPtr(new core::EventHub("eventHub"))),
        estimatedState(new estimation::EstimatedState(
                           core::ConfigNode::fromString("{}"), 
                           eventHub)),
        estimator(new MockEventBasedEstimator(core::ConfigNode::fromString("{}"), 
                                              eventHub))
    {}
                  

    core::EventHubPtr eventHub;
    estimation::EstimatedState* estimatedState;
    MockEventBasedEstimator* estimator;
};

TEST_FIXTURE(EstimatorFixture, rawUpdate_DVL)
{
    CHECK_EQUAL(estimator->rawUpdateCount_DVL, 0);

    vehicle::RawDVLDataEventPtr event = 
        vehicle::RawDVLDataEventPtr(new vehicle::RawDVLDataEvent());
    eventHub->publish(vehicle::device::IVelocitySensor::RAW_UPDATE, event);

    CHECK_EQUAL(estimator->rawUpdateCount_DVL, 1);
}

TEST_FIXTURE(EstimatorFixture, rawUpdate_IMU)
{
    CHECK_EQUAL(estimator->rawUpdateCount_IMU, 0);

    vehicle::RawIMUDataEventPtr event = 
        vehicle::RawIMUDataEventPtr(new vehicle::RawIMUDataEvent());
    eventHub->publish(vehicle::device::IIMU::RAW_UPDATE, event);

    CHECK_EQUAL(estimator->rawUpdateCount_IMU, 1);
}

TEST_FIXTURE(EstimatorFixture, rawUpdate_DepthSensor)
{
    CHECK_EQUAL(estimator->rawUpdateCount_Depth, 0);

    vehicle::RawDepthSensorDataEventPtr event = 
        vehicle::RawDepthSensorDataEventPtr(new vehicle::RawDepthSensorDataEvent());
    eventHub->publish(vehicle::device::IDepthSensor::RAW_UPDATE, event);

    CHECK_EQUAL(estimator->rawUpdateCount_Depth, 1);
}
