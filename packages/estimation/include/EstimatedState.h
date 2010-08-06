/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/EstiamtedState.h
 */

/* EstiamtedState stores all state data and provides thread safe methods 
** to set and get an estimated state quantity. This class should only be
** accessed directly from within the estimator framework because the
** IStateEstimator provides a public interface to get needed estimates.
*/

#ifndef RAM_ESTIMATION_ESTIMATEDSTATE_H
#define RAM_ESTIMATION_ESTIMATEDSTATE_H

// Library Includes
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

// Project Includes
#include "estimation/include/IStateEstimator.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/EventPublisher.h"
#include "estimation/include/Obstacle.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace estimation {

class EstimatedState;
typedef boost::shared_ptr<EstimatedState> EstimatedStatePtr;

class EstimatedState : public core::EventPublisher
{
public:

    EstimatedState(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    virtual ~EstimatedState(){};

    math::Vector2 getEstPosition();
    math::Vector2 getEstVelocity();
    math::Vector3 getEstLinearAccel();
    math::Vector3 getEstAngularRate();
    math::Quaternion getEstOrientation();
    double getEstDepth();
    double getEstDepthDot();
    math::Vector3 getEstThrusterForces();
    math::Vector3 getEstThrusterTorques();

    void setEstPosition(math::Vector2 position);
    void setEstVelocity(math::Vector2 velocity);
    void setEstLinearAccel(math::Vector3 linearAccel);
    void setEstAngularRate(math::Vector3 angularRate);
    void setEstOrientation(math::Quaternion orientation);
    void setEstDepth(double depth);
    void setEstDepthDot(double depthDot);
    void setEstThrust(math::Vector3 forces, math::Vector3 torques);


    /* The estimated state will contain all information about obstacles in a
       mapping from obstacle name to a pointer to that obstacle.  These functions
       allow access to that information */
    void addObstacle(std::string name, ObstaclePtr obstacle);
    math::Vector2 getObstaclePosition(std::string name);
    double getObstacleDepth(std::string name);


private:

    void publishPositionUpdate(const math::Vector2& position);
    void publishVelocityUpdate(const math::Vector2& velocity);
    void publishLinearAccelUpdate(const math::Vector3& linearAccel);
    void publishAngularRateUpdate(const math::Vector3& angularRate);
    void publishOrientationUpdate(const math::Quaternion& orientation);
    void publishDepthUpdate(const double& depth);
    void publishDepthDotUpdate(const double& depthDot);
    void publishThrustUpdate(const math::Vector3& forces,
                             const math::Vector3& torques);

    core::ReadWriteMutex m_stateMutex;

    math::Vector2 estPosition;
    math::Vector2 estVelocity;
    math::Vector3 estLinearAccel;
    math::Vector3 estAngularRate;
    math::Quaternion estOrientation;
    double estDepth;
    double estDepthDot;
    math::Vector3 estThrusterForces;
    math::Vector3 estThrusterTorques;

    std::map<std::string, ObstaclePtr> obstacleMap;

};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_ESTIMATEDSTATE_H
