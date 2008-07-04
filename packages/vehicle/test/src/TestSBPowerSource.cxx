/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestSBPowerSource.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vehicle/include/device/SBPowerSource.h"
#include "vehicle/include/device/SensorBoard.h"

#include "vehicle/test/include/MockVehicle.h"
#include "vehicle/test/include/MockSensorBoard.h"

static const std::string SB_CONFIG(
    "{'name' : 'TestVehicle',"
    "'depthCalibSlope' : 33.01,"
    "'depthCalibIntercept' : 94}");

struct SBPowerSourceFixture
{
    SBPowerSourceFixture() :
        vehicle(new MockVehicle),
        ivehicle(vehicle),
        sensorBoard(new MockSensorBoard(
                        ram::core::ConfigNode::fromString(SB_CONFIG))),
        powerSource(0)
    {
        vehicle->devices["SensorBoard"] =
            ram::vehicle::device::SensorBoardPtr(sensorBoard);
    }
    
    ~SBPowerSourceFixture()
    {
    }

    MockVehicle* vehicle;
    ram::vehicle::IVehiclePtr ivehicle;
    MockSensorBoard* sensorBoard;
    ram::vehicle::device::IPowerSource* powerSource;
};

TEST_FIXTURE(SBPowerSourceFixture, initialization)
{
    sensorBoard->powerSourceEnables[3] = true;
    sensorBoard->powerSourceUsed[3] = true;
    
    powerSource = new ram::vehicle::device::SBPowerSource(
        ram::core::ConfigNode::fromString("{ 'id' : 3 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK(powerSource->isEnabled());
    CHECK(powerSource->inUse());
}

TEST_FIXTURE(SBPowerSourceFixture, getVoltage)
{
    powerSource = new ram::vehicle::device::SBPowerSource(
        ram::core::ConfigNode::fromString("{ 'id' : 1 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL("Batt 2", powerSource->getName());

    // Check default
    CHECK_EQUAL(0.0, powerSource->getVoltage());

    // Publish event and check values
    sensorBoard->publishPowerSourceUpdate(1, false, false, 12.5, 0);
    CHECK_EQUAL(12.5, powerSource->getVoltage());
}

TEST_FIXTURE(SBPowerSourceFixture, getCurrent)
{
    powerSource = new ram::vehicle::device::SBPowerSource(
        ram::core::ConfigNode::fromString("{ 'id' : 2 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL("Batt 3", powerSource->getName());

    // Check default
    CHECK_EQUAL(0.0, powerSource->getCurrent());

    // Publish event and check values
    sensorBoard->publishPowerSourceUpdate(2, false, false, 0, 18.5);
    CHECK_EQUAL(18.5, powerSource->getCurrent());
}

TEST_FIXTURE(SBPowerSourceFixture, isEnabled)
{
    powerSource = new ram::vehicle::device::SBPowerSource(
        ram::core::ConfigNode::fromString("{ 'id' : 4 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL("Shore", powerSource->getName());

    // Check default
//    CHECK_EQUAL(false, powerSource->isEnabled());

    // Publish event and check values
    sensorBoard->publishPowerSourceUpdate(4, true, false, 0, 0);
    CHECK_EQUAL(true, powerSource->isEnabled());
}

TEST_FIXTURE(SBPowerSourceFixture, used)
{
    powerSource = new ram::vehicle::device::SBPowerSource(
        ram::core::ConfigNode::fromString("{ 'id' : 2 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL("Batt 3", powerSource->getName());

    // Check default
    CHECK_EQUAL(false, powerSource->inUse());

    // Publish event and check values
    sensorBoard->publishPowerSourceUpdate(2, true, true, 0, 0);
    CHECK_EQUAL(true, powerSource->inUse());
}

TEST_FIXTURE(SBPowerSourceFixture, updateID)
{
    powerSource = new ram::vehicle::device::SBPowerSource(
        ram::core::ConfigNode::fromString("{ 'id' : 3 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL("Batt 4", powerSource->getName());

    // Check default
    CHECK_EQUAL(false, powerSource->isEnabled());

    // Publish event and make sure it didn't change
    sensorBoard->publishPowerSourceUpdate(4, true, false, 0, 0);
    CHECK_EQUAL(false, powerSource->isEnabled());
}
