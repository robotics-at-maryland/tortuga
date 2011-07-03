/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestTortuga.cxx
 */

// STD Includes
#include <set>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/Events.h"
#include "vehicle/include/device/IThruster.h"
#include "vehicle/include/device/Common.h"

#include "core/include/ConfigNode.h"
#include "core/include/EventConnection.h"
#include "core/include/EventHub.h"

#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"

#include "vehicle/test/include/MockDevice.h"
#include "vehicle/test/include/MockIMU.h"
#include "vehicle/test/include/MockThruster.h"
#include "vehicle/test/include/MockDepthSensor.h"
#include "vehicle/test/include/MockVelocitySensor.h"
#include "vehicle/test/include/MockPositionSensor.h"
#include "vehicle/test/include/MockStateEstimator.h"

using namespace ram;


static const std::string CONFIG("{'name' : 'TestVehicle'}");

struct VehicleFixture
{
    VehicleFixture() :
        eventHub(new core::EventHub()),
        veh(new vehicle::Vehicle(core::ConfigNode::fromString(CONFIG),
                                 boost::assign::list_of(eventHub)))
    {
    }
    
    ~VehicleFixture() {
        delete veh;
    }

    core::EventHubPtr eventHub;
    vehicle::Vehicle* veh;
};

void eventHelper(std::string* eventType, ram::core::EventPtr event)
{
    *eventType = event->type;
}

TEST(DeviceCreation)
{
    std::string config =
            "{"
            "'name' : 'TestVehicle',"
            "'Devices' : {"
            "    'IMU' : {'type' : 'MockIMU'},"
            "    'PSU' : {'type' : 'MockDevice'}"
            " },"
            "}";

    core::EventHubPtr eventHub(new core::EventHub());
    vehicle::IVehicle* veh = 
        new vehicle::Vehicle(core::ConfigNode::fromString(config),
                             boost::assign::list_of(eventHub));
    
    CHECK_EQUAL("IMU", veh->getDevice("IMU")->getName());
    CHECK_EQUAL("PSU", veh->getDevice("PSU")->getName());

    // Check to make sure the device got the right vehicle
    MockDevice* dev = dynamic_cast<MockDevice*>(veh->getDevice("PSU").get());
    CHECK(dev);
    CHECK_EQUAL(veh, dev->vehicle.get());
    
    // Check to make sure the list of names matches
    std::set<std::string> expNames = boost::assign::list_of("IMU")("PSU");
    std::set<std::string> givenNames;
    std::vector<std::string> names = veh->getDeviceNames();

    BOOST_FOREACH(std::string name, names)
    {
        givenNames.insert(name);
    }

    CHECK_EQUAL(2u, givenNames.size());
    CHECK(expNames == givenNames);

    // Make sure events get through
    std::string eventType;

    eventHub->subscribeToType(
        vehicle::device::IThruster::FORCE_UPDATE,
        boost::bind(eventHelper, &eventType, _1));

    veh->getDevice("IMU")->publish(vehicle::device::IThruster::FORCE_UPDATE,
                                   core::EventPtr(new core::Event()));

    CHECK_EQUAL(vehicle::device::IThruster::FORCE_UPDATE, eventType);
    delete veh;
}

TEST(ValueInitialization)
{
    // Create a vehicle with the some mock sensors that have initial state
    std::string config =
            "{"
            "'name' : 'TestVehicle',"
            "'Devices' : {"
            "    'IMU' : {'type' : 'MockIMU'},"
            "    'DepthSensor' : {'type' : 'MockDepthSensor'},"
            "    'VelocitySensor' : {'type' : 'MockVelocitySensor'},"
            "    'PositionSensor' : {'type' : 'MockPositionSensor'},"
            " },"
            "}";

    core::EventHubPtr eventHub(new core::EventHub());
    vehicle::IVehicle* veh = 
        new vehicle::Vehicle(core::ConfigNode::fromString(config),
                             boost::assign::list_of(eventHub));
    
    CHECK(veh != 0);
}

TEST_FIXTURE(VehicleFixture, _addDevice)
{
    MockDevice* mockDevice = new MockDevice("TestName");
    veh->_addDevice(vehicle::device::IDevicePtr(mockDevice));

    CHECK_EQUAL(mockDevice, veh->getDevice("TestName").get());
    CHECK_EQUAL("TestName", veh->getDevice("TestName")->getName());
}

struct ThrusterVehicleFixture
{
    ThrusterVehicleFixture() :
        eventHub(new core::EventHub()),
        veh(new vehicle::Vehicle(core::ConfigNode::fromString(CONFIG),
                                 boost::assign::list_of(eventHub))),
        starboard(new MockThruster("StarboardThruster")),
        port(new MockThruster("PortThruster")),
        fore(new MockThruster("ForeThruster")),
        aft(new MockThruster("AftThruster")),
        top(new MockThruster("TopThruster")),
        bottom(new MockThruster("BottomThruster"))
    {
        veh->_addDevice(vehicle::device::IDevicePtr(starboard));
        veh->_addDevice(vehicle::device::IDevicePtr(port));
        veh->_addDevice(vehicle::device::IDevicePtr(fore));
        veh->_addDevice(vehicle::device::IDevicePtr(aft));
        veh->_addDevice(vehicle::device::IDevicePtr(top));
        veh->_addDevice(vehicle::device::IDevicePtr(bottom));

        for (int i = 0; i < 6; ++i)
            forceArray[i] = 0.0;
    }
    
    ~ThrusterVehicleFixture() {
        delete veh;
    }

    double* thrusterForceArray()
    {
        forceArray[0] = port->force;
        forceArray[1] = starboard->force;
        forceArray[2] = top->force;
        forceArray[3] = fore->force;
        forceArray[4] = bottom->force;
        forceArray[5] = aft->force;
        return forceArray;
    }
    
