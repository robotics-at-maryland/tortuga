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
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/Utility.h"
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/include/device/IThruster.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IPayloadSet.h"
#include "vehicle/include/device/IPositionSensor.h"
#include "vehicle/include/device/IVelocitySensor.h"
#include "vehicle/include/device/LoopStateEstimator.h"

//#include "sensorapi-r5/include/sensorapi.h"

#include "math/include/Events.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
#include "core/include/DependencyGraph.h"
#include "core/include/EventConnection.h"

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
    m_stateEstimatorName(config["StateEstimatorName"].asString("StateEstimator")),
    m_stateEstimator(device::IStateEstimatorPtr()),
    m_imuName(config["IMUName"].asString("IMU")),
    m_imu(device::IIMUPtr()),
    m_hasMagBoom(false),
    m_magBoomName(config["MagBoomName"].asString("")),
    m_magBoom(device::IIMUPtr()),
    m_depthSensorName(config["DepthSensorName"].asString("SensorBoard")),
    m_depthSensor(device::IDepthSensorPtr()),
    m_velocitySensorName(config["VelocitySensorName"].asString("VelocitySensor")),
    m_velocitySensor(device::IVelocitySensorPtr()),
    m_positionSensorName(config["PositionSensorName"].asString("PositionSensor")),
    m_positionSensor(device::IPositionSensorPtr()),
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

    // Now lets create our default state estimator if we don't have one
    if (!m_stateEstimator)
    {
        m_stateEstimator = device::IStateEstimatorPtr(
            new device::LoopStateEstimator(
                config,
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps),
                IVehiclePtr(this, null_deleter())));
    }

    // Now set the intial values of the estimator
    if (m_depthSensor)
        m_stateEstimator->depthUpdate(getRawDepth());
    if (m_imu)
        m_stateEstimator->orientationUpdate(getRawOrientation());
    if (m_velocitySensor)
        m_stateEstimator->velocityUpdate(getRawVelocity());
    if (m_positionSensor)
        m_stateEstimator->positionUpdate(getRawPosition());
    
    // If we specified a name of the mag boom we actually have one
    if (m_magBoomName.size() > 0)
        m_hasMagBoom = true;

    // Make sure thrusters are unsafed
    //unsafeThrusters();
}

Vehicle::~Vehicle()
{
    // Unconnect from all the events
    if (m_depthConnection)
        m_depthConnection->disconnect();

    if (m_orientationConnection)
        m_orientationConnection->disconnect();

    if (m_positionConnection)
        m_positionConnection->disconnect();

    if (m_velocityConnection)
        m_velocityConnection->disconnect();

    
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
    return m_stateEstimator->getDepth();
}

math::Vector2 Vehicle::getPosition()
{
    return m_stateEstimator->getPosition();
}

math::Vector2 Vehicle::getVelocity()
{
    return m_stateEstimator->getVelocity();
}
    
math::Vector3 Vehicle::getLinearAcceleration()
{
    return m_imu->getLinearAcceleration();
}
    
math::Vector3 Vehicle::getAngularRate()
{
    return m_imu->getAngularRate();
}
    
