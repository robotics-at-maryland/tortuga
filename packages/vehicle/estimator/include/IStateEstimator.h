/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/IStateEstimator.h
 */

/* IStateEstimator is the public interface to a state estimator.  All
** state data needed by other systems shoudl be accessed through this 
** interface.
*/

#ifndef RAM_VEHICLE_ESTIMATOR_ISTATEESTIMATOR_H
#define RAM_VEHICLE_ESTIMATOR_ISTATEESTIMATOR_H

// Library Includes
#include <string>
#include <boost/shared_ptr.hpp>

// Project Includes
#include "vehicle/include/Common.h"
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "vehicle/estimator/include/Obstacle.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace estimator {

class IStateEstimator
{
public:
    virtual ~IStateEstimator(){};

    /* Estimated quantities will be returned in the Inertial frame if applicable */

    virtual math::Vector2 getEstimatedPosition() = 0;
    virtual math::Vector2 getEstimatedVelocity() = 0;
    virtual math::Vector3 getEstimatedLinearAcceleration() = 0;
    virtual math::Vector3 getEstimatedAngularRate() = 0;
    virtual math::Quaternion getEstimatedOrientation() = 0;
    virtual double getEstimatedDepth() = 0;
    virtual double getEstimatedDepthDot() = 0;
    
    /* Implementations of IStateEstimator should store the information about course
       obstacles.  These functions allow interaction with each obstacle. */
    virtual void addObstacle(std::string name, ObstaclePtr obstacle) = 0;
    virtual math::Vector2 getObstaclePosition(std::string name) = 0;
    virtual double getObstacleDepth(std::string name) = 0;

    static const core::Event::EventType ESTIMATED_DEPTH_UPDATE;
    static const core::Event::EventType ESTIMATED_ORIENTATION_UPDATE;
    static const core::Event::EventType ESTIMATED_VELOCITY_UPDATE;
    static const core::Event::EventType ESTIMATED_POSITION_UPDATE;
    static const core::Event::EventType ESTIMATED_LINEARACCELERATION_UPDATE;
    static const core::Event::EventType ESTIMATED_DEPTHDOT_UPDATE;
    static const core::Event::EventType ESTIMATED_ANGULARRATE_UPDATE;

protected:
    IStateEstimator(){};
};


} // namespace estimator
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_ISTATEESTIMATOR_H
