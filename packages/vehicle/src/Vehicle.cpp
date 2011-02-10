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
#include <string>
#ifdef RAM_POSIX
    #include <unistd.h>
#endif

// Library Includes
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <log4cpp/Category.hh>
#include "boost/tuple/tuple.hpp"

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/Events.h"
#include "vehicle/include/device/IPayloadSet.h"
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/include/device/IThruster.h"

#include "math/include/Events.h"
#include "math/include/Vector2.h"
#include "math/include/Matrix2.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
#include "core/include/DependencyGraph.h"
#include "core/include/EventConnection.h"
#include "core/include/TimeVal.h"

// Register vehicle into the maker subsystem
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vehicle::Vehicle, Vehicle);

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Vehicle"));
static log4cpp::Category& TLOGGER(log4cpp::Category::getInstance("Thrusters"));

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
    Updatable(this),
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
    m_topThrusterThrottle(config["TopThrusterThrottle"].asDouble(1.0)),
    m_markerDropperName(config["MarkerDropperName"].asString("MarkerDropper")),
    m_markerDropper(device::IPayloadSetPtr()),
    m_torpedoLauncherName(config["TorpedoLauncherName"].asString("TorpedoLauncher")),
    m_torpedoLauncher(device::IPayloadSetPtr()),
    m_grabberName(config["GrabberName"].asString("Grabber")),
    m_grabber(device::IPayloadSetPtr()),
    m_controlSignalToThrusterForces(0.0, 6, 6),
    m_controlSignalToThrusterForcesCreated(false)
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

            LOGGER.infoStream() << "Creating device "
                                << node["name"].asString()
                                << " of type: "
                                << node["type"].asString();
            
            // TODO: Figure out how to get my python object if needed
            device::IDeviceMakerParamType params(
                node,
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps),
                IVehiclePtr(this, null_deleter()));
            
            if (!_addDevice(device::IDeviceMaker::newObject(params))) {
                LOGGER.infoStream() << "Sucessfully set device "
                                    << node["name"].asString()
                                    << " of type: "
                                    << node["type"].asString();
            }
        }
        TLOGGER.info("starboard port bottom top fore aft");
    }



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
    m_markerDropper->releaseObject();
}

void Vehicle::fireTorpedo()
{
    m_torpedoLauncher->releaseObject();
}

void Vehicle::releaseGrabber()
{
    m_grabber->releaseObject();
}
    
void Vehicle::applyForcesAndTorques(const math::Vector3& translationalForces,
                                    const math::Vector3& rotationalTorques)
{
    // Bail out if we don't currently have all thruster devices
    if (!lookupThrusterDevices())
        return;

    if(!m_controlSignalToThrusterForcesCreated)
    {
        Tuple6D thrusterDistanceFromCG = Tuple6D(
            m_starboardThruster->getOffset(),
            m_portThruster->getOffset(),
            m_bottomThruster->getOffset(),
            m_topThruster->getOffset(),
            m_foreThruster->getOffset(),
            m_aftThruster->getOffset());

        m_controlSignalToThrusterForces = 
            createControlSignalToThrusterForcesMatrix(
                thrusterDistanceFromCG);

        m_controlSignalToThrusterForcesCreated = true;
    }

    /****** Calculate Individual Thruster Forces ********/
    
    // Thruster order convention is based on the direction of
    // the force applied and the offset location
    // STAR, PORT, TOP, BOT, FORE, AFT

    math::VectorN controlSignal(0.0, 6);
    controlSignal[0] = translationalForces[0];
    controlSignal[1] = translationalForces[1];
    controlSignal[2] = translationalForces[2];
    controlSignal[3] = rotationalTorques[0];
    controlSignal[4] = rotationalTorques[1];
    controlSignal[5] = rotationalTorques[2];

    math::VectorN thrusterForces = 
        m_controlSignalToThrusterForces * controlSignal;


    /****** Set Thruster Forces *************************/

    m_starboardThruster->setForce(thrusterForces[STAR]);
    m_portThruster->setForce(thrusterForces[PORT]);
    m_bottomThruster->setForce(thrusterForces[BOT]);
    m_topThruster->setForce(thrusterForces[TOP]);
    m_foreThruster->setForce(thrusterForces[FORE]);
    m_aftThruster->setForce(thrusterForces[AFT]);


    ThrustUpdateEventPtr event = ThrustUpdateEventPtr(
        new ThrustUpdateEvent());
    event->forces = translationalForces;
    event->torques = rotationalTorques;
    publish(VEHICLE_THRUST_UPDATE ,event);

    TLOGGER.infoStream() << thrusterForces[STAR] << " "
                         << thrusterForces[PORT] << " "
                         << thrusterForces[BOT] << " "
                         << thrusterForces[TOP] << " "
                         << thrusterForces[FORE] << " "
                         << thrusterForces[AFT];
}
    
