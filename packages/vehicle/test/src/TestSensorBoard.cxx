/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestSensorBoard.cxx
 */

// STD Includes
#include <vector>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/Events.h"

#include "core/include/EventConnection.h"

class TestSensorBoard : public ram::vehicle::device::SensorBoard
{
public:
    TestSensorBoard(ram::core::ConfigNode config,
                    ram::core::EventHubPtr eventHub = ram::core::EventHubPtr()) :
        SensorBoard(-1, config, eventHub),
        updateDone(false),
        thrusterState(0),
        markerDropped(-1)
    {
        memset(speeds, 0, sizeof(int) * 6);
        memset(&currentTelemetry, 0, sizeof(struct boardInfo));
    }

    int speeds[6];
    struct boardInfo currentTelemetry;
    bool updateDone;
    int depth;
    int thrusterState;
    int markerDropped;
    
protected:
    virtual void setSpeeds(int s1, int s2, int s3, int s4, int s5, int s6)
    {
        speeds[0] = s1;
        speeds[1] = s2;
        speeds[2] = s3;
        speeds[3] = s4;
        speeds[4] = s5;
        speeds[5] = s6;
    }
    
    virtual int partialRead(struct boardInfo* telemetry)
    {
        *telemetry = currentTelemetry;
        if (updateDone)
            return SB_UPDATEDONE;
        else
            return SB_OK;
    }

    virtual int readDepth() { return depth; }

    virtual void setThrusterSafety(int state) { thrusterState = state; }

    virtual void dropMarker(int markerNum) { markerDropped = markerNum; }

    // Does nothing
    virtual void syncBoard() {}    
    
};

struct SensorBoardFixture
{
};

const std::string BASE_CONFIG = "{ 'name' : 'SensorBoard',";
const std::string BLANK_CONFIG = BASE_CONFIG + "'depthCalibSlope' : 1,"
        "'depthCalibIntercept' : 1}";

#define LENGTH(X) (sizeof(X)/sizeof(*X))

TEST_FIXTURE(SensorBoardFixture, getDepth)
{
    std::string config = BASE_CONFIG + "'depthCalibSlope' : 5,"
        "'depthCalibIntercept' : 0}";

    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(config));

    // This makes the zero point 15 counts
    int calibDepths[] = {5,10,15,20,25};
    for (size_t i = 0; i < LENGTH(calibDepths); ++i)
    {
        sb->depth = calibDepths[i];
        sb->update(0);
    }

    sb->depth = 15 + 5;;
    sb->update(0);
    CHECK_EQUAL(1.0, sb->getDepth());
}

TEST_FIXTURE(SensorBoardFixture, setThrusterValue)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    int speeds[6] = {238, -519, 23, 1011, -900, 758};
    for (size_t i = 0; i < LENGTH(speeds); ++i)
        sb->setThrusterValue(i, speeds[i]);

    // Send speeds to the board
    sb->update(0);

    // Now check them
    CHECK_ARRAY_EQUAL(speeds, sb->speeds, 6);
}

TEST_FIXTURE(SensorBoardFixture, isThrusterEnabled)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    // There must be a better way to test this...
    int num2Enables[6] = {
        THRUSTER1_ENABLED,
        THRUSTER2_ENABLED,
        THRUSTER3_ENABLED,
        THRUSTER4_ENABLED,
        THRUSTER5_ENABLED,
        THRUSTER6_ENABLED
    };

    for (size_t i = 0; i < LENGTH(num2Enables); ++i)
    {
        bool enables[6] = {false, false, false, false, false, false};
        enables[i] = true;
        
        // Set values, read them from fake board
        sb->currentTelemetry.thrusterState = num2Enables[i];
        sb->update(0);

        // Check results
        for (size_t j = 0; j < LENGTH(enables); ++j)
        {
            CHECK_EQUAL(enables[j], sb->isThrusterEnabled(j));
        }
    }
}

TEST_FIXTURE(SensorBoardFixture, setThrusterSafety)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));
    
    int num2On[6] = {
        CMD_THRUSTER1_ON,
        CMD_THRUSTER2_ON,
        CMD_THRUSTER3_ON,
        CMD_THRUSTER4_ON,
        CMD_THRUSTER5_ON,
        CMD_THRUSTER6_ON
    };

    for (size_t i = 0; i < LENGTH(num2On); ++i)
    {
        sb->setThrusterEnable(i, true);
        CHECK_EQUAL(num2On[i], sb->thrusterState);
    }
    
    int num2Off[6] = {
        CMD_THRUSTER1_OFF,
        CMD_THRUSTER2_OFF,
        CMD_THRUSTER3_OFF,
        CMD_THRUSTER4_OFF,
        CMD_THRUSTER5_OFF,
        CMD_THRUSTER6_OFF
    };

    for (size_t i = 0; i < LENGTH(num2Off); ++i)
    {
        sb->setThrusterEnable(i, false);
        CHECK_EQUAL(num2Off[i], sb->thrusterState);
    }
}

TEST_FIXTURE(SensorBoardFixture, dropMarker)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    ((ram::vehicle::device::SensorBoard*)sb)->dropMarker();
    CHECK_EQUAL(0, sb->markerDropped);
    ((ram::vehicle::device::SensorBoard*)sb)->dropMarker();
    CHECK_EQUAL(1, sb->markerDropped);
}

typedef std::vector<ram::vehicle::PowerSourceEventPtr>
PowerSourceEventPtrList;

void powerSourceUpdateHelper(PowerSourceEventPtrList* list,
                             ram::core::EventPtr event)
{
    list->push_back(boost::dynamic_pointer_cast<
                    ram::vehicle::PowerSourceEvent>(event));
}

TEST_FIXTURE(SensorBoardFixture, event_POWERSOURCE_UPDATE)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    float expectedVoltages[5] = {9.3, 8.6, 5.7, 3.3, 0};
    float expectedCurrents[5] = {2.23, 16.2, 5.3, 2.6, 0};
    bool expectedEnables[5] = {true, false, false, true, false};

    // Set values to be returned
    sb->updateDone = true;
    memcpy(expectedVoltages, &(sb->currentTelemetry.powerInfo.battVoltages),
           sizeof(expectedVoltages));
    memcpy(expectedCurrents, &(sb->currentTelemetry.powerInfo.battCurrents),
           sizeof(expectedCurrents));
    sb->currentTelemetry.battEnabled = BATT1_ENABLED | BATT4_ENABLED;

    PowerSourceEventPtrList eventList;
    ram::core::EventConnectionPtr conn = sb->subscribe(
        ram::vehicle::device::SensorBoard::POWERSOURCE_UPDATE,
        boost::bind(powerSourceUpdateHelper, &eventList, _1));
    sb->update(0);

    CHECK_EQUAL(5u, eventList.size());

    
    float actualVoltages[5] = {0};
    float actualCurrents[5] = {0};
    bool actualEnables[5] = {0};
    
    for (size_t i = 0; i < LENGTH(expectedVoltages); ++i)
    {
        actualVoltages[i] = eventList[i]->voltage;
        actualCurrents[i] = eventList[i]->current;
        actualEnables[i] = eventList[i]->enabled;
    }

    CHECK_ARRAY_EQUAL(expectedVoltages, actualVoltages, 5);
    CHECK_ARRAY_EQUAL(expectedCurrents, actualCurrents, 5);
    CHECK_ARRAY_EQUAL(expectedEnables, actualEnables, 5);
    
    conn->disconnect();
}

