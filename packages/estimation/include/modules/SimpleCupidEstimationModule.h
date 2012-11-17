/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/src/modules/ParticleCupidEstimationModule.h
 */


#ifndef RAM_ESTIMATION_SIMPLECUPIDESTIMATIONMODULE_H
#define RAM_ESTIMATION_SIMPLECUPIDESTIMATIONMODULE_H

// STD Includes
#include <vector>
#include <deque>

// Library Includes
// #include <boost/math/distributions/normal.hpp>

// Project Includes
#include "estimation/include/EstimationModule.h"
#include "estimation/include/EstimatedState.h"
#include "estimation/include/Obstacle.h"
#include "estimation/include/Particle3D.h"

#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "core/include/ReadWriteMutex.h"

#include "math/include/Math.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"
#include "math/include/Matrix3.h"
#include "math/include/Quaternion.h"
#include "math/include/Events.h"

#include "vision/include/VisionSystem.h"

namespace ram {
namespace estimation {

class SimpleCupidEstimationModule : public EstimationModule
{
public:
    SimpleCupidEstimationModule(core::ConfigNode config,
                               core::EventHubPtr eventHub,
                               EstimatedStatePtr estState,
                               Obstacle::ObstacleType obstacle,
                               core::Event::EventType inputEventType);

    virtual ~SimpleCupidEstimationModule() {};

    virtual void update(core::EventPtr event);

private:
    
};


} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_SIMPLECUPIDESTIMATIONMODULE_H
