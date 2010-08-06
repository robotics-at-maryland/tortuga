/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/Obstacle.h
 */

/* Obstacle represents a course obstacle and contains all data that would be
** stored for an obstacle.
*/

#ifndef RAM_VEHICLE_ESTIMATOR_OBSTACLE_H
#define RAM_VEHICLE_ESTIMATOR_OBSTACLE_H


// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "math/include/Vector2.h"


namespace ram {
namespace estimator {

class Obstacle;
typedef boost::shared_ptr<Obstacle> ObstaclePtr;

class Obstacle
{
public:
    virtual ~Obstacle(){};

    math::Vector2 getObstaclePosition();
    double getObstacleDepth();
    
    void setObstaclePosition(math::Vector2 position);
    void setObstacleDepth(double depth);

private:
    double depth;
    math::Vector2 position;
    // possibly also store color or other properties that might need accessed
};



} // namespace estimator
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_OBSTACLE_H
