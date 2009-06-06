/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestSBTorpedoLauncher.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/device/SBTorpedoLauncher.h"
#include "vehicle/include/device/SensorBoard.h"

#include "vehicle/test/include/MockVehicle.h"
#include "vehicle/test/include/MockSensorBoard.h"

#include "math/test/include/MathChecks.h"

static const std::string SB_CONFIG(
    "{'name' : 'TestVehicle',"
    "'depthCalibSlope' : 33.01,"
    "'depthCalibIntercept' : 94}");

static const std::string BLANK_CONFIG("{}");

struct SBTorpedoLauncherFixture
{
    SBTorpedoLauncherFixture() :
        vehicle(new MockVehicle),
        ivehicle(vehicle),
        sensorBoard(new MockSensorBoard(
                        ram::core::ConfigNode::fromString(SB_CONFIG))),
        launcher(0)
    {
        vehicle->devices["SensorBoard"] =
            ram::vehicle::device::SensorBoardPtr(sensorBoard);
    }
    
    ~SBTorpedoLauncherFixture()
    {
        // Don't delete vehicle, sensorBoard, handled by smart pointers
    }

    MockVehicle* vehicle;
    ram::vehicle::IVehiclePtr ivehicle;
    MockSensorBoard* sensorBoard;
    ram::vehicle::device::IPayloadSet* launcher;
};

TEST_FIXTURE(SBTorpedoLauncherFixture, releaseObject)
{
    launcher = new ram::vehicle::device::SBTorpedoLauncher(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    sensorBoard->torpedoFireNum = 0;
    launcher->releaseObject();
}

TEST_FIXTURE(SBTorpedoLauncherFixture, objectCount)
{
    launcher = new ram::vehicle::device::SBTorpedoLauncher(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL(MockSensorBoard::NUMBER_OF_TORPEDOS,
                launcher->initialObjectCount());

    int expectedCount = launcher->initialObjectCount() - 1;

    // Release an object
    sensorBoard->torpedoFireNum = 0;
    launcher->releaseObject();

    // Check the count
    CHECK_EQUAL(expectedCount, launcher->objectCount());
}

TEST_FIXTURE(SBTorpedoLauncherFixture, initialObjectCount)
{
    launcher = new ram::vehicle::device::SBTorpedoLauncher(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL(MockSensorBoard::NUMBER_OF_TORPEDOS,
                launcher->initialObjectCount());
    CHECK_EQUAL(launcher->objectCount(), launcher->initialObjectCount());
}

typedef std::vector<ram::core::EventPtr>
TorpedoLauncherEventPtrList;

void torpedoLauncherHelper(TorpedoLauncherEventPtrList* list,
                          ram::core::EventPtr event)
{
    list->push_back(event);
}

TEST_FIXTURE(SBTorpedoLauncherFixture, event_OBJECT_RELEASED)
{
    launcher = new ram::vehicle::device::SBTorpedoLauncher(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    // Register for the event
    TorpedoLauncherEventPtrList eventList;
    ram::core::EventConnectionPtr conn = launcher->subscribe(
        ram::vehicle::device::IPayloadSet::OBJECT_RELEASED,
        boost::bind(torpedoLauncherHelper, &eventList, _1));
    
    // Real marker drops
    sensorBoard->torpedoFireNum = 0;
    launcher->releaseObject();
    CHECK_EQUAL(1u, eventList.size());
    
    sensorBoard->torpedoFireNum = 1;
    launcher->releaseObject();
    CHECK_EQUAL(2u, eventList.size());

    // Now a bad marker drop (should be no event)
    sensorBoard->torpedoFireNum = -1;
    launcher->releaseObject();
    CHECK_EQUAL(2u, eventList.size());
    
    delete launcher;
}
