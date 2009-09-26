/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/MockSensorBoard.h
 */

#ifndef RAM_VEHICLE_DEVICE_MOCKSENSORBOARD_06_12_2008
#define RAM_VEHICLE_DEVICE_MOCKSENSORBOARD_06_12_2008

// STD Includes
#include <string>
#include <cstdlib>


// Project Includes
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/Events.h"

class MockSensorBoard : public ram::vehicle::device::SensorBoard
{
public:
    MockSensorBoard(
        ram::core::ConfigNode config,
        ram::core::EventHubPtr eventHub = ram::core::EventHubPtr()) :
        SensorBoard(-1, config, eventHub)
    {
        // Set array to false
        memset(thrusterEnables, false, sizeof(bool) * 6);
        memset(thrusterValues, 0, sizeof(int) * 6);
        memset(powerSourceEnables, false, sizeof(bool) * 6);
        memset(powerSourceUsed, false, sizeof(bool) * 6);

        // Init other values
        mainBusVoltage = 0.0;
        markerDropNum = -1;
        torpedoFireNum = -1;
    }    
    virtual ~MockSensorBoard() {}

    virtual double getDepth() { return 0; }

    virtual void setThrusterValue(int address, int count) {
        assert(address <= 5 && address >=0 && "Invalid Address");
        thrusterValues[address] = count;
    }

    virtual bool isThrusterEnabled(int address) {
        assert(address <= 5 && address >=0 && "Invalid Address");
        return thrusterEnables[address];
    }

    virtual void setThrusterEnable(int address, bool state) {
        assert(address <= 5 && address >=0 && "Invalid Address");
        thrusterEnables[address] = state;
    }

    virtual bool isPowerSourceEnabled(int address) {
        assert(address <= 5 && address >=0 && "Invalid Address");
        return powerSourceEnables[address];
    }

    virtual bool isPowerSourceInUse(int address) {
        assert(address <= 5 && address >=0 && "Invalid Address");
        return powerSourceUsed[address];
    }

    virtual void setPowerSouceEnabled(int address, bool state) {
        assert(address <= 5 && address >=0 && "Invalid Address");
        powerSourceEnables[address] = state;
    }

    virtual double getMainBusVoltage() { return mainBusVoltage; }
    
    virtual int dropMarker() { return markerDropNum; }

    virtual int fireTorpedo() { return torpedoFireNum; }

    bool thrusterEnables[6];
    int thrusterValues[6];

    bool powerSourceEnables[6];
    bool powerSourceUsed[6];

    int markerDropNum;
    int torpedoFireNum;

    double mainBusVoltage;
    
    void publishPowerSourceUpdate(int id, bool enabled, bool inUse,
                                  double voltage, double current)
    {
        ram::vehicle::PowerSourceEventPtr event(
            new ram::vehicle::PowerSourceEvent);
        
        event->id = id;
        event->enabled = enabled;
        event->inUse = inUse;
        event->voltage = voltage;
        event->current = current;

        publish(ram::vehicle::device::SensorBoard::POWERSOURCE_UPDATE, event);
    }

    void publishTempSensorUpdate(int id, int temp)
    {
        ram::vehicle::TempSensorEventPtr event(
            new ram::vehicle::TempSensorEvent);
        
        event->id = id;
        event->temp = temp;

        publish(ram::vehicle::device::SensorBoard::TEMPSENSOR_UPDATE, event);
    }

    void publishThrusterUpdate(int address, double current, bool enabled)
    {
        ram::vehicle::ThrusterEventPtr event(
            new ram::vehicle::ThrusterEvent);
        
        event->address = address;
        event->current = current;
        event->enabled = enabled;
        
        publish(ram::vehicle::device::SensorBoard::THRUSTER_UPDATE, event);
    }

    void publishSonarUpdate(ram::math::Vector3 direction, int range,
                            int sec, int usec)
    {
        ram::vehicle::SonarEventPtr event(
            new ram::vehicle::SonarEvent);

        event->direction = direction;
        event->range = range;
        event->pingTimeSec = sec;
        event->pingTimeUSec = usec;
        
        publish(ram::vehicle::device::SensorBoard::SONAR_UPDATE, event);
    }
};

#endif // RAM_VEHICLE_DEVICE_MOCKSENSORBOARD_06_12_2008
