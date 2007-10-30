/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Vehicle.cpp
 */

// STD Includes
#include <iostream>
#ifdef RAM_POSIX
    #include <unistd.h>
#endif

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/include/device/IThruster.h"
#include "vehicle/include/device/IIMU.h"
#include "sensorapi/include/sensorapi.h"

using namespace ram::vehicle::device;

namespace ram {
namespace vehicle {

Vehicle::Vehicle(core::ConfigNode config) :
    m_config(config),
    m_sensorFD(-1),
    m_markerNum(0),
    m_depthCalibSlope(m_config["depthCalibSlope"].asDouble()),
    m_depthCalibIntercept(m_config["depthCalibIntercept"].asDouble()),
    m_calibratedDepth(false),
    m_depthOffset(0),
    m_rStarboard(0),
    m_rPort(0),
    m_rFore(0),
    m_rAft(0),
    m_imu(device::IIMUPtr())
{
    std::string devfile =
        m_config["sensor_board_file"].asString("/dev/sensor");
    
    m_sensorFD = openSensorBoard(devfile.c_str());

    if (m_sensorFD < 0)
    {
        std::cout << "Could not open sensor board\n";
    }
    else
    {    
        syncBoard(m_sensorFD);
        /// @TODO Check to see if we are already unsafed, and if we are don't 
        // try to unsafe again.  If we aren't unsafe, and sleep.
        unsafeThrusters();
    }
    
    // Get the thruster names
    m_starboardThruster =
        config["StarboardThrusterName"].asString("StarboardThruster");
    m_portThruster =
        config["PortThrusterName"].asString("PortThruster");
    m_foreThruster =
        config["ForeThrusterName"].asString("ForeThruster");
    m_aftThruster =
        config["AftThrusterName"].asString("AftThruster");

    m_imuName = config["IMUName"].asString("IMU");
    
    // Grab thruster combining constants
    m_rStarboard = config["rStarboard"].asDouble(0.1905);
    m_rPort = config["rPort"].asDouble(0.1905);
    m_rFore= config["rFore"].asDouble(0.3366);
    m_rAft = config["rAft"].asDouble(0.3366);
    
    // Allocate space for temperate readings
    m_state.temperatures.reserve(NUM_TEMP_SENSORS);
    
    // Create devices
    if (config.exists("Devices"))
    {
	    core::NodeNameList subnodes = config["Devices"].subNodes();
	    BOOST_FOREACH(std::string nodeName, subnodes)
	    {
	    	core::ConfigNode node(config["Devices"][nodeName]);
	    	node.set("name", nodeName);
	    	std::cout << "Creating device " << node["name"].asString()
	    		<< " of type: " << node["type"].asString() << std::endl;
	    	device::IDevicePtr device(device::IDeviceMaker::newObject(node));
	    	_addDevice(device);
	    }
    }
}

Vehicle::~Vehicle()
{	
	// For safeties sake send a zero torque and force command which will kill
	// any current thruster power
	applyForcesAndTorques(math::Vector3::ZERO, math::Vector3::ZERO);
	
    // Remove all references to the devices, will cause them to be destructed
    // this will cause the Thruster objects to be deleted and set the 
    // thrusters to nuetral.  The lone problem here is that these objects are
    // reference counted, so we can't be sure that there are not any 
    // references  floating around keeping the object from being destoryed.
    m_devices.clear();

    //safeThrusters();
    
	// Stop all background threads (does not include device background threads)
	unbackground(true);
    
    if (m_sensorFD >= 0)
        close(m_sensorFD);
    
}
    
device::IDevicePtr Vehicle::getDevice(std::string name)
{
    NameDeviceMapIter iter = m_devices.find(name);
    assert(iter != m_devices.end() && "Error Device not found");
    return (*iter).second;
}

double Vehicle::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.depth;
}

std::vector<std::string> Vehicle::getTemperatureNames()
{
    std::vector<std::string> names;

    // No current way to get actual sensor names
    for (int i = 0; i < NUM_TEMP_SENSORS; ++i)
        names.push_back("Unknown");
    
    return names;
}

std::vector<int> Vehicle::getTemperatures()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.temperatures;
}

math::Vector3 Vehicle::getLinearAcceleration()
{
    return getIMU()->getLinearAcceleration();
}
    
math::Vector3 Vehicle::getAngularRate()
{
    return getIMU()->getAngularRate();
}
    
math::Quaternion Vehicle::getOrientation()
{
    return getIMU()->getOrientation();
}
    
void Vehicle::safeThrusters()
{
    boost::mutex::scoped_lock lock(m_sensorBoardMutex);
    
    if (m_sensorFD >= 0)
    {
        // Todo, check whether these succeed
        thrusterSafety(m_sensorFD, CMD_THRUSTER1_OFF);
        thrusterSafety(m_sensorFD, CMD_THRUSTER2_OFF);
        thrusterSafety(m_sensorFD, CMD_THRUSTER3_OFF);
        thrusterSafety(m_sensorFD, CMD_THRUSTER4_OFF);
    }
}

