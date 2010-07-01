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
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/Events.h"

#include "core/include/EventConnection.h"
#include "core/test/include/BufferedAppender.h"

#include "math/include/Events.h"
#include "math/test/include/MathChecks.h"

#include "drivers/sensor-r5/include/sensorapi.h"

class TestSensorBoard : public ram::vehicle::device::SensorBoard
{
public:
    TestSensorBoard(ram::core::ConfigNode config,
                    ram::core::EventHubPtr eventHub = ram::core::EventHubPtr()) :
        SensorBoard(-1, config, eventHub),
        updateDone(false),
        thrusterState(0),
        markerDropped(-1),
        servoUsed(-1),
        servoPosition(-1),
        servoEnable(-1),
        servoPower(-1)
    {
        memset(speeds, 0, sizeof(int) * 6);
        memset(&currentTelemetry, 0, sizeof(struct boardInfo));
    }

    int speeds[6];
    struct boardInfo currentTelemetry;
    bool updateDone;
    int depth;
    int thrusterState;
    int batteryState;
    int markerDropped;
    int servoUsed;
    int servoPosition;
    int servoEnable;
    int servoPower;
    
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

    virtual void setBatteryState(int state) { batteryState = state; }

    virtual void dropMarker(int markerNum) { markerDropped = markerNum; }

    virtual void setServoPosition(unsigned char servoNumber,
                                 unsigned short position)
    {
        servoUsed = servoNumber;
        servoPosition = position;
    }

    virtual void setServoEnable(unsigned char mask)
    {
        servoEnable = mask;
    }

    virtual void setServoPower(unsigned char power)
    {
        servoPower = power;
    }

    
    // Does nothing
    virtual void syncBoard() {}
    
};

struct SensorBoardFixture
{
};

const std::string START_CONFIG = "{ 'name' : 'SensorBoard',";
const std::string BASE_CONFIG = START_CONFIG + "'depthCalibSlope' : 1,"
        "'depthCalibIntercept' : 0,";
const std::string BLANK_CONFIG = START_CONFIG + "'depthCalibSlope' : 1,"
        "'depthCalibIntercept' : 0}";
const std::string LOC_CONFIG = START_CONFIG + "'depthCalibSlope' : 1,"
        "'depthSensorLocation' : [1.0, 2.0, 3.0],"
        "'depthCalibIntercept' : 0}";

#define LENGTH(X) (sizeof(X)/sizeof(*X))

TEST_FIXTURE(SensorBoardFixture, getDepth)
{
    std::string config = START_CONFIG + "'depthCalibSlope' : 5,"
        "'depthCalibIntercept' : 2}";

    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(config));

    sb->depth = 17; // (3 * slope) + intercept
    sb->update(0);
    CHECK_CLOSE(3.0, sb->getDepth(), 0.00001);
    delete sb;
}

TEST_FIXTURE(SensorBoardFixture, getLocation)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(LOC_CONFIG));

    ram::math::Vector3 expectedLocation(1, 2, 3);
    CHECK_CLOSE(expectedLocation, sb->getLocation(), 0.0001);

    delete sb;
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
    delete sb;
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
    delete sb;
}

TEST_FIXTURE(SensorBoardFixture, isPowerSourceEnabled)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    // There must be a better way to test this...
    int num2Enables[5] = {
        BATT1_ENABLED,
        BATT2_ENABLED,
        BATT3_ENABLED,
        BATT4_ENABLED,
        BATT5_ENABLED,
    };

    for (size_t i = 0; i < LENGTH(num2Enables); ++i)
    {
        bool enables[5] = {false, false, false, false, false};
        enables[i] = true;
        
        // Set values, read them from fake board
        sb->currentTelemetry.battEnabled = num2Enables[i];
        sb->update(0);

        // Check results
        for (size_t j = 0; j < LENGTH(enables); ++j)
        {
            CHECK_EQUAL(enables[j], sb->isPowerSourceEnabled(j));
        }
    }
    delete sb;
}

TEST_FIXTURE(SensorBoardFixture, isPowerSourceInUse)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    // There must be a better way to test this...
    int num2Enables[5] = {
        BATT1_INUSE,
        BATT2_INUSE,
        BATT3_INUSE,
        BATT4_INUSE,
        BATT5_INUSE,
    };

    for (size_t i = 0; i < LENGTH(num2Enables); ++i)
    {
        bool enables[5] = {false, false, false, false, false};
        enables[i] = true;
        
        // Set values, read them from fake board
        sb->currentTelemetry.battUsed = num2Enables[i];
        sb->update(0);

        // Check results
        for (size_t j = 0; j < LENGTH(enables); ++j)
        {
            CHECK_EQUAL(enables[j], sb->isPowerSourceInUse(j));
        }
    }
    delete sb;
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
    delete sb;
}

