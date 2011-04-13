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
#include "vehicle/include/Utility.h"
#include "vehicle/include/Events.h"
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/include/device/IThruster.h"
#include "vehicle/include/device/IIMU.h"
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/IPayloadSet.h"
#include "vehicle/include/device/IPositionSensor.h"
#include "vehicle/include/device/IVelocitySensor.h"
#include "vehicle/include/device/LoopStateEstimator.h"
#include "vehicle/include/estimator/ModularStateEstimator.h"
#include "vehicle/include/estimator/IStateEstimator.h"

//#include "sensorapi-r5/include/sensorapi.h"

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
    m_torpedoLauncher(device::IPayloadSetPtr()),
    m_grabberName(config["GrabberName"].asString("Grabber")),
    m_grabber(device::IPayloadSetPtr()),
    stateEstimator(estimator::IStateEstimatorPtr()),
    m_controlSignalToThrusterForces(0.0, 6, 6),
    m_controlSignalToThrusterForcesCreated(false)
{

    /* Make the new state estimator.  This needs to be changed to replace the old one 
       once it is throughly tested.  Right now its added into this file in a 
       hackish way.  Sorry.  This MUST be created before the sensors.  Otherwise,
       it will not receive their configuration values when they publish init events */

    if(!stateEstimator)
    {
        stateEstimator = estimator::IStateEstimatorPtr(
            new estimator::ModularStateEstimator(
                config["NewStateEstimator"],
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps),
                IVehiclePtr(this, null_deleter())));
    }


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
    }

    // Now lets create our default state estimator if we don't have one
    if (!m_stateEstimator)
    {
        LOGGER.info("No state estimator found. "
                    "Creating default LoopStateEstimator.");
        m_stateEstimator = device::IStateEstimatorPtr(
            new device::LoopStateEstimator(
                config,
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps),
                IVehiclePtr(this, null_deleter())));
    }

    // Now set the initial values of the estimator
    LOGGER.info("Setting initial state estimator values.");
    double timeStamp = core::TimeVal::timeOfDay().get_double();
    if (m_depthSensor)
        m_stateEstimator->depthUpdate(getRawDepth(), timeStamp);
    if (m_imu)
        m_stateEstimator->orientationUpdate(getRawOrientation(), timeStamp);
    if (m_velocitySensor)
        m_stateEstimator->velocityUpdate(getRawVelocity(), timeStamp);
    if (m_positionSensor)
        m_stateEstimator->positionUpdate(getRawPosition(), timeStamp);
    
    // If we specified a name of the mag boom we actually have one
    if (m_magBoomName.size() > 0) {
        LOGGER.info("MagBoom initialized.");
        m_hasMagBoom = true;
    }

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
    
double Vehicle::getDepth(std::string obj)
{
    return m_stateEstimator->getDepth(obj);
}

math::Vector2 Vehicle::getPosition(std::string obj)
{
    return m_stateEstimator->getPosition(obj);
}

math::Vector2 Vehicle::getVelocity(std::string obj)
{
    return m_stateEstimator->getVelocity(obj);
}
    
math::Vector3 Vehicle::getLinearAcceleration()
{
    return m_imu->getLinearAcceleration();
}
    
math::Vector3 Vehicle::getAngularRate()
{
    return m_imu->getAngularRate();
}
    
math::Quaternion Vehicle::getOrientation(std::string obj)
{
    return m_stateEstimator->getOrientation(obj);
}

estimator::IStateEstimatorPtr Vehicle::getStateEstimator()
{
    return stateEstimator;
}

bool Vehicle::hasObject(std::string obj)
{
    return m_stateEstimator->hasObject(obj);
}

