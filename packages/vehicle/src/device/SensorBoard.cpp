/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SensorBoard.cpp
 */

// Project Includes
#include "vehicle/include/device/SensorBoard.h"

#include "math/include/Events.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::SensorBoard, DEPTH_UPDATE);

namespace ram {
namespace vehicle {
namespace device {

SensorBoard::SensorBoard(int deviceFD,
                         core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Device(config["name"].asString()),
    m_depthCalibSlope(config["depthCalibSlope"].asDouble()),
    m_depthCalibIntercept(config["depthCalibIntercept"].asDouble()),
    m_calibratedDepth(false),
    m_depthOffset(0),
    m_deviceFile(""),
    m_deviceFD(deviceFD)
{
    // If we get a negative FD, don't try to talk to the board
    if (deviceFD >= 0)
        establishConnection();
}
    

SensorBoard::SensorBoard(core::ConfigNode config,
                         core::EventHubPtr eventHub,
                         IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    m_depthCalibSlope(config["depthCalibSlope"].asDouble()),
    m_depthCalibIntercept(config["depthCalibIntercept"].asDouble()),
    m_calibratedDepth(false),
    m_depthOffset(0),
    m_deviceFile(config["deviceFile"].asString("/dev/sensor")),
    m_deviceFD(-1)
{
    establishConnection();
}
    
SensorBoard::~SensorBoard()
{
    boost::mutex::scoped_lock lock(m_deviceMutex);
    if (m_deviceFD >= 0)
    {
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
        int ret = partialRead(&state.telemetry);
        // TODO: trigger proper events when a full update happens
    
        // Now read depth
        ret = readDepth();
        double depth = (((double)ret) - m_depthCalibIntercept) /
            m_depthCalibSlope - m_depthOffset; 
                
        if (m_calibratedDepth)
        {
            // Only record depth after we are calibrated
            state.depth = depth;
            
            // Publish event
            math::NumericEventPtr devent(new math::NumericEvent());
            devent->number = depth;
            publish(SensorBoard::DEPTH_UPDATE, devent);
        }
        else
        {
            // If we aren't calibrated, take values
            m_depthFilter.addValue(depth);
            
            // After five values, take the reading
            if (5 == m_depthFilter.getSize())
            {
                m_calibratedDepth = true;
                m_depthOffset = m_depthFilter.getValue();
            }
        }
    } // end deviceMutex lock
    
    // Copy the values back
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_state = state;
    }
}

double SensorBoard::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_state.depth;
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

void SensorBoard::dropMarker()
{
    static int markerNum = 0;
    boost::mutex::scoped_lock lock(m_deviceMutex);
    
    if (markerNum <= 1)
    {
        dropMarker(markerNum);
        markerNum++;
    }
    else
    {
        // Report an error
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
}

bool SensorBoard::handleReturn(int ret)
{
    if (ret < 0)
    {
        close(m_deviceFD);
        m_deviceFD = -1;
        establishConnection();
    }

    return true;
}

void SensorBoard::setSpeeds(int s1, int s2, int s3, int s4, int s5, int s6)
{
    handleReturn(::setSpeeds(m_deviceFD, s1, s2, s3, s4, s5, s6));
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

void SensorBoard::dropMarker(int markerNum)
{
    handleReturn(::dropMarker(m_deviceFD, markerNum));
}

void SensorBoard::syncBoard()
{
    if (SB_ERROR == ::syncBoard(m_deviceFD))
    {
        assert(false && "Can't sync with the sensor board");
    }
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
