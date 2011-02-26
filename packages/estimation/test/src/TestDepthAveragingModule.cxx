/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/test/src/TestDepthAveragingModule.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "estimation/include/modules/DepthAveragingModule.h"
#include "estimation/include/EstimatedState.h"
#include "vehicle/include/Events.h"
#include "core/include/EventHub.h"

using namespace ram;

TEST(depthAveragingUpdate1)
{
    core::EventHubPtr eventHub = core::EventHubPtr(
        new core::EventHub("eventHub"));

    estimation::EstimatedStatePtr estimatedState = 
        estimation::EstimatedStatePtr(
            new estimation::EstimatedState(core::ConfigNode::fromString("{}"),
                                           eventHub));

    vehicle::RawDepthSensorDataEventPtr event =
        vehicle::RawDepthSensorDataEventPtr(
            new vehicle::RawDepthSensorDataEvent());

    event->sensorLocation = math::Vector3(0, 0, 0);
    event->rawDepth = 4.0;
    event->timestep = 0.2;
    event->name = "depthSensor";

    estimation::EstimationModulePtr module = 
        estimation::EstimationModulePtr(
            new estimation::DepthAveragingModule(
                core::ConfigNode::fromString("{}"),
                eventHub, estimatedState));
        
    module->update(event);
    
    CHECK_CLOSE(estimatedState->getEstimatedDepth(), 4.0, 0.0001);
}

TEST(depthAveragingUpdate2)
{
    core::EventHubPtr eventHub = core::EventHubPtr(
        new core::EventHub("eventHub"));

    estimation::EstimatedStatePtr estimatedState = 
        estimation::EstimatedStatePtr(
            new estimation::EstimatedState(core::ConfigNode::fromString("{}"),
                                           eventHub));

    vehicle::RawDepthSensorDataEventPtr event =
        vehicle::RawDepthSensorDataEventPtr(
            new vehicle::RawDepthSensorDataEvent());

    // The sensor is in the back, left, and upper corner of the vehicle
    event->sensorLocation = math::Vector3(-1, -0.2, 0.2);
    event->rawDepth = 2.6;
    event->timestep = 0.2;
    event->name = "depthSensor";

    // We are pitched forward down by 15 degrees
    estimatedState->setEstimatedOrientation(
        math::Quaternion(math::Degree(15), math::Vector3::UNIT_Y));

    estimation::EstimationModulePtr module = 
        estimation::EstimationModulePtr(
            new estimation::DepthAveragingModule(
                core::ConfigNode::fromString("{}"),
                eventHub,estimatedState));
        
    module->update(event);
    
    // We add to the expected depth because the downward pitch moves our sensor
    // to a shallow depth then we are really at
    CHECK_CLOSE(estimatedState->getEstimatedDepth(), 2.6 + 0.252, 0.0001);
}