bool Vehicle::hasMagBoom()
{
    return m_hasMagBoom;
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
        Tuple6Vector3 thrusterLocations = Tuple6Vector3(
            m_starboardThruster->getLocation(),
            m_portThruster->getLocation(),
            m_bottomThruster->getLocation(),
            m_topThruster->getLocation(),
            m_foreThruster->getLocation(),
            m_aftThruster->getLocation());

        m_controlSignalToThrusterForces = 
            createControlSignalToThrusterForcesMatrix(
                thrusterLocations);

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

        //device::LoopStateEstimator* loopEstimator =
        //    boost::dynamic_pointer_cast<device::LoopStateEstimator>(
        //        m_stateEstimator).get();
    if (/*loopEstimator && (*/name == m_stateEstimatorName/*)*/)
    {
        m_stateEstimator =
            device::IDevice::castTo<device::IStateEstimator>(device);
        return 0;
    }
    
    if ((!m_imu) && (name == m_imuName))
    {
        m_imu = device::IDevice::castTo<device::IIMU>(device);

        m_orientationConnection = m_imu->subscribe(
            device::IIMU::UPDATE,
            boost::bind(&Vehicle::onOrientationUpdate, this, _1));
        return 0;
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
        return 0;
    }
    
    if ((!m_depthSensor) && (name == m_depthSensorName))
    {
        m_depthSensor = device::IDevice::castTo<device::IDepthSensor>(device);
        m_depthConnection = m_depthSensor->subscribe(
            device::IDepthSensor::UPDATE,
            boost::bind(&Vehicle::onDepthUpdate, this, _1));
        return 0;
    }
    
    if ((!m_positionSensor) && (name == m_positionSensorName))
    {
        m_positionSensor =
            device::IDevice::castTo<device::IPositionSensor>(device);
        m_positionConnection = m_positionSensor->subscribe(
            device::IPositionSensor::UPDATE,
            boost::bind(&Vehicle::onPositionUpdate, this, _1));
        return 0;
    }

    if ((!m_velocitySensor) && (name == m_velocitySensorName))
    {
        m_velocitySensor =
            device::IDevice::castTo<device::IVelocitySensor>(device);
        m_velocityConnection = m_velocitySensor->subscribe(
            device::IVelocitySensor::UPDATE,
            boost::bind(&Vehicle::onVelocityUpdate, this, _1));
        return 0;
    }

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
    math::NumericEventPtr devent =
        boost::dynamic_pointer_cast<math::NumericEvent>(event);
    
    // Feed the latest value to the estimator, then broadcast the results
    int flags = m_stateEstimator->depthUpdate(getRawDepth(),
                                              devent->timeStamp);
    
    handleReturn(flags);
}

void Vehicle::onOrientationUpdate(core::EventPtr event)
{
    math::OrientationEventPtr oevent =
        boost::dynamic_pointer_cast<math::OrientationEvent>(event);

    // Feed the latest value to the estimator, then broadcast the results
    int flags = m_stateEstimator->orientationUpdate(getRawOrientation(),
                                                    oevent->timeStamp);

    handleReturn(flags);
}

void Vehicle::onPositionUpdate(core::EventPtr event)
{
    math::Vector2EventPtr pevent =
        boost::dynamic_pointer_cast<math::Vector2Event>(event);

    // Feed the latest value to the estimator, then broadcast the results
    int flags = m_stateEstimator->positionUpdate(getRawPosition(),
                                                 pevent->timeStamp);
    
    handleReturn(flags);
}

void Vehicle::onVelocityUpdate(core::EventPtr event)
{
    math::Vector2EventPtr vevent =
        boost::dynamic_pointer_cast<math::Vector2Event>(event);

    // Feed the latest value to the estimator, then broadcast the results
    int flags = m_stateEstimator->velocityUpdate(getRawVelocity(),
                                                 vevent->timeStamp);

    handleReturn(flags);
}

math::MatrixN Vehicle::createControlSignalToThrusterForcesMatrix(
    Tuple6Vector3 thrusterLocations)
{
    Tuple6Vector3 tl = thrusterLocations;

    // make a 6 x 6 matrix that maps thruster force to output force and torque
    math::MatrixN A(0.0, 6, 6);

    // this is the torque calculated for a unit force in the thruster direction
    math::Vector3 tB = tl.get<BOT>().crossProduct(math::Vector3::UNIT_Y);
    math::Vector3 tT = tl.get<TOP>().crossProduct(math::Vector3::UNIT_Y);
    math::Vector3 tF = tl.get<FORE>().crossProduct(math::Vector3::UNIT_Z);
    math::Vector3 tA = tl.get<AFT>().crossProduct(math::Vector3::UNIT_Z);
    math::Vector3 tS = tl.get<STAR>().crossProduct(-math::Vector3::UNIT_X);
    math::Vector3 tP = tl.get<PORT>().crossProduct(-math::Vector3::UNIT_X);


    A[0][0] = 1; A[0][1] = 1;
    A[1][2] = 1; A[1][3] = 1;
    A[2][4] = 1; A[2][5] = 1;
    
    // these terms multiplied by force component give resultant torque
    // torque_x
    A[3][0] = tS[0];
    A[3][1] = tP[0];
    A[3][2] = tB[0];
    A[3][3] = tT[0];
    A[3][4] = tF[0];
    A[3][5] = tA[0];
    
    // torque_y
    A[4][0] = tS[1];
    A[4][1] = tP[1];
    A[4][2] = tB[1];
    A[4][3] = tT[1];
    A[4][4] = tF[1];
    A[4][5] = tA[1];

    // torque_z
    A[5][0] = tS[2];
    A[5][1] = tP[2];
    A[5][2] = tB[2];
    A[5][3] = tT[2];
    A[5][4] = tF[2];
    A[5][5] = tA[2];


    // when given control signal vector b, this will allow 
    // us to efficiently compute x = A_inv * b
    return A.inverse();
}

    
} // namespace vehicle
} // namespace ram
