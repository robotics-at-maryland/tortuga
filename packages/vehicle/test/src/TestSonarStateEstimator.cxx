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

class TestSonarEstimator : public vehicle::device::SonarStateEstimator
{
public:
    TestSonarEstimator(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr(),
                       vehicle::IVehiclePtr vehicle = vehicle::IVehiclePtr()) :
        vehicle::device::SonarStateEstimator(config, eventHub, vehicle)
    {}

    double deltaT;
protected:
    virtual double getDeltaT() { return deltaT; }
};

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
        estimator = new TestSonarEstimator(
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
    TestSonarEstimator* estimator;
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
    // TODO: This will break when velocity estimation is done, must update the 
    // test code
    //createEstimator(BLANK_CONFIG);
    
    // Make sure velocity is just passed right through
    //math::Vector2 velocity(1.23, 5.6);
    //estimator->velocityUpdate(velocity);
    //CHECK_CLOSE(velocity, estimator->getVelocity(), 0.0001);
}

TEST_FIXTURE(Fixture, createMeasurementModel)
{
    math::MatrixN result(2, 8); // 2x8 matrix
    double rawXHat[] = {0, 0, 0, 0, 0, 0, 0, 0};
    math::VectorN xHat(rawXHat, 8); // 8 dim vector filled with 0's
    
    // The expected result
    double rawExpectedResult[] =
        {0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0};
    math::MatrixN expectedResult(rawExpectedResult, 2, 8);

    // Run the function and check the result
    vehicle::device::SonarStateEstimator::createMeasurementModel(xHat, result);
    CHECK_CLOSE(expectedResult, result, 0.0001);
}

TEST_FIXTURE(Fixture, findAbsPingerAngle)
{
    // Vehicle 30 degrees to the left, pinger 45 to the left relative
    math::Quaternion orientation(math::Degree(30), math::Vector3::UNIT_Z);
    math::Vector3 pingerVector(1, 1, 0);
    pingerVector.normalise();
  
    // We expected it to be 75 degrees to the left of north
    math::Degree expectedPingerAngle(math::Degree(75));

    // Get the result and check
    math::Degree pingerAngle(
        vehicle::device::SonarStateEstimator::findAbsPingerAngle(orientation,
                                                                 pingerVector));
    //CHECK_CLOSE(expectedPingerAngle.valueDegrees(), 
    //            pingerAngle.valueDegrees(), 0.0001);
}


TEST_FIXTURE(Fixture, getPositionBasic)
{
    // Create the state estimator with two pingers at (10,0) and (-10,0)
    // Initial vehicle depth, position, and velocity are 0, heading is North
    std::string config = BASE_CONFIG +
        "'pingerLeftPosition' : [10, 0], 'pingerRightPosition' : [-10, 0],"
        "'dragDensity': 1.0 }";
    createEstimator(config);

    // Feed in the update for the right pinger with a DT of 1.5 seconds
    // and rotate 45 degrees to the left (ie. pointing N-NW)
    estimator->deltaT = 1.5;
    estimator->orientationUpdate(math::Quaternion(math::Degree(45),
                                                  math::Vector3::UNIT_Z));
    sendPingerDirection(math::Vector3(1, 1, 0.5), 1); // Right pinger update

/*
    CHECK_CLOSE(math::Vector2(0, 10), estimator->getPosition(), 0.0001);
    CHECK_CLOSE(math::Vector2(0, 0), estimator->getVelocity(), 0.0001);
    CHECK_CLOSE(math::Vector2(10, 0), 
                estimator->getLeftPingerEstimatedPosition(), 0.0001);
    CHECK_CLOSE(math::Vector2(-10, 0), 
                estimator->getRightPingerEstimatedPosition(), 0.0001);
*/
    // Feed in the update for the left pinger with a DT of 0.75 seconds
    estimator->deltaT = 0.75;
    estimator->orientationUpdate(math::Quaternion(math::Degree(45),
                                                  math::Vector3::UNIT_Z));    
    sendPingerDirection(math::Vector3(-1, 1, 0.5), 0); // Left pinger update
/*
    CHECK_CLOSE(math::Vector2(0, 10), estimator->getPosition(), 0.0001);
    CHECK_CLOSE(math::Vector2(0, 0), estimator->getVelocity(), 0.0001);
    CHECK_CLOSE(math::Vector2(10, 0), 
                estimator->getLeftPingerEstimatedPosition(), 0.0001);
    CHECK_CLOSE(math::Vector2(-10, 0), 
                estimator->getRightPingerEstimatedPosition(), 0.0001);
*/
}

// TODO: Add more tests that take into account the vehicle pointed different
// directions

} // SUITE(SonarStateEstimator)
