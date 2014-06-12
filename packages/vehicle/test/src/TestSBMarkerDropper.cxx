/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestSBMarkerDropper.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/device/SBMarkerDropper.h"
#include "vehicle/include/device/SensorBoard.h"

#include "vehicle/test/include/MockVehicle.h"
#include "vehicle/test/include/MockSensorBoard.h"

#include "math/test/include/MathChecks.h"

static const std::string SB_CONFIG(
    "{'name' : 'TestVehicle',"
    "'depthCalibSlope' : 33.01,"
    "'depthCalibIntercept' : 94}");

static const std::string BLANK_CONFIG("{}");

struct SBMarkerDropperFixture
{
    SBMarkerDropperFixture() :
        vehicle(new MockVehicle),
        ivehicle(vehicle),
        sensorBoard(new MockSensorBoard(
                        ram::core::ConfigNode::fromString(SB_CONFIG))),
        dropper(0)
    {
        vehicle->devices["SensorBoard"] =
            ram::vehicle::device::SensorBoardPtr(sensorBoard);
    }
    
    ~SBMarkerDropperFixture()
    {
        // Don't delete vehicle, sensorBoard, handled by smart pointers
    }

    MockVehicle* vehicle;
    ram::vehicle::IVehiclePtr ivehicle;
    MockSensorBoard* sensorBoard;
    ram::vehicle::device::IPayloadSet* dropper;
};

TEST_FIXTURE(SBMarkerDropperFixture, releaseObject)
{
    dropper = new ram::vehicle::device::SBMarkerDropper(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    sensorBoard->markerDropNum = 0;
    dropper->releaseObject();
}

TEST_FIXTURE(SBMarkerDropperFixture, objectCount)
{
    dropper = new ram::vehicle::device::SBMarkerDropper(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL(MockSensorBoard::NUMBER_OF_MARKERS,
                dropper->initialObjectCount());

    int expectedCount = dropper->initialObjectCount() - 1;

    // Release an object
    sensorBoard->markerDropNum = 0;
    dropper->releaseObject();

    // Check the count
    CHECK_EQUAL(expectedCount, dropper->objectCount());
}

TEST_FIXTURE(SBMarkerDropperFixture, initialObjectCount)
{
    dropper = new ram::vehicle::device::SBMarkerDropper(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    CHECK_EQUAL(MockSensorBoard::NUMBER_OF_MARKERS,
                dropper->initialObjectCount());
    CHECK_EQUAL(dropper->objectCount(), dropper->initialObjectCount());
}

typedef std::vector<ram::core::EventPtr>
MarkerDropperEventPtrList;

void markerDroppedHelper(MarkerDropperEventPtrList* list,
                          ram::core::EventPtr event)
{
    list->push_back(event);
}

TEST_FIXTURE(SBMarkerDropperFixture, event_OBJECT_RELEASED)
{
    dropper = new ram::vehicle::device::SBMarkerDropper(
        ram::core::ConfigNode::fromString(BLANK_CONFIG),
        ram::core::EventHubPtr(), ivehicle);

    // Register for the event
    MarkerDropperEventPtrList eventList;
    ram::core::EventConnectionPtr conn = dropper->subscribe(
        ram::vehicle::device::IPayloadSet::OBJECT_RELEASED,
        boost::bind(markerDroppedHelper, &eventList, _1));
    
    // Real marker drops
    sensorBoard->markerDropNum = 0;
    dropper->releaseObject();
    CHECK_EQUAL(1u, eventList.size());
    
    sensorBoard->markerDropNum = 1;
    dropper->releaseObject();
    CHECK_EQUAL(2u, eventList.size());

    // Now a bad marker drop (should be no event)
    sensorBoard->markerDropNum = -1;
    dropper->releaseObject();
    CHECK_EQUAL(2u, eventList.size());
    
    delete dropper;
}
