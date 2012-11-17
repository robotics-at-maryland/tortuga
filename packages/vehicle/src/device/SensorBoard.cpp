/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SensorBoard.cpp
 */
//#include <iostream>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/Events.h"
#include "vehicle/include/Common.h"

#include "math/include/Events.h"


RAM_CORE_EVENT_TYPE(ram::vehicle::device::SensorBoard, POWERSOURCE_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::SensorBoard, TEMPSENSOR_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::SensorBoard, THRUSTER_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::SensorBoard, SONAR_UPDATE);

// Current vehicle 
int ram::vehicle::device::SensorBoard::NUMBER_OF_MARKERS = 2;
int ram::vehicle::device::SensorBoard::NUMBER_OF_TORPEDOS = 2;

//Make sure the size of these arrays matches the number of markers/torpedos.
bool markersDropped[2] = {0};
bool torpedosFired[2] = {0};

static log4cpp::Category& s_thrusterLog
(log4cpp::Category::getInstance("Thruster"));
static log4cpp::Category& s_powerLog
(log4cpp::Category::getInstance("Power"));
static log4cpp::Category& s_tempLog
(log4cpp::Category::getInstance("Temp"));

static void setupLogging() {
    s_thrusterLog.info("%% MC1 MC2 MC3 MC4 MC5 MC6"
                       " TV1 TV2 TV3 TV4 TV5 TV6 TimeStamp");
    s_powerLog.info("%% iBatt1 iBatt2 iBatt3 iBatt4 iShore "
                    "vBatt1 vBatt2 vBatt3 vBatt4 vShore "
                    "i5V_Bus i12V_Bus v5V_Bus v12V_Bus TimeStamp");
    s_tempLog.info("%% \"Sensor Board\" Unused Unused Unused Unused "
                   "\"Distro Board\" \"Balancer Board\"");
}

