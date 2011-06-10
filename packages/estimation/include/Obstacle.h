/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/Obstacle.h
 */

/* Obstacle represents a course obstacle and contains all data that would be
** stored for an obstacle.
*/

#ifndef RAM_ESTIMATION_OBSTACLE_H
#define RAM_ESTIMATION_OBSTACLE_H

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace estimation {


class Obstacle;
typedef boost::shared_ptr<Obstacle> ObstaclePtr;

class Obstacle
{
public:

    enum ObstacleType {
        RED_BUOY, YELLOW_BUOY, GREEN_BUOY, LOVERS_LANE,RED_TARGET,GREEN_TARGET,
        BIN1, BIN2, BIN3, BIN4
    };

    Obstacle();
    virtual ~Obstacle();

    double getDepth();
    math::Vector2 getPosition();
    math::Quaternion getAttackOrientation();
    
    void setDepth(double depth);
    void setPosition(math::Vector2 position);
    void setAttackOrientation(math::Quaternion orientation);



private:
    /* Includes depth and position */
    math::Vector3 m_position;
    math::Quaternion m_attackOrientation;

    // possibly also store color or other properties that might need accessed
};



} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_OBSTACLE_H
