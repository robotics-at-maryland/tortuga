/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestVisionVelocitySensor.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/device/VisionVelocitySensor.h"
#include "vehicle/test/include/MockVehicle.h"
#include "vehicle/test/include/MockDepthSensor.h"

#include "vision/include/Events.h"
#include "vision/include/VisionSystem.h"

#include "math/include/Events.h"
#include "math/test/include/MathChecks.h"

#include "core/include/EventHub.h"
#include "core/include/EventConnection.h"

using namespace ram;

static const std::string CONFIG("{"
    "'name' : 'VisionVelocitySensor',"
    "'bottomDepth' : 44.3700787," // 12 meters + 5 feet
"}");

struct VisionVelocitySensorFixture
{
    VisionVelocitySensorFixture() :
        vehicle(new MockVehicle()),
        depthSensor(new MockDepthSensor("SensorBoard")),
        ivehicle(vehicle),
        eventHub(new core::EventHub())
    {
        vehicle->devices["SensorBoard"] = 
            vehicle::device::IDevicePtr(depthSensor);
            
        visionVelocitySensor =
            new vehicle::device::VisionVelocitySensor(
                core::ConfigNode::fromString(CONFIG),
                eventHub, ivehicle);
    }
    
    ~VisionVelocitySensorFixture()
    {
        delete visionVelocitySensor;
    }

    void publishVisionVelocityEvent(math::Vector2 velocity,
                                    double timeStamp = -1)
    {
        math::Vector2EventPtr event(new math::Vector2Event());
        event->vector2 = velocity;
        if (timeStamp >= 0)
            event->timeStamp = timeStamp;
        eventHub->publish(vision::EventType::VELOCITY_UPDATE, event);
    }

    MockVehicle* vehicle;
    MockDepthSensor* depthSensor;                    
    vehicle::IVehiclePtr ivehicle;
    core::EventHubPtr eventHub;
    vehicle::device::IVelocitySensor* visionVelocitySensor;
};

TEST_FIXTURE(VisionVelocitySensorFixture, getVelocity)
{
    // Check default
    CHECK_EQUAL(math::Vector2::ZERO, visionVelocitySensor->getVelocity());
    
    // Set the depth, FOV, and pixels
    depthSensor->publishUpdate(5); // 12 meters difference
    vision::VisionSystem::_setDownVerticalFieldOfView(math::Degree(45));
    vision::VisionSystem::_setDownVerticalPixelResolution(480);
    vision::VisionSystem::_setDownHorizontalFieldOfView(math::Degree(60));
    vision::VisionSystem::_setDownHorizontalPixelResolution(640);

    // Put in the first event, make sure we still have zero
    math::Vector2 expectedVelocity(0, 0);
    publishVisionVelocityEvent(expectedVelocity, 0);

    // Now pulish another value and check the results
    math::Vector2 pixelVelocity(15, 30);
    expectedVelocity = math::Vector2(0.324*2.0, 0.621*2.0);
    publishVisionVelocityEvent(pixelVelocity, 0.5);
    
    CHECK_CLOSE(expectedVelocity, visionVelocitySensor->getVelocity(), 0.05);
}

typedef std::vector<math::Vector2EventPtr>
Vector2EventPtrList;

void velocityUpdateHelper(Vector2EventPtrList* list,
                       core::EventPtr event)
{
    list->push_back(
        boost::dynamic_pointer_cast<math::Vector2Event>(event));
}

TEST_FIXTURE(VisionVelocitySensorFixture, event_UPDATE)
{
    // Check default
    CHECK_EQUAL(math::Vector2::ZERO, visionVelocitySensor->getVelocity());

    // Register for the event
    Vector2EventPtrList eventList;
    core::EventConnectionPtr conn = visionVelocitySensor->subscribe(
        vehicle::device::IVelocitySensor::UPDATE,
        boost::bind(velocityUpdateHelper, &eventList, _1));
    
    // Publish event and make sure it didn't change
    math::Vector2 expectedVelocity(12.5, 90.2);
    publishVisionVelocityEvent(expectedVelocity);

    // Check results
    CHECK_EQUAL(1u, eventList.size());
    math::Vector2EventPtr event = eventList[0];
    CHECK_CLOSE(expectedVelocity, event->vector2, 0.0001);

    conn->disconnect();
}

