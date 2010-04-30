/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/EstiamtedState.h
 */

/* EstiamtedState stores all state data and provides thread safe methods 
** to set and get an estimated state quantity. This class should only be
** accessed directly from within the estimator framework because the
** IStateEstimator provides a public interface to get needed estimates.
*/

#ifndef RAM_VEHICLE_ESTIMATOR_ESTIMATEDSTATE_H
#define RAM_VEHICLE_ESTIMATOR_ESTIMATEDSTATE_H

// Library Includes
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/ReadWriteMutex.h"
#include "vehicle/estimator/include/Obstacle.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace estimator {

class EstimatedState;
typedef boost::shared_ptr<EstimatedState> EstimatedStatePtr;

class EstimatedState
{
public:

    EstimatedState();
    virtual ~EstimatedState(){};

    math::Vector2 getEstimatedPosition();
    math::Vector2 getEstimatedVelocity();
    math::Vector3 getEstimatedLinearAcceleration();
    math::Vector3 getEstimatedAngularRate();
    math::Quaternion getEstimatedOrientation();
    double getEstimatedDepth();
    double getEstimatedDepthDot();

    void setEstimatedPosition(math::Vector2 position);
    void setEstimatedVelocity(math::Vector2 velocity);
    void setEstimatedLinearAcceleration(math::Vector3 linearAcceleration);
    void setEstimatedAngularRate(math::Vector3 angularRate);
    void setEstimatedOrientation(math::Quaternion orientation);
    void setEstimatedDepth(double depth);
    void setEstimatedDepthDot(double depthDot);

    void addObstacle(std::string name, ObstaclePtr obstacle);
    math::Vector2 getObstaclePosition(std::string name);
    double getObstacleDepth(std::string name);


private:

    core::ReadWriteMutex m_stateMutex;

    math::Vector2 estimatedPosition;
    math::Vector2 estimatedVelocity;
    math::Vector3 estimatedLinearAcceleration;
    math::Vector3 estimatedAngularRate;
    math::Quaternion estimatedOrientation;
    double estimatedDepth;
    double estimatedDepthDot;

    std::map<std::string, ObstaclePtr> obstacleMap;

};

} // namespace estimator
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_ESTIMATEDSTATE_H