TEST_FIXTURE(SensorBoardFixture, setPowerSourceEnabled)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));
    
    int num2On[5] = {
        CMD_BATT1_ON,
        CMD_BATT2_ON,
        CMD_BATT3_ON,
        CMD_BATT4_ON,
        CMD_BATT5_ON,
    };

    for (size_t i = 0; i < LENGTH(num2On); ++i)
    {
        sb->setPowerSouceEnabled(i, true);
        CHECK_EQUAL(num2On[i], sb->batteryState);
    }
    
    int num2Off[5] = {
        CMD_BATT1_OFF,
        CMD_BATT2_OFF,
        CMD_BATT3_OFF,
        CMD_BATT4_OFF,
        CMD_BATT5_OFF,
    };

    for (size_t i = 0; i < LENGTH(num2Off); ++i)
    {
        sb->setPowerSouceEnabled(i, false);
        CHECK_EQUAL(num2Off[i], sb->batteryState);
    }
    delete sb;
}

TEST_FIXTURE(SensorBoardFixture, getMainBusVoltage)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));
    
    // Enable only 3 power sources
    sb->currentTelemetry.battUsed = BATT1_INUSE | BATT2_INUSE | BATT4_INUSE;

    // Set voltages for all
    sb->currentTelemetry.powerInfo.battVoltages[0] = 1.0;
    sb->currentTelemetry.powerInfo.battVoltages[1] = 2.0;
    sb->currentTelemetry.powerInfo.battVoltages[2] = 3.0;
    sb->currentTelemetry.powerInfo.battVoltages[3] = 4.0;
    sb->currentTelemetry.powerInfo.battVoltages[4] = 5.0;

    sb->currentTelemetry.updateState = BATTERY_VOLTAGES;

    // Do an update to read in the values
    sb->update(1.0/40.0);

    // Check average 2.33 ~= (1 + 2 + 4) / 3
    CHECK_CLOSE(7.0/3.0, sb->getMainBusVoltage(), 0.0001);
}

TEST_FIXTURE(SensorBoardFixture, dropMarker)
{
    TestSensorBoard* testSb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));
    ram::vehicle::device::SensorBoard* sb =
        (ram::vehicle::device::SensorBoard*)testSb;

    // Drop first marker
    CHECK_EQUAL(0, sb->dropMarker());
    CHECK_EQUAL(0, testSb->markerDropped);
    // Drop second marker
    CHECK_EQUAL(1, sb->dropMarker());
    CHECK_EQUAL(1, testSb->markerDropped);
    // Drop non-existent marker
    CHECK_EQUAL(-1, sb->dropMarker());
    CHECK_EQUAL(1, testSb->markerDropped);

    delete testSb;
}

TEST_FIXTURE(SensorBoardFixture, fireTorpedo)
{
    TestSensorBoard* testSb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BASE_CONFIG +
            "'servo1FirePosition' : 8000,"
            "'servo2FirePosition' : 7000}"));
    ram::vehicle::device::SensorBoard* sb =
        (ram::vehicle::device::SensorBoard*)testSb;

    // Fire first torpedo
    CHECK_EQUAL(0, sb->fireTorpedo());
    CHECK_EQUAL(SERVO_1, testSb->servoUsed);
    CHECK_EQUAL(8000, testSb->servoPosition);
    CHECK_EQUAL(SERVO_ENABLE_1, testSb->servoEnable);
    CHECK_EQUAL(SERVO_POWER_ON, testSb->servoPower);
    // Fire second torpedo
    CHECK_EQUAL(1, sb->fireTorpedo());
    CHECK_EQUAL(SERVO_2, testSb->servoUsed);
    CHECK_EQUAL(7000, testSb->servoPosition);
    CHECK_EQUAL(SERVO_ENABLE_2, testSb->servoEnable);
    CHECK_EQUAL(SERVO_POWER_ON, testSb->servoPower);

    
    // Fire non-existent torpedo
    CHECK_EQUAL(-1, sb->fireTorpedo());
    CHECK_EQUAL(SERVO_2, testSb->servoUsed);
    CHECK_EQUAL(7000, testSb->servoPosition);

    delete testSb;
}

