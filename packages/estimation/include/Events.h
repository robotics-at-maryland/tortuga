/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/include/Events.h
 */

#ifndef RAM_ESTIMATION_EVENTS_H
#define RAM_ESTIMATION_EVENTS_H

// STD Includes

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "estimation/include/Obstacle.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"
#include "core/include/Event.h"

namespace ram {
namespace estimation {

struct ObstacleEvent : public core::Event
{
    Obstacle::ObstacleType obstacle;
    math::Vector3 location;
    math::Matrix3 covariance;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<ObstacleEvent> ObstacleEventPtr;

} // namespace estimation
} // namespace ram


#endif // RAM_ESTIMATION_EVENTS_H
