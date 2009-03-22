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
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IPayloadSet.h"

//#include "sensorapi-r5/include/sensorapi.h"

#include "math/include/Events.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
#include "core/include/DependencyGraph.h"

// Register vehicle into the maker subsystem
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vehicle::Vehicle, Vehicle);

using namespace ram::vehicle::device;

namespace ram {
namespace vehicle {

// Lets us have a shared pointer, with no ownership requirements
struct null_deleter
{
    void operator()(void const *) const
    {
    }
};
    
Vehicle::Vehicle(core::ConfigNode config, core::SubsystemList deps) :
    IVehicle(config["name"].asString(),
             core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_config(config),
    m_starboardThrusterName(
        config["StarboardThrusterName"].asString("StarboardThruster")),
    m_starboardThruster(device::IThrusterPtr()),
    m_portThrusterName(
        config["PortThrusterName"].asString("PortThruster")),
    m_portThruster(device::IThrusterPtr()),
    m_foreThrusterName(
        config["ForeThrusterName"].asString("ForeThruster")),
    m_foreThruster(device::IThrusterPtr()),
    m_aftThrusterName(
        config["AftThrusterName"].asString("AftThruster")),
    m_aftThruster(device::IThrusterPtr()),
    m_topThrusterName(
        config["TopThrusterName"].asString("TopThruster")),
    m_topThruster(device::IThrusterPtr()),
    m_bottomThrusterName(
        config["BottomThrusterName"].asString("BottomThruster")),
    m_bottomThruster(device::IThrusterPtr()),
    m_imuName(config["IMUName"].asString("IMU")),
    m_imu(device::IIMUPtr()),
    m_depthSensorName(config["DepthSensorName"].asString("SensorBoard")),
    m_depthSensor(device::IDepthSensorPtr()),
    m_markerDropperName(config["MarkerDropperName"].asString("MarkerDropper")),
    m_markerDropper(device::IPayloadSetPtr()),
    m_torpedoLauncherName(config["TorpedoLauncherName"].asString("TorpedoLauncher")),
    m_torpedoLauncher(device::IPayloadSetPtr())
{
    // Create devices
    if (config.exists("Devices"))
    {
        core::ConfigNode deviceConfig(config["Devices"]);

        // Properly fills m_order, and m_subsystemDeps
        core::DependencyGraph depGraph(deviceConfig);
        std::vector<std::string> deviceOrder = depGraph.getOrder();
        
        BOOST_FOREACH(std::string deviceName, deviceOrder)
        {
            core::ConfigNode node(deviceConfig[deviceName]);
            node.set("name", deviceName);
            // TODO: Make me a log
            //std::cout << "Creating device " << node["name"].asString()
            //    << " of type: " << node["type"].asString() << std::endl;
            
            // TODO: Figure out how to get my python object if needed
            device::IDeviceMakerParamType params(
                node,
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps),
                IVehiclePtr(this, null_deleter()));
            
            _addDevice(device::IDeviceMaker::newObject(params));
        }
    }

    // Make sure thrusters are unsafed
    //unsafeThrusters();
}

Vehicle::~Vehicle()
{
    // For safeties sake send a zero torque and force command which will kill
    // any current thruster power
    for (int i = 0; i < 2; ++i)
    {
        applyForcesAndTorques(math::Vector3::ZERO, math::Vector3::ZERO);
        usleep(30 * 1000);
    }
    
    // Remove all references to the devices, will cause them to be destructed
    // this will cause the Thruster objects to be deleted and set the 
    // thrusters to nuetral.  The lone problem here is that these objects are
    // reference counted, so we can't be sure that there are not any 
    // references  floating around keeping the object from being destoryed.
    m_devices.clear();

    // Stop all background threads (does not include device background threads)
    unbackground(true);
}
    
device::IDevicePtr Vehicle::getDevice(std::string name)
{
    NameDeviceMapIter iter = m_devices.find(name);
    if (iter == m_devices.end())
        std::cout << "Could not find device: \"" << name << "\"" << std::endl;
    assert(iter != m_devices.end() && "Error Device not found");
    return (*iter).second;
}

std::vector<std::string> Vehicle::getDeviceNames()
{
    std::vector<std::string> names;
    BOOST_FOREACH(NameDeviceMap::value_type pair, m_devices)
    {
        names.push_back(pair.first);
    }

    return names;
}
    
double Vehicle::getDepth()
{
    return getDepthSensor()->getDepth();
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
    if (lookupThrusterDevices())
    {
        m_starboardThruster->setEnabled(false);
        m_portThruster->setEnabled(false);
        m_foreThruster->setEnabled(false);
        m_aftThruster->setEnabled(false);
        m_topThruster->setEnabled(false);
        m_bottomThruster->setEnabled(false);
    }
}

void Vehicle::unsafeThrusters()
{
    if (lookupThrusterDevices())
    {
        m_starboardThruster->setEnabled(true);
        m_portThruster->setEnabled(true);
        m_foreThruster->setEnabled(true);
        m_aftThruster->setEnabled(true);
        m_topThruster->setEnabled(true);
        m_bottomThruster->setEnabled(true);
    }
}

void Vehicle::dropMarker()
{
    getMarkerDropper()->releaseObject();
}

void Vehicle::fireTorpedo()
{
    getTorpedoLauncher()->releaseObject();
}
    
void Vehicle::applyForcesAndTorques(const math::Vector3& translationalForces,
                                    const math::Vector3& rotationalTorques)
{
    // Bail out if we don't currently have all thruster devices
    if (!lookupThrusterDevices())
        return;

    // Calculate indivdual thruster foces
    double star = translationalForces[0] / 2 +
        0.5 * rotationalTorques[2] / m_starboardThruster->getOffset();
    double port = translationalForces[0] / 2 -
        0.5 * rotationalTorques[2] / m_portThruster->getOffset();
    double fore = translationalForces[2] / 2 -
        0.5 * rotationalTorques[1] / m_foreThruster->getOffset();
    double aft = translationalForces[2]/2 +
        0.5 * rotationalTorques[1] / m_aftThruster->getOffset();
    double top = translationalForces[1] / 2 +
        0.5 * rotationalTorques[0] / m_topThruster->getOffset();
    double bottom = translationalForces[1]/2 -
        0.5 * rotationalTorques[0] / m_bottomThruster->getOffset();

//        std::cout << "Force: " << star << " " << port << " " << fore
//                  << " " << aft << std::endl;

    // Set actual thruster forces
    m_starboardThruster->setForce(star);
    m_portThruster->setForce(port);
    m_foreThruster->setForce(fore);
    m_aftThruster->setForce(aft);
    m_topThruster->setForce(top);
    m_bottomThruster->setForce(bottom);
}
    
void Vehicle::_addDevice(device::IDevicePtr device)
{
    m_devices[device->getName()] = device;
}

void Vehicle::update(double timestep)
{
    if (m_devices.end() != m_devices.find(m_imuName))
    {    
        math::OrientationEventPtr oevent(new math::OrientationEvent());
        oevent->orientation = getOrientation();
        publish(IVehicle::ORIENTATION_UPDATE, oevent);
    }

    if (m_devices.end() != m_devices.find(m_depthSensorName))
    {    
        math::NumericEventPtr nevent(new math::NumericEvent());
        nevent->number = getDepth();
        publish(IVehicle::DEPTH_UPDATE, nevent);
    }
}

void Vehicle::setPriority(core::IUpdatable::Priority priority)
{
    BOOST_FOREACH(NameDeviceMap::value_type pair, m_devices)
    {
        pair.second->setPriority(priority);
    }

    Updatable::setPriority(priority);    
}

void Vehicle::setAffinity(size_t affinity)
{
    BOOST_FOREACH(NameDeviceMap::value_type pair, m_devices)
    {
        pair.second->setAffinity(affinity);
    }

    Updatable::setAffinity(affinity);    
}
    
void Vehicle::background(int interval) 
{
    core::ConfigNode deviceConfig(m_config["Devices"]);
    
    BOOST_FOREACH(NameDeviceMap::value_type pair, m_devices)
    {
        device::IDevicePtr device(pair.second);
        if (deviceConfig.exists(device->getName()))
        {
            core::ConfigNode devCfg(deviceConfig[device->getName()]);
            if (devCfg.exists("update_interval"))
                device->background(devCfg["update_interval"].asInt());

	    if (devCfg.exists("priority"))
	        device->setPriority(
                    IUpdatable::stringToPriority(devCfg["priority"].asString()));

            if (devCfg.exists("affinity"))
                device->setAffinity(devCfg["affinity"].asInt());
        }
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

device::IIMUPtr Vehicle::getIMU()
{
    if (!m_imu)
        m_imu = device::IDevice::castTo<device::IIMU>(getDevice(m_imuName));
    return m_imu;
}

device::IDepthSensorPtr Vehicle::getDepthSensor()
{
    if (!m_depthSensor)
    {
        m_depthSensor = device::IDevice::castTo<device::IDepthSensor>(
            getDevice(m_depthSensorName));
    }
    return m_depthSensor;
}

device::IPayloadSetPtr Vehicle::getMarkerDropper()
{
    if (!m_markerDropper)
    {
        m_markerDropper = device::IDevice::castTo<device::IPayloadSet>(
            getDevice(m_markerDropperName));
    }
    return m_markerDropper;
}

device::IPayloadSetPtr Vehicle::getTorpedoLauncher()
{
    if (!m_torpedoLauncher)
    {
        m_torpedoLauncher = device::IDevice::castTo<device::IPayloadSet>(
            getDevice(m_torpedoLauncherName));
    }
    return m_torpedoLauncher;
}

    
bool Vehicle::lookupThrusterDevices()
{
    bool good = true;
    
    if (!m_starboardThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_starboardThrusterName);
        if(iter != m_devices.end())
        {
            m_starboardThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);

        }
        else
        {
            good = false;
        }
    }

    if (!m_portThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_portThrusterName);
        if(iter != m_devices.end())
        {
            m_portThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);

        }
        else
        {
            good = false;
        }
    }

    if (!m_foreThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_foreThrusterName);
        if(iter != m_devices.end())
        {
            m_foreThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);

        }
        else
        {
            good = false;
        }
    }

    if (!m_aftThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_aftThrusterName);
        if(iter != m_devices.end())
        {
            m_aftThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);

        }
        else
        {
            good = false;
        }
    }

    if (!m_topThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_topThrusterName);
        if(iter != m_devices.end())
        {
            m_topThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);

        }
        else
        {
            good = false;
        }
    }
    
    if (!m_bottomThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_bottomThrusterName);
        if(iter != m_devices.end())
        {
            m_bottomThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);

        }
        else
        {
            good = false;
        }
    }

    return good;
}

} // namespace vehicle
} // namespace ram