TEST_FIXTURE(SensorBoardFixture, releaseGrabber)
{
    TestSensorBoard* testSb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BASE_CONFIG +
            "'servo3FirePosition' : 6000}"));
    ram::vehicle::device::SensorBoard* sb =
        (ram::vehicle::device::SensorBoard*)testSb;

    // Release Grabber
    CHECK_EQUAL(0, sb->releaseGrabber());
    CHECK_EQUAL(SERVO_3, testSb->servoUsed);
    CHECK_EQUAL(6000, testSb->servoPosition);
    CHECK_EQUAL(SERVO_ENABLE_3, testSb->servoEnable);
    CHECK_EQUAL(SERVO_POWER_ON, testSb->servoPower);
    
    // Fire non-existent torpedo
    CHECK_EQUAL(-1, sb->releaseGrabber());
    CHECK_EQUAL(SERVO_3, testSb->servoUsed);
    CHECK_EQUAL(6000, testSb->servoPosition);

    delete testSb;
}

typedef std::vector<ram::math::NumericEventPtr>
DepthSensorEventPtrList;

void depthSensorUpdateHelper(DepthSensorEventPtrList* list,
                             ram::core::EventPtr event)
{
    list->push_back(boost::dynamic_pointer_cast<
                    ram::math::NumericEvent>(event));
}

TEST_FIXTURE(SensorBoardFixture, event_DEPTH_UPDATE)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    double expectedDepth = 6;

    // Set values to be returned
    sb->updateDone = true;
    sb->depth = (int)expectedDepth;

    // Register handler and trigger and update
    DepthSensorEventPtrList eventList;
    ram::core::EventConnectionPtr conn = sb->subscribe(
        ram::vehicle::device::IDepthSensor::UPDATE,
        boost::bind(depthSensorUpdateHelper, &eventList, _1));
    sb->update(0);

    // Check to make sure we got the proper number of updates
    CHECK_EQUAL(1u, eventList.size());

    // Make sure the values were correct
    CHECK_CLOSE(expectedDepth, eventList[0]->number, 0.00001);
    
    conn->disconnect();
    delete sb;
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
    bool expectedUsed[5] = {false, true, true, false, false};

    // Set values to be returned
    sb->updateDone = true;
    memcpy(expectedVoltages, &(sb->currentTelemetry.powerInfo.battVoltages),
           sizeof(expectedVoltages));
    memcpy(expectedCurrents, &(sb->currentTelemetry.powerInfo.battCurrents),
           sizeof(expectedCurrents));
    sb->currentTelemetry.battEnabled = BATT1_ENABLED | BATT4_ENABLED;
    sb->currentTelemetry.battUsed = BATT2_INUSE | BATT3_INUSE;

    PowerSourceEventPtrList eventList;
    ram::core::EventConnectionPtr conn = sb->subscribe(
        ram::vehicle::device::SensorBoard::POWERSOURCE_UPDATE,
        boost::bind(powerSourceUpdateHelper, &eventList, _1));
    sb->update(0);

    CHECK_EQUAL(5u, eventList.size());

    
    float actualVoltages[5] = {0};
    float actualCurrents[5] = {0};
    bool actualEnables[5] = {0};
    bool actualUsed[5] = {0};
    
    for (size_t i = 0; i < LENGTH(expectedVoltages); ++i)
    {
        actualVoltages[i] = eventList[i]->voltage;
        actualCurrents[i] = eventList[i]->current;
        actualEnables[i] = eventList[i]->enabled;
        actualUsed[i] = eventList[i]->inUse;
    }

    CHECK_ARRAY_EQUAL(expectedVoltages, actualVoltages, 5);
    CHECK_ARRAY_EQUAL(expectedCurrents, actualCurrents, 5);
    CHECK_ARRAY_EQUAL(expectedEnables, actualEnables, 5);
    CHECK_ARRAY_EQUAL(expectedUsed, actualUsed, 5);
    
    conn->disconnect();
    delete sb;
}

typedef std::vector<ram::vehicle::TempSensorEventPtr>
TempSensorEventPtrList;

void tempSensorUpdateHelper(TempSensorEventPtrList* list,
                             ram::core::EventPtr event)
{
    list->push_back(boost::dynamic_pointer_cast<
                    ram::vehicle::TempSensorEvent>(event));
}