void Vehicle::unsafeThrusters()
{
    boost::mutex::scoped_lock lock(m_sensorBoardMutex);
    
    if (m_sensorFD >= 0)
    {
        // todo check whether these succeed
        thrusterSafety(m_sensorFD, CMD_THRUSTER1_ON);
        thrusterSafety(m_sensorFD, CMD_THRUSTER2_ON);
        thrusterSafety(m_sensorFD, CMD_THRUSTER3_ON);
        thrusterSafety(m_sensorFD, CMD_THRUSTER4_ON);
    }
}

void Vehicle::dropMarker()
{
    boost::mutex::scoped_lock lock(m_sensorBoardMutex);
    ::dropMarker(m_sensorFD, m_markerNum);
}

int Vehicle::startStatus()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    return m_state.startSwitch;   
}

void Vehicle::printLine(int line, std::string text)
{
    if (m_sensorFD >= 0)
    {
        boost::mutex::scoped_lock lock(m_sensorBoardMutex);
        displayText(m_sensorFD, line, text.c_str());
    }
}


void Vehicle::applyForcesAndTorques(const math::Vector3& translationalForces,
                                    const math::Vector3& rotationalTorques)
{
    double star = translationalForces[0] / 2 +
        0.5 * rotationalTorques[2] / m_rStarboard;
    double port = translationalForces[0] / 2 -
        0.5 * rotationalTorques[2] / m_rPort;
    double fore = translationalForces[2] / 2 +
        0.5 * rotationalTorques[1] / m_rFore;
    double aft = translationalForces[2]/2 -
      0.5 * rotationalTorques[1] / m_rAft;

    if (m_devices.end() != m_devices.find(m_starboardThruster))
    {
	    device::IDevice::castTo<device::IThruster>(
	        getDevice(m_starboardThruster))->setForce(star);
	    device::IDevice::castTo<device::IThruster>(
	        getDevice(m_portThruster))->setForce(port);
	    device::IDevice::castTo<device::IThruster>(
	        getDevice(m_foreThruster))->setForce(fore);
	    device::IDevice::castTo<device::IThruster>(
	        getDevice(m_aftThruster))->setForce(aft);
    }
}
    
void Vehicle::getState(Vehicle::VehicleState& state)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    state = m_state;
}

void Vehicle::setState(const Vehicle::VehicleState& state)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_state_mutex);
    m_state = state;
}

void Vehicle::_addDevice(device::IDevicePtr device)
{
    m_devices[device->getName()] = device;
}

void Vehicle::update(double timestep)
{
    if (m_sensorFD >= 0)
    {
        core::ReadWriteMutex::ScopedWriteLock lockState(m_state_mutex);
        boost::mutex::scoped_lock lockSensor(m_sensorBoardMutex);

        // Depth
        double rawDepth = readDepth(m_sensorFD);
        m_state.depth = (rawDepth - m_depthCalibIntercept) /
            m_depthCalibSlope - m_depthOffset;;

        // If we aren't calibrated, take values
        if (!m_calibratedDepth)
        {
            m_depthFilter.addValue(m_state.depth);

            // After five values, take the reading
            if (5 == m_depthFilter.getSize())
            {
                m_calibratedDepth = true;
                m_depthOffset = m_depthFilter.getValue();
            }
        }
        
        //m_state.depth = rawDepth;
        // Status register
        int status = readStatus(m_sensorFD);
        m_state.startSwitch = status & STATUS_STARTSW;

        // Temperatures
        unsigned char temps[NUM_TEMP_SENSORS];
        readTemp(m_sensorFD, temps);

        // Copy the contents of temps into the temperature state
        std::copy(temps, &temps[NUM_TEMP_SENSORS - 1] + 1,
                  m_state.temperatures.begin());
    }
}

void Vehicle::background(int interval) 
{
	BOOST_FOREACH(NameDeviceMap::value_type pair, m_devices)
	{
		device::IDevicePtr device(pair.second);
		core::ConfigNode devCfg(m_config["Devices"][device->getName()]);
		if (devCfg.exists("update_intervale"))
			device->background(devCfg["update_interval"].asInt());
	}
	
    Updatable::background(interval);
}
 
void Vehicle::unbackground(bool join) 
{
	BOOST_FOREACH(NameDeviceMap::value_type pair, m_devices)
	{
		pair.second->unbackground(join);
	}

    Updatable::unbackground(join);
}

void Vehicle::calibrateDepth()
{
    m_depthFilter.clear();
    m_calibratedDepth = false;
}

device::IIMUPtr Vehicle::getIMU()
{
    if (0 == m_imu.get())
        m_imu = device::IDevice::castTo<device::IIMU>(getDevice(m_imuName));
    return m_imu;
}
    
} // namespace vehicle
} // namespace ram
