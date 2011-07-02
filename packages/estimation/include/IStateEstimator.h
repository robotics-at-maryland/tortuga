/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/IStateEstimator.h
 */

/* IStateEstimator is the public interface to a state estimator.  All
** state data needed by other systems shoudl be accessed through this 
** interface.
*/

#ifndef RAM_ESTIMATION_ISTATEESTIMATOR_H
#define RAM_ESTIMATION_ISTATEESTIMATOR_H

// STD Includes
#include <string>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "estimation/include/Common.h"
#include "estimation/include/Obstacle.h"

#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/Subsystem.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace estimation {

class IStateEstimator : public core::Subsystem
{
public:

    virtual ~IStateEstimator();

    /* Estimated quantities will be returned in the Inertial frame if applicable */

    /** returns the estimated position in the inertial frame */
    virtual math::Vector2 getEstimatedPosition() = 0;

    /** returns the estimated velocity in the inertial frame */
    virtual math::Vector2 getEstimatedVelocity() = 0;

    /** returns the estimated linear acceleration in the inertial frame */
    virtual math::Vector3 getEstimatedLinearAcceleration() = 0;

    /** returns the estimated angular rate */
    virtual math::Vector3 getEstimatedAngularRate() = 0;

    /** returns the estimated orientation */
    virtual math::Quaternion getEstimatedOrientation() = 0;

    /** returns the estimated depth */
    virtual double getEstimatedDepth() = 0;

    /** returns the estiamted depth change rate */
    virtual double getEstimatedDepthRate() = 0;

    /** return the estimated vehicle mass */
    virtual double getEstimatedMass() = 0;

    /** return the estimated thruster forces */
    virtual math::Vector3 getEstimatedThrusterForces() = 0;

    /** return the estimated thruster torques */
    virtual math::Vector3 getEstimatedThrusterTorques() = 0;
    
    /* Implementations of IStateEstimator should store the information about course
       obstacles.  These functions allow interaction with each obstacle. */
    virtual void addObstacle(Obstacle::ObstacleType name, ObstaclePtr obstacle) = 0;
    virtual math::Vector3 getObstacleLocation(Obstacle::ObstacleType name) = 0;
    virtual math::Matrix3 getObstacleLocationCovariance(Obstacle::ObstacleType name) = 0;
    virtual math::Quaternion getObstacleAttackOrientation(Obstacle::ObstacleType name) = 0;


    static const core::Event::EventType ESTIMATED_DEPTH_UPDATE;
    static const core::Event::EventType ESTIMATED_ORIENTATION_UPDATE;
    static const core::Event::EventType ESTIMATED_VELOCITY_UPDATE;
    static const core::Event::EventType ESTIMATED_POSITION_UPDATE;
    static const core::Event::EventType ESTIMATED_LINEARACCELERATION_UPDATE;
    static const core::Event::EventType ESTIMATED_DEPTHRATE_UPDATE;
    static const core::Event::EventType ESTIMATED_ANGULARRATE_UPDATE;
    static const core::Event::EventType ESTIMATED_FORCES_UPDATE;
    static const core::Event::EventType ESTIMATED_TORQUES_UPDATE;
    static const core::Event::EventType ESTIMATED_OBSTACLE_UPDATE;

    // static const core::Event::EventType ESTIMATED_GREEN_BUOY_UPDATE;
    // static const core::Event::EventType ESTIMATED_RED_BUOY_UPDATE;
    // static const core::Event::EventType ESTIMATED_YELLOW_BUOY_UPDATE;
    // static const core::Event::EventType ESTIMATED_WINDOW_UPDATE;
    // static const core::Event::EventType ESTIMATED_LOVERSLANE_UPDATE;
    // static const core::Event::EventType ESTIMATED_PIPE_UPDATE;
    // static const core::Event::EventType ESTIMATED_SONARPIPE_UPDATE;

protected:
    IStateEstimator(std::string name,
                    core::EventHubPtr eventHub = core::EventHubPtr());
};


} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_ISTATEESTIMATOR_H