math::Quaternion Vehicle::getOrientation()
{
    return m_stateEstimator->getOrientation();
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
    std::string name(device->getName());
    m_devices[name] = device;

    device::LoopStateEstimator* loopEstimator =
        boost::dynamic_pointer_cast<device::LoopStateEstimator>(
            m_stateEstimator).get();
    if (loopEstimator && (name == m_stateEstimatorName))
    {
        m_stateEstimator =
            device::IDevice::castTo<device::IStateEstimator>(device);
    }
    
    if ((!m_imu) && (name == m_imuName))
    {
        m_imu = device::IDevice::castTo<device::IIMU>(device);

        m_orientationConnection = m_imu->subscribe(
            device::IIMU::UPDATE,
            boost::bind(&Vehicle::onOrientationUpdate, this, _1));
    }
    
    if ((!m_magBoom) && (name == m_magBoomName))
    {
        m_magBoom = device::IDevice::castTo<device::IIMU>(device);

        // Disconnect from normal IMU event if we have one
        if (m_orientationConnection)
            m_orientationConnection->disconnect();
        
        m_orientationConnection = m_magBoom->subscribe(
            device::IIMU::UPDATE,
            boost::bind(&Vehicle::onOrientationUpdate, this, _1));
    }
    
    if ((!m_depthSensor) && (name == m_depthSensorName))
    {
        m_depthSensor = device::IDevice::castTo<device::IDepthSensor>(device);
        m_depthConnection = m_depthSensor->subscribe(
            device::IDepthSensor::UPDATE,
            boost::bind(&Vehicle::onDepthUpdate, this, _1));
    }
    
    if ((!m_positionSensor) && (name == m_positionSensorName))
    {
        m_positionSensor =
            device::IDevice::castTo<device::IPositionSensor>(device);
        m_positionConnection = m_positionSensor->subscribe(
            device::IPositionSensor::UPDATE,
            boost::bind(&Vehicle::onPositionUpdate, this, _1));
    }

    if ((!m_velocitySensor) && (name == m_velocitySensorName))
    {
        m_velocitySensor =
            device::IDevice::castTo<device::IVelocitySensor>(device);
        m_velocityConnection = m_velocitySensor->subscribe(
            device::IVelocitySensor::UPDATE,
            boost::bind(&Vehicle::onVelocityUpdate, this, _1));
    }

    if ((!m_markerDropper) && (name == m_markerDropperName))
    {
        m_markerDropper =
            device::IDevice::castTo<device::IPayloadSet>(device);
    }

    if ((!m_torpedoLauncher) && (name == m_torpedoLauncherName))
    {
        m_torpedoLauncher =
            device::IDevice::castTo<device::IPayloadSet>(device);
    }
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

double Vehicle::getRawDepth()
{
    // Get the reference to our depth sensor
    device::IDepthSensorPtr depthSensor = m_depthSensor;

    // Determine depth correction
    math::Vector3 initialSensorLocation = depthSensor->getLocation();
    math::Vector3 currentSensorLocation = 
      getOrientation() * initialSensorLocation;
    math::Vector3 sensorMovement = 
      currentSensorLocation - initialSensorLocation;
    double correction = sensorMovement.z;

    // Grab the depth
    double depth = depthSensor->getDepth();

    // Return the corrected depth (its addition and not subtraction because
    // depth is positive down)
    return depth + correction;
}

math::Vector2 Vehicle::getRawPosition()
{
    return m_positionSensor->getPosition();
}

math::Vector2 Vehicle::getRawVelocity()
{
    return m_velocitySensor->getVelocity();
}

math::Quaternion Vehicle::getRawOrientation()
{
    if (m_hasMagBoom)
    {
        return Utility::quaternionFromMagAccel(
            m_magBoom->getMagnetometer(),
            m_imu->getLinearAcceleration());
    }
    else
    {
        return m_imu->getOrientation();
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

void Vehicle::onDepthUpdate(core::EventPtr event)
{
    math::NumericEventPtr nevent =
        boost::dynamic_pointer_cast<math::NumericEvent>(event);
    
    // Feed the latest value to the estimator, then broadcast the results
    m_stateEstimator->depthUpdate(getRawDepth());
    nevent->number = m_stateEstimator->getDepth();
    
    publish(IVehicle::DEPTH_UPDATE, event);
}

void Vehicle::onOrientationUpdate(core::EventPtr event)
{
    math::OrientationEventPtr oevent =
        boost::dynamic_pointer_cast<math::OrientationEvent>(event);

    // Feed the latest value to the estimator, then broadcast the results
    m_stateEstimator->orientationUpdate(getRawOrientation());
    oevent->orientation = m_stateEstimator->getOrientation();
    
    publish(IVehicle::ORIENTATION_UPDATE, event);
}

void Vehicle::onPositionUpdate(core::EventPtr event)
{
    math::Vector2EventPtr oevent =
        boost::dynamic_pointer_cast<math::Vector2Event>(event);

    // Feed the latest value to the estimator, then broadcast the results
    m_stateEstimator->positionUpdate(getRawPosition());
    oevent->vector2 = m_stateEstimator->getPosition();
    
    publish(IVehicle::POSITION_UPDATE, event);
}

void Vehicle::onVelocityUpdate(core::EventPtr event)
{
    math::Vector2EventPtr oevent =
        boost::dynamic_pointer_cast<math::Vector2Event>(event);

    // Feed the latest value to the estimator, then broadcast the results
    m_stateEstimator->velocityUpdate(getRawVelocity());
    oevent->vector2 = m_stateEstimator->getVelocity();
    
    publish(IVehicle::VELOCITY_UPDATE, event);
}

    
} // namespace vehicle
} // namespace ram