TEST_FIXTURE(SensorBoardFixture, event_TEMPSENSOR_UPDATE)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    int expectedTemps[7] = {2, 23, 4, 89, 22, 57, 79};

    // Set values to be returned
    sb->updateDone = true;
    for (size_t i = 0; i < LENGTH(expectedTemps); ++i)
        sb->currentTelemetry.temperature[i] = expectedTemps[i];

    // Register handler and trigger and update
    TempSensorEventPtrList eventList;
    ram::core::EventConnectionPtr conn = sb->subscribe(
        ram::vehicle::device::SensorBoard::TEMPSENSOR_UPDATE,
        boost::bind(tempSensorUpdateHelper, &eventList, _1));
    sb->update(0);

    // Check to make sure we got the proper number of updates
    CHECK_EQUAL(7u, eventList.size());


    // Make sure the values were correct
    int actualTemps[7] = {0};
    
    for (size_t i = 0; i < LENGTH(expectedTemps); ++i)
        actualTemps[i] = eventList[i]->temp;

    CHECK_ARRAY_EQUAL(expectedTemps, actualTemps, 7);
    
    conn->disconnect();
    delete sb;
}

typedef std::vector<ram::vehicle::ThrusterEventPtr>
ThrusterEventPtrList;

void motorCurrentUpdateHelper(ThrusterEventPtrList* list,
                             ram::core::EventPtr event)
{
    list->push_back(boost::dynamic_pointer_cast<
                    ram::vehicle::ThrusterEvent>(event));
}

TEST_FIXTURE(SensorBoardFixture, event_THRUSTER_UPDATE)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    double expectedCurrents[6] = {2.3, 5.1, 2.5, 1.0, 0.0, 9.0};
    bool expectedEnables[6] = {false, true, false, true, true, false};

    // Set values to be returned
    sb->updateDone = true;
    for (size_t i = 0; i < LENGTH(expectedCurrents); ++i)
        sb->currentTelemetry.powerInfo.motorCurrents[i] = expectedCurrents[i];
    sb->currentTelemetry.thrusterState = THRUSTER2_ENABLED |
        THRUSTER4_ENABLED | THRUSTER5_ENABLED;

    // Register handler and trigger and update
    ThrusterEventPtrList eventList;
    ram::core::EventConnectionPtr conn = sb->subscribe(
        ram::vehicle::device::SensorBoard::THRUSTER_UPDATE,
        boost::bind(motorCurrentUpdateHelper, &eventList, _1));
    sb->update(0);

    // Check to make sure we got the proper number of updates
    CHECK_EQUAL(6u, eventList.size());


    // Make sure the values were correct
    double actualCurrents[6] = {0};
    bool actualEnables[6] = {false};
    
    for (size_t i = 0; i < LENGTH(expectedCurrents); ++i)
    {
        actualCurrents[i] = eventList[i]->current;
        actualEnables[i] = eventList[i]->enabled;
    }

    CHECK_ARRAY_CLOSE(expectedCurrents, actualCurrents, 6, 0.0001);
    CHECK_ARRAY_EQUAL(expectedEnables, actualEnables, 6);
    
    conn->disconnect();
    delete sb;
}

typedef std::vector<ram::vehicle::SonarEventPtr>
SonarEventPtrList;

void sonarUpdateHelper(SonarEventPtrList* list,
                       ram::core::EventPtr event)
{
    list->push_back(boost::dynamic_pointer_cast<
                    ram::vehicle::SonarEvent>(event));
}

TEST_FIXTURE(SensorBoardFixture, event_SONAR_UPDATE)
{
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    ram::math::Vector3 expectedDirection(12.5, 0.32, 8.16);
    double expectedRange = 23;
    int expectedSec = 1237;
    int expectedUSec = 2822;
    unsigned char expectedPingerID = 3;

    // Set values to be returned
    sb->updateDone = true;

    sb->currentTelemetry.sonar.vectorX = expectedDirection.x;
    sb->currentTelemetry.sonar.vectorY = expectedDirection.y;
    sb->currentTelemetry.sonar.vectorZ = expectedDirection.z;
    sb->currentTelemetry.sonar.range = (short)expectedRange;
    sb->currentTelemetry.sonar.timeStampSec = expectedSec;
    sb->currentTelemetry.sonar.timeStampUSec = expectedUSec;
    sb->currentTelemetry.sonar.pingerID = expectedPingerID;

    // Register handler and trigger and update
    SonarEventPtrList eventList;
    ram::core::EventConnectionPtr conn = sb->subscribe(
        ram::vehicle::device::SensorBoard::SONAR_UPDATE,
        boost::bind(sonarUpdateHelper, &eventList, _1));
    sb->update(0);

    // Check to make sure we got the proper number of updates
    CHECK_EQUAL(1u, eventList.size());

    ram::vehicle::SonarEventPtr event = eventList[0];

    // Make sure the values were correct
    CHECK_EQUAL(expectedDirection, event->direction);
    CHECK_EQUAL(expectedRange, event->range);
    CHECK_EQUAL(expectedSec, event->pingTimeSec);
    CHECK_EQUAL(expectedUSec, event->pingTimeUSec);
    CHECK_EQUAL(expectedPingerID, event->pingerID);
    CHECK_EQUAL(1, event->pingCount);

    // Do another update and make sure we don't have another event
    sb->updateDone = true;
    sb->update(0);
    CHECK_EQUAL(1u, eventList.size());

    // Now change the timestamp and update again
    expectedSec = 174855;
    expectedUSec = 47856;
    sb->currentTelemetry.sonar.timeStampSec = expectedSec;
    sb->currentTelemetry.sonar.timeStampUSec = expectedUSec;

    sb->updateDone = true;
    sb->update(0);
    CHECK_EQUAL(2u, eventList.size());

    // Make sure the ping count updated
    event = eventList[1];
    CHECK_EQUAL(2, event->pingCount);
    
    conn->disconnect();
    delete sb;
}