int Vehicle::_addDevice(device::IDevicePtr device)
{
    std::string name(device->getName());
    m_devices[name] = device;
    
    if ((!m_markerDropper) && (name == m_markerDropperName))
    {
        m_markerDropper =
            device::IDevice::castTo<device::IPayloadSet>(device);
        return 0;
    }

    if ((!m_torpedoLauncher) && (name == m_torpedoLauncherName))
    {
        m_torpedoLauncher =
            device::IDevice::castTo<device::IPayloadSet>(device);
        return 0;
    }

    if ((!m_grabber) && (name == m_grabberName))
    {
        m_grabber =
            device::IDevice::castTo<device::IPayloadSet>(device);
        return 0;
    }

    // Failure to set a device
    return 1;
}

void Vehicle::update(double timestep)
{
    // Update the devices if we are not running the background
    if (!backgrounded())
    {
        BOOST_FOREACH(NameDeviceMap::value_type pair, m_devices)
            pair.second->update(timestep);
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

bool Vehicle::lookupThrusterDevices()
{
    bool good = true;
    
    if (!m_starboardThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_starboardThrusterName);
        if(iter != m_devices.end())
            m_starboardThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);
        else
            good = false;
    }

    if (!m_portThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_portThrusterName);
        if(iter != m_devices.end())
            m_portThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);
        else
            good = false;
    }

    if (!m_foreThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_foreThrusterName);
        if(iter != m_devices.end())
            m_foreThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);
        else
            good = false;
    }

    if (!m_aftThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_aftThrusterName);
        if(iter != m_devices.end())
            m_aftThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);
        else
            good = false;
    }

    if (!m_topThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_topThrusterName);
        if(iter != m_devices.end())
            m_topThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);
        else
            good = false;
    }
    
    if (!m_bottomThruster)
    {
        NameDeviceMapIter iter = m_devices.find(m_bottomThrusterName);
        if(iter != m_devices.end())
            m_bottomThruster = device::IDevice::castTo<device::IThruster>(
                (*iter).second);
        else
            good = false;
    }

    return good;
}
    
math::MatrixN Vehicle::createControlSignalToThrusterForcesMatrix(
    Tuple6D thrusterDistanceFromCG)
{
    Tuple6D td = thrusterDistanceFromCG;

    // make a 6 x 6 matrix that maps thruster forces to output forces and torques
    math::MatrixN A(0.0, 6, 6);
    A[0][0] = 1; A[0][1] = 1;
    A[1][2] = 1; A[1][3] = 1;
    A[2][4] = 1; A[2][5] = 1;
    A[3][2] = -td.get<BOT>(); A[3][3] = td.get<TOP>();
    A[4][4] = -td.get<FORE>(); A[4][5] = td.get<AFT>();
    A[5][0] = td.get<STAR>(); A[5][1] = -td.get<PORT>();

    // when given control signal vector b, this will allow 
    // us to efficiently compute x = A_inv * b
    return A.inverse();
}


} // namespace vehicle
} // namespace ram
