/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestTortuga.cxx
 */


// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
//#include "vehicle/include/Common.h"
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/device/SonarStateEstimator.h"
#include "vehicle/test/include/MockSonar.h"

#include "math/test/include/MathChecks.h"

using namespace ram;

static const std::string VEH_CONFIG("{'name' : 'TestVehicle'}");
static const std::string BASE_CONFIG("{'name' : 'SonarStateEstimator',");
static const std::string BLANK_CONFIG = BASE_CONFIG + "}";

SUITE(SonarStateEstimator) {

struct Fixture
{
    Fixture() :
        sonar(new MockSonar("Sonar")),
        estimator(0),
        rawVehicle(0),
        vehicle(vehicle::IVehiclePtr())
    {
        // Create the vehicle and wrap in a smart pointer
        rawVehicle = new vehicle::Vehicle(
            core::ConfigNode::fromString(VEH_CONFIG));
        vehicle = vehicle::IVehiclePtr(rawVehicle);

        // Add the sonar device to the vehicle
        rawVehicle->_addDevice(vehicle::device::IDevicePtr(sonar));
    }
    
    ~Fixture()
    {
        // No deletion needed handled by smart pointers
    }

    void createEstimator(std::string configStr)
    {
        estimator = new vehicle::device::SonarStateEstimator(
            core::ConfigNode::fromString(configStr),
            core::EventHubPtr(),
            vehicle);
        rawVehicle->_addDevice(vehicle::device::IDevicePtr(estimator));
    }

    void sendPingerDirection(math::Vector3 direction, unsigned char pingerID)
    {
        direction.normalise();
        sonar->publishUpdate(direction, pingerID);
    }
    
    MockSonar* sonar;
    vehicle::device::SonarStateEstimator* estimator;
    vehicle::Vehicle* rawVehicle;
    vehicle::IVehiclePtr vehicle;
};

TEST_FIXTURE(Fixture, getOrientation)
{
    createEstimator(BLANK_CONFIG);
    
    // Make sure orientation is just passed right through
    math::Quaternion orientation(1,2,3,4);
    estimator->orientationUpdate(orientation);
    CHECK_CLOSE(orientation, estimator->getOrientation(), 0.0001);
}

TEST_FIXTURE(Fixture, getDepth)
{
    createEstimator(BLANK_CONFIG);
    
    // Make sure depth is just passed right through
    double depth = 5.7;
    estimator->depthUpdate(depth);
    CHECK_CLOSE(depth, estimator->getDepth(), 0.0001);
}

TEST_FIXTURE(Fixture, getVelocity)
{
    createEstimator(BLANK_CONFIG);
    
    // Make sure velocity is just passed right through
    math::Vector2 velocity(1.23, 5.6);
    estimator->velocityUpdate(velocity);
    CHECK_CLOSE(velocity, estimator->getVelocity(), 0.0001);
}

TEST_FIXTURE(Fixture, getPositionBasic)
{
    // Create the state estimator with two pingers at (10,0) and (-10,0)
    // Initial vehicle depth, position, and velocity are 0, heading is North
    std::string config = BASE_CONFIG +
        "'pinger0Position' : [10, 0], 'pinger1Position' : [-10, 0] }";
    createEstimator(config);

    // Feed in a pair of pinger vectors which indicates the vehicle is at
    // position (0, -10)
    sendPingerDirection(math::Vector3(1, 1, 0.5), 0);
    sendPingerDirection(math::Vector3(-1, 1, 0.5), 0);

    // Now check the position
    math::Vector2 expectedPosition(0, 10);
    // TODO: Add control peoples code to really test this
//    CHECK_CLOSE(expectedPosition, estimator->getPosition(), 0.0001);
}

// TODO: Add more tests that take into account the vehicle pointed different
// directions

} // SUITE(SonarStateEstimator)