    core::EventHubPtr eventHub;
    vehicle::Vehicle* veh;
    MockThruster* starboard;
    MockThruster* port;
    MockThruster* fore;
    MockThruster* aft;
    MockThruster* top;
    MockThruster* bottom;
private:
    double forceArray[6];
};

TEST_FIXTURE(ThrusterVehicleFixture, applyForcesAndTorque)
{
    // Make all thrusters have locations unit distance from CM
    starboard->location = math::Vector3::UNIT_Y;
    port->location = math::Vector3::NEGATIVE_UNIT_Y;
    fore->location = math::Vector3::UNIT_X;
    aft->location = math::Vector3::NEGATIVE_UNIT_X;
    top->location = math::Vector3::NEGATIVE_UNIT_Z;
    bottom->location = math::Vector3::UNIT_Z;

    starboard->direction = math::Vector3::NEGATIVE_UNIT_X;
    port->direction = math::Vector3::NEGATIVE_UNIT_X;
    fore->direction = math::Vector3::UNIT_Z;
    aft->direction = math::Vector3::UNIT_Z;
    top->direction = math::Vector3::UNIT_Y;
    bottom->direction = math::Vector3::UNIT_Y;
    

    // +X Torque
    veh->applyForcesAndTorques(ram::math::Vector3::ZERO,
                               ram::math::Vector3(5, 0, 0));
    double expectedForcesPosXTorque[] = {
        0.0, // Port
        0.0, // Starboard
        2.5, // Top
        0.0, // Fore
        -2.5, // Bottom
        0.0, // Aft
    };
    CHECK_ARRAY_EQUAL(expectedForcesPosXTorque, thrusterForceArray(), 6);

    // +Y Force
    veh->applyForcesAndTorques(ram::math::Vector3(0, 5, 0),
                               ram::math::Vector3::ZERO);
    double expectedForcesPosYForce[] = {
        0.0, // Port
        0.0, // Starboard
        2.5, // Top
        0.0, // Fore
        2.5, // Bottom
        0.0, // Aft
    };
    CHECK_ARRAY_EQUAL(expectedForcesPosYForce, thrusterForceArray(), 6);

    // +Y Torque
    veh->applyForcesAndTorques(ram::math::Vector3::ZERO,
                               ram::math::Vector3(0, 6, 0));
    double expectedForcesPosYTorque[] = {
        0.0, // Port
        0.0, // Starboard
        0.0, // Top
        -3.0, // Fore
        0.0, // Bottom
        3.0, // Aft
    };
   CHECK_ARRAY_EQUAL(expectedForcesPosYTorque, thrusterForceArray(), 6);

    // +Z Force
    veh->applyForcesAndTorques(ram::math::Vector3(0, 0, 6),
                               ram::math::Vector3::ZERO);
    double expectedForcesPosZForce[] = {
        0.0, // Port
        0.0, // Starboard
        0.0, // Top
        3.0, // Fore
        0.0, // Bottom
        3.0, // Aft
    };
    CHECK_ARRAY_EQUAL(expectedForcesPosZForce, thrusterForceArray(), 6);

    // +Z Torque
    veh->applyForcesAndTorques(ram::math::Vector3::ZERO,
                               ram::math::Vector3(0, 0, 7.5));
    double expectedForcesPosZTorque[] = {
        -3.75, // Port
        3.75,  // Starboard
        0.0, // Top
        0.0, // Fore
        0.0, // Bottom
        0.0, // Aft
    };
    CHECK_ARRAY_EQUAL(expectedForcesPosZTorque, thrusterForceArray(), 6);

    // +X Force
    veh->applyForcesAndTorques(ram::math::Vector3(7.5, 0, 0),
                               ram::math::Vector3::ZERO);
    double expectedForcesPosXForce[] = {
        3.75, // Port
        3.75, // Starboard
        0.0, // Top
        0.0, // Fore
        0.0, // Bottom
        0.0, // Aft
    };
    CHECK_ARRAY_EQUAL(expectedForcesPosXForce, thrusterForceArray(), 6);
}

TEST_FIXTURE(ThrusterVehicleFixture, safeThrusters)
{
    starboard->enabled = true;
    port->enabled = true;
    fore->enabled = true;
    aft->enabled = true;
    top->enabled = true;
    bottom->enabled = true;

    veh->safeThrusters();

    CHECK_EQUAL(false, starboard->enabled);
    CHECK_EQUAL(false, port->enabled);
    CHECK_EQUAL(false, fore->enabled);
    CHECK_EQUAL(false, aft->enabled);
    CHECK_EQUAL(false, top->enabled);
    CHECK_EQUAL(false, bottom->enabled);
}

TEST_FIXTURE(ThrusterVehicleFixture, unsafeThrusters)
{
    veh->unsafeThrusters();

    CHECK_EQUAL(true, starboard->enabled);
    CHECK_EQUAL(true, port->enabled);
    CHECK_EQUAL(true, fore->enabled);
    CHECK_EQUAL(true, aft->enabled);
    CHECK_EQUAL(true, top->enabled);
    CHECK_EQUAL(true, bottom->enabled);
}

TEST_FIXTURE(ThrusterVehicleFixture, getDevicesOfType)
{
    // Add a non-thruster device
    MockIMU* imu = new MockIMU("IMU");
    veh->_addDevice(vehicle::device::IDevicePtr(imu));

    vehicle::device::DeviceList devices =
        veh->getDevicesOfType<vehicle::device::IThruster>();
    CHECK_EQUAL(6u, devices.size());

    devices = veh->getDevicesOfType<vehicle::device::IIMU>();
    CHECK_EQUAL(1u, devices.size());
}
