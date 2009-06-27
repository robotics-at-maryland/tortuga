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

#include "vision/include/Events.h"

#include "math/include/Events.h"
#include "math/test/include/MathChecks.h"

#include "core/include/EventHub.h"
#include "core/include/EventConnection.h"

static const std::string CONFIG("{'name' : 'VisionVelocitySensor'}");

struct VisionVelocitySensorFixture
{
    VisionVelocitySensorFixture() :
        eventHub(new ram::core::EventHub()),
        visionVelocitySensor(new ram::vehicle::device::VisionVelocitySensor(
                                 ram::core::ConfigNode::fromString(CONFIG),
                                 eventHub, ram::vehicle::IVehiclePtr()))
    {
    }
    
    ~VisionVelocitySensorFixture()
    {
        delete visionVelocitySensor;
    }

    void publishVisionVelocityEvent(ram::math::Vector2 velocity)
    {
        ram::math::Vector2EventPtr event(new ram::math::Vector2Event());
        event->vector2 = velocity;
        eventHub->publish(ram::vision::EventType::VELOCITY_UPDATE, event);
    }
    
    ram::core::EventHubPtr eventHub;
    ram::vehicle::device::IVelocitySensor* visionVelocitySensor;
};

TEST_FIXTURE(VisionVelocitySensorFixture, getVelocity)
{
    // Check default
    CHECK_EQUAL(ram::math::Vector2::ZERO, visionVelocitySensor->getVelocity());

    // Publish event and check values
    ram::math::Vector2 expectedVelocity(362.2, 12.5);
    publishVisionVelocityEvent(expectedVelocity);

    CHECK_EQUAL(expectedVelocity, visionVelocitySensor->getVelocity());
}

typedef std::vector<ram::math::Vector2EventPtr>
Vector2EventPtrList;

void velocityUpdateHelper(Vector2EventPtrList* list,
                       ram::core::EventPtr event)
{
    list->push_back(
        boost::dynamic_pointer_cast<ram::math::Vector2Event>(event));
}

TEST_FIXTURE(VisionVelocitySensorFixture, event_UPDATE)
{
    // Check default
    CHECK_EQUAL(ram::math::Vector2::ZERO, visionVelocitySensor->getVelocity());

    // Register for the event
    Vector2EventPtrList eventList;
    ram::core::EventConnectionPtr conn = visionVelocitySensor->subscribe(
        ram::vehicle::device::IVelocitySensor::UPDATE,
        boost::bind(velocityUpdateHelper, &eventList, _1));
    
    // Publish event and make sure it didn't change
    ram::math::Vector2 expectedVelocity(12.5, 90.2);
    publishVisionVelocityEvent(expectedVelocity);

    // Check results
    CHECK_EQUAL(1u, eventList.size());
    ram::math::Vector2EventPtr event = eventList[0];
    CHECK_CLOSE(expectedVelocity, event->vector2, 0.0001);

    conn->disconnect();
}

