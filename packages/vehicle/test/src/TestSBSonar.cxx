/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestSBSonar.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/device/SBSonar.h"
#include "vehicle/include/device/SensorBoard.h"

#include "vehicle/test/include/MockVehicle.h"
#include "vehicle/test/include/MockSensorBoard.h"

#include "math/test/include/MathChecks.h"

static const std::string SB_CONFIG(
    "{'name' : 'TestVehicle',"
    "'depthCalibSlope' : 33.01,"
    "'depthCalibIntercept' : 94}");

struct SBSonarFixture
{
    SBSonarFixture() :
        vehicle(new MockVehicle),
        ivehicle(vehicle),
        sensorBoard(new MockSensorBoard(
                        ram::core::ConfigNode::fromString(SB_CONFIG))),
        sonar(0)
    {
        vehicle->devices["SensorBoard"] =
            ram::vehicle::device::SensorBoardPtr(sensorBoard);
    }
    
    ~SBSonarFixture()
    {
        // Don't delete vehicle, sensorBoard, handled by smart pointers
    }

    MockVehicle* vehicle;
    ram::vehicle::IVehiclePtr ivehicle;
    MockSensorBoard* sensorBoard;
    ram::vehicle::device::ISonar* sonar;
};

TEST_FIXTURE(SBSonarFixture, getDirection)
{
    sonar = new ram::vehicle::device::SBSonar(
        ram::core::ConfigNode::fromString("{ 'id' : 0 }"),
        ram::core::EventHubPtr(), ivehicle);

    // Check default
    CHECK_EQUAL(ram::math::Vector3::ZERO, sonar->getDirection());

    // Publish event and check values
    ram::math::Vector3 expectedDir(362.2, 12.5, 90.2);
    sensorBoard->publishSonarUpdate(expectedDir, 0, 0, 0);
    CHECK_EQUAL(expectedDir, sonar->getDirection());
    delete sonar;
}

TEST_FIXTURE(SBSonarFixture, getRange)
{
    sonar = new ram::vehicle::device::SBSonar(
        ram::core::ConfigNode::fromString("{ 'id' : 0 }"),
        ram::core::EventHubPtr(), ivehicle);

    // Check default
    CHECK_EQUAL(0.0, sonar->getRange());

    // Publish event and check values
    double expectedRange = 23;
    sensorBoard->publishSonarUpdate(ram::math::Vector3::ZERO,
                                    (int)expectedRange, 0, 0);
    CHECK_EQUAL(expectedRange, sonar->getRange());
    delete sonar;
}

TEST_FIXTURE(SBSonarFixture, getPingTime)
{
    sonar = new ram::vehicle::device::SBSonar(
        ram::core::ConfigNode::fromString("{ 'id' : 6 }"),
        ram::core::EventHubPtr(), ivehicle);

    // Check default
    CHECK_EQUAL(0, sonar->getPingTime().seconds());
    CHECK_EQUAL(0, sonar->getPingTime().microseconds());

    // Publish event and make sure it didn't change
    ram::core::TimeVal expectedTime(123, 2820);
    sensorBoard->publishSonarUpdate(ram::math::Vector3::ZERO, 0,
                                    expectedTime.seconds(),
                                    expectedTime.microseconds());
    CHECK_EQUAL(expectedTime.seconds(),
                sonar->getPingTime().seconds());
    CHECK_EQUAL(expectedTime.microseconds(),
                sonar->getPingTime().microseconds());
    delete sonar;
}

typedef std::vector<ram::vehicle::SonarEventPtr>
SonarEventPtrList;

void sonarUpdateHelper2(SonarEventPtrList* list,
                       ram::core::EventPtr event)
{
    list->push_back(boost::dynamic_pointer_cast<
                    ram::vehicle::SonarEvent>(event));
}

TEST_FIXTURE(SBSonarFixture, event_UPDATE)
{
    sonar = new ram::vehicle::device::SBSonar(
        ram::core::ConfigNode::fromString("{ 'id' : 6 }"),
        ram::core::EventHubPtr(), ivehicle);

    // Check default
    CHECK_EQUAL(0, sonar->getPingTime().seconds());
    CHECK_EQUAL(0, sonar->getPingTime().microseconds());

    // Register for the event
    SonarEventPtrList eventList;
    ram::core::EventConnectionPtr conn = sonar->subscribe(
        ram::vehicle::device::ISonar::UPDATE,
        boost::bind(sonarUpdateHelper2, &eventList, _1));
    
    // Publish event and make sure it didn't change
    ram::math::Vector3 expectedDir(362.2, 12.5, 90.2);
    int expectedRange = 23;
    ram::core::TimeVal expectedTime(123, 2820);

    sensorBoard->publishSonarUpdate(expectedDir, expectedRange,
                                     expectedTime.seconds(),
                                     expectedTime.microseconds());

    // Check results
    CHECK_EQUAL(1u, eventList.size());
    ram::vehicle::SonarEventPtr event = eventList[0];
    CHECK_EQUAL(expectedDir, event->direction);
    CHECK_EQUAL(expectedRange, event->range);
    CHECK_EQUAL(expectedTime.seconds(), event->pingTimeSec);
    CHECK_EQUAL(expectedTime.microseconds(), event->pingTimeUSec);
    
    delete sonar;
}

