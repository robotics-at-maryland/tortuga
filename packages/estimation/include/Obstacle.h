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
#include "math/include/Matrix2.h"
#include "math/include/Matrix3.h"
#include "math/include/Quaternion.h"

#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace estimation {


class Obstacle;
typedef boost::shared_ptr<Obstacle> ObstaclePtr;

class Obstacle
{
public:

    enum ObstacleType {
        RED_BUOY, 
        YELLOW_BUOY, 
        GREEN_BUOY, 
        LOVERS_LANE,
        LARGE_HEART,
        SMALL_HEART,
        LARGE_X,
        SMALL_X,
        LARGE_O,
        SMALL_O
    };

    Obstacle();
    virtual ~Obstacle();

    math::Vector3 getLocation();
    math::Matrix3 getLocationCovariance();
    math::Quaternion getAttackOrientation();
    
    void setLocation(math::Vector3 location);
    void setLocationCovariance(math::Matrix3 covariance);
    void setAttackOrientation(math::Quaternion orientation);



private:
    math::Vector3 m_location;
    math::Matrix3 m_locationCovariance;
    math::Quaternion m_attackOrientation;

    core::ReadWriteMutex m_stateMutex;
};



} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_OBSTACLE_H