TEST_FIXTURE(SensorBoardFixture, ThrusterLogging)
{
    BufferedAppender* appender = new BufferedAppender("Test");
    log4cpp::Category::getInstance("Thruster").setAppender(appender);

    // Create board
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    // Make sure the header is present
    CHECK_EQUAL(1u, appender->logEvents.size());
    
    // Load some values to log into memory
    double currents[6] = {2.3, 5.1, 2.5, 1.0, 0.0, 9.0};
    int speeds[6] = {238, -519, 23, 1011, -900, 758};
    
    sb->updateDone = true;
    for (size_t i = 0; i < LENGTH(currents); ++i)
        sb->currentTelemetry.powerInfo.motorCurrents[i] = currents[i];
    for (size_t i = 0; i < LENGTH(speeds); ++i)
        sb->setThrusterValue(i, speeds[i]);

    // Now do a log and check it
    sb->update(0);
    CHECK_EQUAL(2u, appender->logEvents.size());
    CHECK_EQUAL("2.3 5.1 2.5 1.0 0.0 9.0 238 -519 23 1011 -900 758",
                appender->logEvents[1].message);
}


TEST_FIXTURE(SensorBoardFixture, PowerLogging)
{
    BufferedAppender* appender = new BufferedAppender("Test");
    log4cpp::Category::getInstance("Power").setAppender(appender);

    // Create board
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    // Make sure the header is present
    CHECK_EQUAL(1u, appender->logEvents.size());
    
    // Load some values to log into memory
    double battCurrents[5] = {2.3, 5.1, 2.5, 1.0, 0.5};
    double battVoltages[5] = {20.5, 23.0, 19.9, 22.8, 2.0};
    double i5V = 1.5;
    double i12V = 2.3;
    double v5V = 5.9;
    double v12V = 11.9;
    
    sb->updateDone = true;
    for (size_t i = 0; i < LENGTH(battCurrents); ++i)
        sb->currentTelemetry.powerInfo.battCurrents[i] = battCurrents[i];
    for (size_t i = 0; i < LENGTH(battVoltages); ++i)
        sb->currentTelemetry.powerInfo.battVoltages[i] = battVoltages[i];
    sb->currentTelemetry.powerInfo.i5VBus = i5V;
    sb->currentTelemetry.powerInfo.i12VBus = i12V;
    sb->currentTelemetry.powerInfo.v5VBus = v5V;
    sb->currentTelemetry.powerInfo.v12VBus = v12V;


    // Now do a log and check it
    sb->update(0);
    CHECK_EQUAL(2u, appender->logEvents.size());
    CHECK_EQUAL("2.3 5.1 2.5 1.0 0.5 "
                "20.5 23.0 19.9 22.8 2.0 "
                "1.5 2.3 5.9 11.9",
                appender->logEvents[1].message);
}

TEST_FIXTURE(SensorBoardFixture, TempLogging)
{
    BufferedAppender* appender = new BufferedAppender("Test");
    log4cpp::Category::getInstance("Temp").setAppender(appender);

    // Create board
    TestSensorBoard* sb = new TestSensorBoard(
        ram::core::ConfigNode::fromString(BLANK_CONFIG));

    // Make sure the header is present
    CHECK_EQUAL(1u, appender->logEvents.size());
    
    // Load some values to log into memory
    char temps[7] = {23, 52, 1, 0, 24, 58, 46};
    
    sb->updateDone = true;
    for (size_t i = 0; i < LENGTH(temps); ++i)
        sb->currentTelemetry.temperature[i] = temps[i];

    // Now do a log and check it
    sb->update(0);
    CHECK_EQUAL(2u, appender->logEvents.size());
    CHECK_EQUAL("23 52 1 0 24 58 46",
                appender->logEvents[1].message);
}
