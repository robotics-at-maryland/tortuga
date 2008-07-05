/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestSBThruster.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vehicle/include/device/SBThruster.h"
#include "vehicle/include/device/SensorBoard.h"

#include "vehicle/test/include/MockVehicle.h"
#include "vehicle/test/include/MockSensorBoard.h"

static const std::string SB_CONFIG(
    "{'name' : 'TestVehicle',"
    "'depthCalibSlope' : 33.01,"
    "'depthCalibIntercept' : 94}");

static const std::string TH_CONFIG_BASE(
    "{'address' : 1,"
    "'calibration_factor' : 22.1,");

struct Thruster
{
    Thruster() :
        vehicle(new MockVehicle),
        ivehicle(vehicle),
        sensorBoard(new MockSensorBoard(
                        ram::core::ConfigNode::fromString(SB_CONFIG)))
    {
        vehicle->devices["SensorBoard"] =
            ram::vehicle::device::SensorBoardPtr(sensorBoard);
    }
    
    ~Thruster()
    {
        // no vehicle or sensorBoard cleanup becuase they use smart pointers
    }

    MockVehicle* vehicle;
    ram::vehicle::IVehiclePtr ivehicle;
    MockSensorBoard* sensorBoard;
    ram::vehicle::device::IThruster* thruster;
};

TEST_FIXTURE(Thruster, setForce)
{
    std::string config = TH_CONFIG_BASE +
        "'name' : 'StarboardThruster',"
        "'address' : 5,"
        "'direction' : 1}";

    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);

    
    CHECK_EQUAL(0, sensorBoard->thrusterValues[5]);
    thruster->setForce(2.5);
    CHECK(sensorBoard->thrusterValues[5] > 0);
    delete thruster;
    
    // Now with reverse direction
    config = TH_CONFIG_BASE +
        "'name' : 'StarboardThruster',"
        "'address' : 0,"
        "'direction' : -1}";

    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    
    CHECK_EQUAL(0, sensorBoard->thrusterValues[0]);
    thruster->setForce(2.5);
    CHECK(sensorBoard->thrusterValues[0] < 0);
    delete thruster;
}
    
TEST_FIXTURE(Thruster, getForce)
{
    std::string config = TH_CONFIG_BASE +
        "'name' : 'StarboardThruster',"
        "'address' : 5,"
        "'direction' : 1}";

    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);

    
    thruster->setForce(2.5);
    CHECK_EQUAL(2.5, thruster->getForce());
    delete thruster;
}

TEST_FIXTURE(Thruster, getMaxForce)
{
    // TODO: Test me when used
}

TEST_FIXTURE(Thruster, setMaxForce)
{
    // TODO: Test me when used
}

TEST_FIXTURE(Thruster, isEnabled)
{
    std::string config = TH_CONFIG_BASE +
        "'name' : 'StarboardThruster',"
        "'address' : 5}";

    // Default case
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    
    CHECK_EQUAL(false, thruster->isEnabled());

    // Make sure it uses the address
    sensorBoard->thrusterEnables[5] = true;
    CHECK_EQUAL(true, thruster->isEnabled());
    delete thruster;
}

TEST_FIXTURE(Thruster, setEnable)
{
    std::string config = TH_CONFIG_BASE +
        "'name' : 'StarboardThruster',"
        "'address' : 3}";

    // Default case
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);

    CHECK_EQUAL(false, sensorBoard->thrusterEnables[3]);
    thruster->setEnabled(true);
    CHECK_EQUAL(true, sensorBoard->thrusterEnables[3]);


    thruster->setEnabled(false);
    CHECK_EQUAL(false, sensorBoard->thrusterEnables[3]);
    delete thruster;
}

TEST_FIXTURE(Thruster, getOffset)
{
    std::string config = TH_CONFIG_BASE + "'name' : 'StarboardThruster'}";
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    CHECK_EQUAL(0.1905, thruster->getOffset());
    delete thruster;

    config = TH_CONFIG_BASE + "'name' : 'PortThruster'}";
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    CHECK_EQUAL(0.1905, thruster->getOffset());
    delete thruster;

    config = TH_CONFIG_BASE + "'name' : 'ForeThruster'}";
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    CHECK_EQUAL(0.3366, thruster->getOffset());
    delete thruster;

    config = TH_CONFIG_BASE + "'name' : 'AftThruster'}";
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    CHECK_EQUAL(0.3366, thruster->getOffset());
    delete thruster;

    config = TH_CONFIG_BASE + "'name' : 'TopThruster'}";
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    CHECK_EQUAL(0.193, thruster->getOffset());
    delete thruster;

    config = TH_CONFIG_BASE + "'name' : 'BottomThruster'}";
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);
    CHECK_EQUAL(0.193, thruster->getOffset());
    delete thruster;
}

TEST_FIXTURE(Thruster, getCurrent)
{
    std::string config = TH_CONFIG_BASE +
        "'name' : 'StarboardThruster',"
        "'address' : 3}";

    // Default case
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);

    // Check default
    CHECK_EQUAL(0, thruster->getCurrent());

    // Publish event and check values
    sensorBoard->publishThrusterUpdate(3, 6.5, false);
    CHECK_CLOSE(6.5, thruster->getCurrent(), 0.0001);
    delete thruster;
}

TEST_FIXTURE(Thruster, updateAddress)
{
    std::string config = TH_CONFIG_BASE +
        "'name' : 'StarboardThruster',"
        "'address' : 5}";

    // Default case
    thruster = new ram::vehicle::device::SBThruster(
        ram::core::ConfigNode::fromString(config), ram::core::EventHubPtr(),
        ivehicle);

    // Check default
    CHECK_EQUAL(0, thruster->getCurrent());

    // Publish event and check values
    sensorBoard->publishThrusterUpdate(4, 4.5, false);
    CHECK_CLOSE(0, thruster->getCurrent(), 0.0001);
    delete thruster;
}
