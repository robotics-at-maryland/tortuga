/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestSBTempSensor.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vehicle/include/device/SBTempSensor.h"
#include "vehicle/include/device/SensorBoard.h"

#include "vehicle/test/include/MockVehicle.h"
#include "vehicle/test/include/MockSensorBoard.h"

static const std::string SB_CONFIG(
    "{'name' : 'TestVehicle',"
    "'depthCalibSlope' : 33.01,"
    "'depthCalibIntercept' : 94}");

struct SBTempSensorFixture
{
    SBTempSensorFixture() :
        vehicle(new MockVehicle),
        ivehicle(vehicle),
        sensorBoard(new MockSensorBoard(
                        ram::core::ConfigNode::fromString(SB_CONFIG))),
        tempSensor(0)
    {
        vehicle->devices["SensorBoard"] =
            ram::vehicle::device::SensorBoardPtr(sensorBoard);
    }
    
    ~SBTempSensorFixture()
    {
        // Don't delete vehicle, sensorBoard, handled by smart pointers
    }

    MockVehicle* vehicle;
    ram::vehicle::IVehiclePtr ivehicle;
    MockSensorBoard* sensorBoard;
    ram::vehicle::device::ITempSensor* tempSensor;
};

TEST_FIXTURE(SBTempSensorFixture, getVoltage)
{
    tempSensor = new ram::vehicle::device::SBTempSensor(
        ram::core::ConfigNode::fromString("{ 'id' : 0 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL("Sensor Board", tempSensor->getName());

    // Check default
    CHECK_EQUAL(0, tempSensor->getTemp());

    // Publish event and check values
    sensorBoard->publishTempSensorUpdate(0, 23);
    CHECK_EQUAL(23, tempSensor->getTemp());
    delete tempSensor;
}

TEST_FIXTURE(SBTempSensorFixture, updateID)
{
    tempSensor = new ram::vehicle::device::SBTempSensor(
        ram::core::ConfigNode::fromString("{ 'id' : 6 }"),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL("Balancer Board", tempSensor->getName());

    // Check default
    CHECK_EQUAL(0, tempSensor->getTemp());

    // Publish event and make sure it didn't change
    sensorBoard->publishTempSensorUpdate(4, 68);
    CHECK_EQUAL(0, tempSensor->getTemp());
    delete tempSensor;
}
