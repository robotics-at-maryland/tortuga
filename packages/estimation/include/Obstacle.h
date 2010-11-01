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
    /**
     * Base constructor for an obstacle.
     * Obstacles can track depth, position, velocity, and orientation.
     *
     * A specific obstacle is not guaranteed to track all of these.
     * For example, in most cases, tracking velocity makes no sense.
     *
     * Use a bitmask when creating an obstacle to say what the obstacle
     * is going to be used to track. Whenever somebody tries to use a getter
     * on an untracked attribute, this will throw an exception. Anytime a
     * setter is called, the obstacle will change to tracking that attribute.
     * Because of this behavior, Obstacles default to tracking no attributes.
     *
     * This will throw an exception:
     *     obstacle = Obstacle();
     *     obstacle.getPosition();
     *
     * This is fine:
     *     obstacle = Obstacle();
     *     obstacle.setPosition(math::Vector2(0, 0));
     *     obstacle.getPosition();
     *
     * This is also fine (but not recommended):
     *     obstacle = Obstacle(Obstacle::POSITION);
     *     obstacle.getPosition();
     *
     * Values contained are dependent on the specific state estimator's
     * use of the class.
     */
    Obstacle(int attributes = NONE);
    virtual ~Obstacle();

    double getDepth();
    math::Vector2 getPosition();
    math::Vector3 getVelocity();
    math::Quaternion getOrientation();
    
    void setDepth(double depth);
    void setPosition(math::Vector2 position);
    void setVelocity(math::Vector3 velocity);
    void setOrientation(math::Quaternion orientation);

    static const int NONE;
    static const int DEPTH;
    static const int POSITION;
    static const int VELOCITY;
    static const int ORIENTATION;

private:
    /* Includes depth and position */
    math::Vector3 m_position;
    math::Vector3 m_velocity;
    math::Quaternion m_orientation;

    int m_attribs;
    // possibly also store color or other properties that might need accessed
};



} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_OBSTACLE_H
