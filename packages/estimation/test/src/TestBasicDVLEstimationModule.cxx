/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/test/src/TestBasicDVLEstimationModule.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "estimation/include/modules/BasicDVLEstimationModule.h"
#include "estimation/include/EstimatedState.h"
#include "vehicle/include/Events.h"
#include "core/include/EventHub.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector2.h"

using namespace ram;

TEST(dvlUpdate1)
{
    core::EventHubPtr eventHub = core::EventHubPtr(
        new core::EventHub("eventHub"));

    estimation::EstimatedStatePtr estimatedState = 
        estimation::EstimatedStatePtr(
            new estimation::EstimatedState(core::ConfigNode::fromString("{}"),
                                           eventHub));

    vehicle::RawDVLDataEventPtr event =
        vehicle::RawDVLDataEventPtr(
            new vehicle::RawDVLDataEvent());

    event->timestep = 1;
    event->velocity_b = math::Vector2(1,2);
    event->name = "MockDVLData";

    estimation::EstimationModulePtr module = 
        estimation::EstimationModulePtr(
            new estimation::BasicDVLEstimationModule(
                core::ConfigNode::fromString("{}"),
                eventHub,
                estimatedState));
    
    estimatedState->setEstimatedOrientation(math::Quaternion::IDENTITY);
    module->update(event);
    
    CHECK_CLOSE(1.0, estimatedState->getEstimatedVelocity()[0], 0.0001);
    CHECK_CLOSE(2.0, estimatedState->getEstimatedVelocity()[1], 0.0001);
    
    CHECK_CLOSE(0.5, estimatedState->getEstimatedPosition()[0], 0.0001);
    CHECK_CLOSE(1.0, estimatedState->getEstimatedPosition()[1], 0.0001);
}

TEST(dvlUpdate2)
{
    core::EventHubPtr eventHub = core::EventHubPtr(
        new core::EventHub("eventHub"));

    estimation::EstimatedStatePtr estimatedState = 
        estimation::EstimatedStatePtr(
            new estimation::EstimatedState(core::ConfigNode::fromString("{}"),
                                           eventHub));

    vehicle::RawDVLDataEventPtr event =
        vehicle::RawDVLDataEventPtr(
            new vehicle::RawDVLDataEvent());

    event->timestep = 1;
    event->velocity_b = math::Vector2(1,0);
    event->name = "MockDVLData";

    estimation::EstimationModulePtr module = 
        estimation::EstimationModulePtr(
            new estimation::BasicDVLEstimationModule(
                core::ConfigNode::fromString("{}"),
                eventHub,
                estimatedState));
    
    // yawed at 90 degrees
    math::Quaternion orientation(math::Degree(90), math::Vector3::UNIT_Z);
    estimatedState->setEstimatedOrientation(orientation);
    module->update(event);
    
    CHECK_CLOSE(0.0, estimatedState->getEstimatedVelocity()[0], 0.0001);
    CHECK_CLOSE(-1.0, estimatedState->getEstimatedVelocity()[1], 0.0001);
    
    CHECK_CLOSE(0.0, estimatedState->getEstimatedPosition()[0], 0.0001);
    CHECK_CLOSE(-0.5, estimatedState->getEstimatedPosition()[1], 0.0001);
}

TEST(dvlUpdate3)
{
    core::EventHubPtr eventHub = core::EventHubPtr(
        new core::EventHub("eventHub"));

    estimation::EstimatedStatePtr estimatedState = 
        estimation::EstimatedStatePtr(
            new estimation::EstimatedState(core::ConfigNode::fromString("{}"),
                                           eventHub));

    vehicle::RawDVLDataEventPtr event =
        vehicle::RawDVLDataEventPtr(
            new vehicle::RawDVLDataEvent());

    event->timestep = 1;
    event->velocity_b = math::Vector2(1,0);
    event->name = "MockDVLData";

    estimation::EstimationModulePtr module = 
        estimation::EstimationModulePtr(
            new estimation::BasicDVLEstimationModule(
                core::ConfigNode::fromString("{}"),
                eventHub,
                estimatedState));
    
    // pitched 90 degrees (shouldnt have any in plane velocity)
    math::Quaternion orientation(math::Degree(90), math::Vector3::UNIT_Y);
    estimatedState->setEstimatedOrientation(orientation);
    module->update(event);
    
    CHECK_CLOSE(0.0, estimatedState->getEstimatedVelocity()[0], 0.0001);
    CHECK_CLOSE(0.0, estimatedState->getEstimatedVelocity()[1], 0.0001);
    
    CHECK_CLOSE(0.0, estimatedState->getEstimatedPosition()[0], 0.0001);
    CHECK_CLOSE(0.0, estimatedState->getEstimatedPosition()[1], 0.0001);
}
