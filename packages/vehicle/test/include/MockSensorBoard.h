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
        assert(address <= 4 && address >=0 && "Invalid Address");
        return powerSourceEnables[address];
    }

    virtual bool isPowerSourceInUse(int address) {
        assert(address <= 4 && address >=0 && "Invalid Address");
        return powerSourceUsed[address];
    }

    virtual void setPowerSouceEnabled(int address, bool state) {
        assert(address <= 4 && address >=0 && "Invalid Address");
        powerSourceEnables[address] = state;
    }
    
    virtual void dropMarker() {}

    bool thrusterEnables[6];
    int thrusterValues[6];

    bool powerSourceEnables[6];
    bool powerSourceUsed[6];
    
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

    void publishMotorCurrentUpdate(int address, double current)
    {
        ram::vehicle::MotorCurrentEventPtr event(
            new ram::vehicle::MotorCurrentEvent);
        
        event->address = address;
        event->current = current;

        publish(ram::vehicle::device::SensorBoard::MOTORCURRENT_UPDATE, event);
    }
};

#endif // RAM_VEHICLE_DEVICE_MOCKSENSORBOARD_06_12_2008
