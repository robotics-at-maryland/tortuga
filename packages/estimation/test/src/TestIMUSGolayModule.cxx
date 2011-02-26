/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/test/src/TestIMUSGolayModule.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "estimation/include/modules/IMUSGolayModule.h"
#include "estimation/include/EstimatedState.h"
#include "vehicle/include/Events.h"
#include "core/include/EventHub.h"

using namespace ram;

TEST(imuSGolayUpdate1)
{
    core::EventHubPtr eventHub = core::EventHubPtr(
        new core::EventHub("eventHub"));

    estimation::EstimatedStatePtr estimatedState = 
        estimation::EstimatedStatePtr(
            new estimation::EstimatedState(core::ConfigNode::fromString("{}"),
                                           eventHub));

    RawIMUData rawIMUData;
    rawIMUData.gyroX = 1;
    rawIMUData.gyroY = 1;
    rawIMUData.gyroZ = 1;
    
    rawIMUData.accelX = 1;
    rawIMUData.accelY = 1;
    rawIMUData.accelZ = 1;

    rawIMUData.magX = 1;
    rawIMUData.magY = 1;
    rawIMUData.magZ = 1;

    vehicle::RawIMUDataEventPtr cgEvent = vehicle::RawIMUDataEventPtr(
        new vehicle::RawIMUDataEvent());

    cgEvent->name = "IMU";
    cgEvent->rawIMUData = rawIMUData;
    cgEvent->timestep = 1;
    cgEvent->magIsCorrupt = false;

    vehicle::RawIMUDataEventPtr magEvent = vehicle::RawIMUDataEventPtr(
        new vehicle::RawIMUDataEvent());

    magEvent->name = "MagBoom";
    magEvent->rawIMUData = rawIMUData;
    magEvent->timestep = 1;
    magEvent->magIsCorrupt = false;


    estimation::EstimationModulePtr module = 
        estimation::EstimationModulePtr(
            new estimation::IMUSGolayModule(
                core::ConfigNode::fromString("{}"),
                eventHub, estimatedState));

    // right now just make sure that we dont crash, the algorithms are being
    // tested elsewhere this is mostly just to make sure that the filters
    // and maps are accessed correctly
    for(int i = 0; i < 25; i++)
    {
        module->update(cgEvent);
        module->update(magEvent);
    }
}