namespace ram {
namespace vehicle {
namespace device {

SensorBoard::SensorBoard(int deviceFD,
                         core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Device(config["name"].asString()),
    m_depthCalibSlope(config["depthCalibSlope"].asDouble()),
    m_depthCalibIntercept(config["depthCalibIntercept"].asDouble()),
    m_deviceFile(""),
    m_deviceFD(deviceFD)
{
    // Initialize values
    m_location = math::Vector3(config["depthSensorLocation"][0].asDouble(0), 
                               config["depthSensorLocation"][1].asDouble(0),
                               config["depthSensorLocation"][2].asDouble(0));

    m_state.thrusterValues[0] = 0;
    m_state.thrusterValues[1] = 0;
    m_state.thrusterValues[2] = 0;
    m_state.thrusterValues[3] = 0;
    m_state.thrusterValues[4] = 0;
    m_state.thrusterValues[5] = 0;

    m_servo1FirePosition = config["servo1FirePosition"].asInt(4000);
    m_servo2FirePosition = config["servo2FirePosition"].asInt(4000);
    m_servo3FirePosition = config["servo3FirePosition"].asInt(4000);
    m_servo4FirePosition = config["servo4FirePosition"].asInt(4000);
    
    // If we get a negative FD, don't try to talk to the board
    if (deviceFD >= 0)
        establishConnection();

    // Log file header
    setupLogging();
}
    

SensorBoard::SensorBoard(core::ConfigNode config,
                         core::EventHubPtr eventHub,
                         IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IDepthSensor(eventHub, config["name"].asString()),
    m_depthCalibSlope(config["depthCalibSlope"].asDouble()),
    m_depthCalibIntercept(config["depthCalibIntercept"].asDouble()),
    m_deviceFile(config["deviceFile"].asString("/dev/sensor")),
    m_deviceFD(-1)
{

    // Initialize values
    m_location = math::Vector3(config["depthSensorLocation"][0].asDouble(0), 
                               config["depthSensorLocation"][1].asDouble(0),
                               config["depthSensorLocation"][2].asDouble(0));

    m_state.thrusterValues[0] = 0;
    m_state.thrusterValues[1] = 0;
    m_state.thrusterValues[2] = 0;
    m_state.thrusterValues[3] = 0;
    m_state.thrusterValues[4] = 0;
    m_state.thrusterValues[5] = 0;

    m_servo1FirePosition = config["servo1FirePosition"].asInt(4000);
    m_servo2FirePosition = config["servo2FirePosition"].asInt(4000);
    m_servo3FirePosition = config["servo3FirePosition"].asInt(4000);

    // Connect to the sensor board
    establishConnection();
  
    for (int i = 0; i < 11; ++i)
        update(1.0/40);

    // Log file header
    setupLogging();
}
    
SensorBoard::~SensorBoard()
{
    Updatable::unbackground(true);

    boost::mutex::scoped_lock lock(m_deviceMutex);
    if (m_deviceFD >= 0)
    {
        // setServoPower(SERVO_POWER_OFF);
        close(m_deviceFD);
        m_deviceFD = -1;
    }
}

void SensorBoard::update(double timestep)
{
    // Copy the values to local state
    VehicleState state;
    {
        core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
        state = m_state;
    }

    int partialRet = SB_ERROR;
    double depth = 0;
    {
        boost::mutex::scoped_lock lock(m_deviceMutex);
    
        // Send commands
        setSpeeds(state.thrusterValues[0],
                  state.thrusterValues[1],
                  state.thrusterValues[2],
                  state.thrusterValues[3],
                  state.thrusterValues[4],
                  state.thrusterValues[5]);
    
        // Do a partial read
        partialRet = partialRead(&state.telemetry);
    
        // Now read depth and set its state
        int ret = readDepth();
        depth = (((double)ret) - m_depthCalibIntercept) / m_depthCalibSlope;
        state.depth = depth;
    } // end deviceMutex lock

    // Publish depth event
    depthEvent(depth);

    /* publish the new depth sensor reading */
    vehicle::RawDepthSensorDataEventPtr rawEvent(
        new vehicle::RawDepthSensorDataEvent());
    rawEvent->name = getName();
    rawEvent->rawDepth = depth;
    rawEvent->sensorLocation = m_location;
    rawEvent->timestep = timestep;
    publish(IDepthSensor::RAW_UPDATE, rawEvent);

    // If we got the battery use status or the latest voltages recompute bus
    // Voltage
    if ((state.telemetry.updateState == BATTERY_VOLTAGES) ||
        (state.telemetry.updateState == BATTERY_USED))
    {
        state.mainBusVoltage = calculateMainBusVoltage(&state.telemetry);
    }

    // If we have done a full update of telem, trigger events and log
    if (partialRet == SB_UPDATEDONE)
    {
        powerSourceEvents(&state.telemetry);
        tempSensorEvents(&state.telemetry);
        thrusterEvents(&state.telemetry);
        sonarEvent(&state.telemetry);
        
        // Thruster logging data
        s_thrusterLog.info("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f" // Current
                           " %d %d %d %d %d %d",                 // Command
                           state.telemetry.powerInfo.motorCurrents[0],
                           state.telemetry.powerInfo.motorCurrents[1],
                           state.telemetry.powerInfo.motorCurrents[2],
                           state.telemetry.powerInfo.motorCurrents[3],
                           state.telemetry.powerInfo.motorCurrents[4],
                           state.telemetry.powerInfo.motorCurrents[5],
                           state.thrusterValues[0],
                           state.thrusterValues[1],
                           state.thrusterValues[2],
                           state.thrusterValues[3],
                           state.thrusterValues[4],
                           state.thrusterValues[5]);
        
        // Power Logging Data
        s_powerLog.info("%3.1f %3.1f %3.1f %3.1f %3.1f " // Batt Current
                        "%3.1f %3.1f %3.1f %3.1f %3.1f " // Batt Voltage
                        "%3.1f %3.1f %3.1f %3.1f",       // Bus I, Bus V
                        state.telemetry.powerInfo.battCurrents[0],
                        state.telemetry.powerInfo.battCurrents[1],
                        state.telemetry.powerInfo.battCurrents[2],
                        state.telemetry.powerInfo.battCurrents[3],
                        state.telemetry.powerInfo.battCurrents[4],
                        state.telemetry.powerInfo.battVoltages[0],
                        state.telemetry.powerInfo.battVoltages[1],
                        state.telemetry.powerInfo.battVoltages[2],
                        state.telemetry.powerInfo.battVoltages[3],
                        state.telemetry.powerInfo.battVoltages[4],
                        state.telemetry.powerInfo.i5VBus,
                        state.telemetry.powerInfo.i12VBus,
                        state.telemetry.powerInfo.v5VBus,
                        state.telemetry.powerInfo.v12VBus);
        
        // Temp Logging data
        s_tempLog.info("%d %d %d %d %d %d %d",
                       (int)state.telemetry.temperature[0],
                       (int)state.telemetry.temperature[1],
                       (int)state.telemetry.temperature[2],
                       (int)state.telemetry.temperature[3],
                       (int)state.telemetry.temperature[4],
                       (int)state.telemetry.temperature[5],
                       (int)state.telemetry.temperature[6]);
    } // end partialRet == SB_UPDATEDONE
    
    // Copy the values back
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_state = state;
    }
}

math::Vector3 SensorBoard::getLocation()
{
    return m_location;
}


void SensorBoard::setThrusterValue(int address, int count)
{
    assert((0 <= address) && (address < 6) && "Address out of range");
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_state.thrusterValues[address] = count;
}

bool SensorBoard::isThrusterEnabled(int address)
{
    static int addressToEnable[] = {
        THRUSTER1_ENABLED,
        THRUSTER2_ENABLED,
        THRUSTER3_ENABLED,
        THRUSTER4_ENABLED,
        THRUSTER5_ENABLED,
        THRUSTER6_ENABLED
    };

    assert((0 <= address) && (address < 6) && "Address out of range");

    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return (0 != (addressToEnable[address] & m_state.telemetry.thrusterState));
}

void SensorBoard::setThrusterEnable(int address, bool state)
{
    static int addressToOn[] = {
        CMD_THRUSTER1_ON,
        CMD_THRUSTER2_ON,
        CMD_THRUSTER3_ON,
        CMD_THRUSTER4_ON,
        CMD_THRUSTER5_ON,
        CMD_THRUSTER6_ON
    };
    
    static int addressToOff[] = {
        CMD_THRUSTER1_OFF,
        CMD_THRUSTER2_OFF,
        CMD_THRUSTER3_OFF,
        CMD_THRUSTER4_OFF,
        CMD_THRUSTER5_OFF,
        CMD_THRUSTER6_OFF
    };

    assert((0 <= address) && (address < 6) && "Address out of range");

    {
        boost::mutex::scoped_lock lock(m_deviceMutex);

        int val;
        
        if (state)
            val = addressToOn[address];
        else
            val = addressToOff[address];
        
        setThrusterSafety(val);
    }
    
    // Now set our internal flag to make everything consistent (maybe)
}


bool SensorBoard::isPowerSourceEnabled(int address)
{
    static int addressToEnable[] = {
        BATT1_ENABLED,
        BATT2_ENABLED,
        BATT3_ENABLED,
        BATT4_ENABLED,
        BATT5_ENABLED,
        BATT6_ENABLED,
    };

    assert((0 <= address) && (address < 6) && "Address out of range");

    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return (0 != (addressToEnable[address] & m_state.telemetry.battEnabled));
}

bool SensorBoard::isPowerSourceInUse(int address)
{
    static int addressToEnable[] = {
        BATT1_INUSE,
        BATT2_INUSE,
        BATT3_INUSE,
        BATT4_INUSE,
        BATT5_INUSE,
        BATT6_INUSE,
    };

    assert((0 <= address) && (address < 6) && "Address out of range");

    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return (0 != (addressToEnable[address] & m_state.telemetry.battUsed));
}

void SensorBoard::setPowerSouceEnabled(int address, bool state)
{
    static int addressToOn[] = {
        CMD_BATT1_ON,
        CMD_BATT2_ON,
        CMD_BATT3_ON,
        CMD_BATT4_ON,
        CMD_BATT5_ON,
        CMD_BATT6_ON,
    };
    
    static int addressToOff[] = {
        CMD_BATT1_OFF,
        CMD_BATT2_OFF,
        CMD_BATT3_OFF,
        CMD_BATT4_OFF,
        CMD_BATT5_OFF,
        CMD_BATT6_OFF,
    };

    assert((0 <= address) && (address < 6) && "Power source id out of range");

    {
        boost::mutex::scoped_lock lock(m_deviceMutex);

        int val;
        
        if (state)
            val = addressToOn[address];
        else
            val = addressToOff[address];
        
        setBatteryState(val);
    }
}

void SensorBoard::setDVLPowerEnabled(bool state)
{
    // 1 is on, 0 is off
    if (state)
        setDVLPower(1);
    else
        setDVLPower(0);
}

double SensorBoard::getMainBusVoltage()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_state.mainBusVoltage;
}

int SensorBoard::dropMarker()
{
    boost::mutex::scoped_lock lock(m_deviceMutex);
    
    int markerNum;
    for (markerNum = 0; markerNum < NUMBER_OF_MARKERS; markerNum++)
    {
        if(markersDropped[markerNum] == false){
            dropMarker(markerNum + 1);
            markersDropped[markerNum] = true;
            return (markerNum + 1);
        }
    }

    return -1;
}

int SensorBoard::dropMarkerIndex(int index)
{
    if (index <= NUMBER_OF_MARKERS)
    {
        dropMarker(index);
        markersDropped[index - 1] = true;
        return index;
    }

    return -1;
}

int SensorBoard::fireTorpedo()
{
#ifdef NO_SERVOS
    return -1;
#else // NO_SERVOS
    boost::mutex::scoped_lock lock(m_deviceMutex);

    int torpedoNum = 0;
    for (torpedoNum = 0; torpedoNum < NUMBER_OF_TORPEDOS; torpedoNum++)
    {
        if(torpedosFired[torpedoNum] == false){
            handleReturn(::fireTorpedo(m_deviceFD, torpedoNum));
            torpedosFired[torpedoNum] = true;
            return torpedoNum;
        }
    }

    return -1;
#endif // NO_SERVOS
}

int SensorBoard::fireTorpedo(int index)
{
    boost::mutex::scoped_lock lock(m_deviceMutex);
    
    if (index <= NUMBER_OF_TORPEDOS)
    {
        handleReturn(::fireTorpedo(m_deviceFD, index));
        torpedosFired[index] = true;
        return index;
    }
    return -1;
}
int SensorBoard::extendGrabber()
{
    // Closes Grabber
    boost::mutex::scoped_lock lock(m_deviceMutex);
    handleReturn(::extendGrabber(m_deviceFD));
    return 1;

}
int SensorBoard::retractGrabber()
{
    // Opens Grabber
    boost::mutex::scoped_lock lock(m_deviceMutex);
    handleReturn(::retractGrabber(m_deviceFD));
    return 1;
}
    
void SensorBoard::setSpeeds(int s1, int s2, int s3, int s4, int s5, int s6)
{
    handleReturn(::setSpeeds(m_deviceFD, s1, s2, s3, s4, s5, s6));
}

void SensorBoard::setExtraThrusterSpeed(int speed)
{
    boost::mutex::scoped_lock lock(m_deviceMutex);
    if(speed){
        // NOTE: I don't like this name, but there's no time for me
        // to bother changing it
        handleReturn(::setDerpyPower(m_deviceFD, 1)); //turn on
        handleReturn(::setDerpySpeed(m_deviceFD, speed)); //set speed
    }
    else{
        handleReturn(::setDerpyPower(m_deviceFD, 0)); //turn off
        handleReturn(::setDerpySpeed(m_deviceFD, 0)); //set speed to 0
    }
}
    
int SensorBoard::partialRead(struct boardInfo* telemetry)
{
    int ret = ::partialRead(m_deviceFD, telemetry);
    if (handleReturn(ret))
        return ret;
    else
        assert(false && "Can't talk to sensor board");
    return 0;
}

int SensorBoard::readDepth()
{
    int ret = ::readDepth(m_deviceFD);
    if (handleReturn(ret))
        return ret;
    else
        assert(false && "Can't talk to sensor board");
    return 0;
}

void SensorBoard::setThrusterSafety(int state)
{
    handleReturn(::setThrusterSafety(m_deviceFD, state));
}

void SensorBoard::setBatteryState(int state)
{
    handleReturn(::setBatteryState(m_deviceFD, state));
}

void SensorBoard::dropMarker(int markerNum)
{
    std::cout << "Dropping Marker " << markerNum << std::endl;
    int ret = ::dropMarker(m_deviceFD, markerNum);
    std::cout << "Marker Return: " << ret << std::endl;
    //handleReturn(ret);
}

void SensorBoard::setServoPosition(unsigned char servoNumber,
                                   unsigned short position)
{
    handleReturn(::setServoPosition(m_deviceFD, servoNumber, position));    
}  

void SensorBoard::setServoEnable(unsigned char mask)
{
    handleReturn(::setServoEnable(m_deviceFD, mask));    
}  

void SensorBoard::setServoPower(unsigned char power)
{
    handleReturn(::setServoPower(m_deviceFD, power));    
}

void SensorBoard::setDVLPower(unsigned char power)
{
    handleReturn(::DVLOn(m_deviceFD, power));
}
    
void SensorBoard::syncBoard()
{
    if (SB_ERROR == ::syncBoard(m_deviceFD))
    {
        assert(false && "Can't sync with the sensor board");
    }
}

void SensorBoard::establishConnection()
{
    boost::mutex::scoped_lock lock(m_deviceMutex);
    if (m_deviceFD < 0)
    {
        m_deviceFD = openSensorBoard(m_deviceFile.c_str());

        if (m_deviceFD < 0)
        {
            assert(false && "Can't open sensor board file");
        }
    }

    syncBoard();

    // Turn on the servos
    // setServoPower(SERVO_POWER_ON);
}

bool SensorBoard::handleReturn(int ret)
{
    if (ret < 0)
    {
        std::cout << "some kind of error.  reestablishing connection." << std::endl;

//         int j, nptrs;
// #define SIZE 100
//         void *buffer[100];
//         char **strings;

//         nptrs = backtrace(buffer, SIZE);
//         printf("backtrace() returned %d addresses\n", nptrs);

//         /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
//            would produce similar output to the following: */

//         strings = backtrace_symbols(buffer, nptrs);
//         if (strings == NULL) {
//             perror("backtrace_symbols");
//             exit(EXIT_FAILURE);
//         }

//         for (j = 0; j < nptrs; j++)
//             printf("%s\n", strings[j]);

//         free(strings);
// #undef SIZE

        close(m_deviceFD);
        m_deviceFD = -1;
        establishConnection();
    }

    return true;
}

void SensorBoard::depthEvent(double depth)
{
    /* should be removed when state estimator transition is complete */
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depth;
    publish(IDepthSensor::UPDATE, event);


}
    
void SensorBoard::powerSourceEvents(struct boardInfo* telemetry)
{
    static int id2Enable[5] = {
        BATT1_ENABLED,
        BATT2_ENABLED,
        BATT3_ENABLED,
        BATT4_ENABLED,
        BATT5_ENABLED
    };

    static int id2InUse[5] = {
        BATT1_INUSE,
        BATT2_INUSE,
        BATT3_INUSE,
        BATT4_INUSE,
        BATT5_INUSE
    };
    
    for (int i = BATTERY_1; i <= SHORE; ++i)
    {
        PowerSourceEventPtr event(new PowerSourceEvent);
        event->id = i;
        event->enabled = telemetry->battEnabled & id2Enable[i];
        event->inUse = telemetry->battUsed & id2InUse[i];
        event->voltage = telemetry->powerInfo.battVoltages[i];
        event->current = telemetry->powerInfo.battCurrents[i];
        publish(POWERSOURCE_UPDATE, event);
    }
}

void SensorBoard::tempSensorEvents(struct boardInfo* telemetry)
{
    for (int i = 0; i < NUM_TEMP_SENSORS; ++i)
    {
        TempSensorEventPtr event(new TempSensorEvent);
        event->id = i;
        event->temp = telemetry->temperature[i];
        publish(TEMPSENSOR_UPDATE, event);
    }
}
    
void SensorBoard::thrusterEvents(struct boardInfo* telemetry)
{
    static int addressToEnable[] = {
        THRUSTER1_ENABLED,
        THRUSTER2_ENABLED,
        THRUSTER3_ENABLED,
        THRUSTER4_ENABLED,
        THRUSTER5_ENABLED,
        THRUSTER6_ENABLED
    };
    
    for (int i = 0; i < 6; ++i)
    {
        ThrusterEventPtr event(new ThrusterEvent);
        event->address = i;
        event->current = telemetry->powerInfo.motorCurrents[i];
        event->enabled = telemetry->thrusterState & addressToEnable[i];
        publish(THRUSTER_UPDATE, event);
    }
}

void SensorBoard::sonarEvent(struct boardInfo* telemetry)
{
    static unsigned int previousSec = 0;
    static unsigned int previousUsec = 0;
    static unsigned int pingCount = 0;

    // Only publish an event if the time stamp is chanced
    if ((previousSec != telemetry->sonar.timeStampSec) ||
        (previousUsec != telemetry->sonar.timeStampUSec))
    {

        // Increment the amount of pings we have heard
        pingCount++;

        // Create and pack our new event
        SonarEventPtr event(new SonarEvent);
        event->direction = math::Vector3(telemetry->sonar.vectorX,
                                         telemetry->sonar.vectorY,
                                         telemetry->sonar.vectorZ);
        event->range = telemetry->sonar.range;
        event->pingTimeSec = telemetry->sonar.timeStampSec;
        event->pingTimeUSec = telemetry->sonar.timeStampUSec;
        event->pingerID = telemetry->sonar.pingerID;
        event->pingCount = pingCount;
        
        publish(SONAR_UPDATE, event);

        // Record the previous timestamp
        previousSec = telemetry->sonar.timeStampSec;
        previousUsec = telemetry->sonar.timeStampUSec;
    }
}

double SensorBoard::calculateMainBusVoltage(struct boardInfo* telemetry)
{
    static int addressToEnable[] = {
        BATT1_INUSE,
        BATT2_INUSE,
        BATT3_INUSE,
        BATT4_INUSE,
        BATT5_INUSE,
    };

    int battCount = sizeof(addressToEnable)/sizeof(int);
    double totalVoltage = 0;
    int inUseCount = 0;

    for (int i = 0; i < battCount; ++i)
    {
        if (addressToEnable[i] & telemetry->battUsed)
        {
            totalVoltage += telemetry->powerInfo.battVoltages[i];
            inUseCount++;
        }
    }

    return totalVoltage / inUseCount;
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
