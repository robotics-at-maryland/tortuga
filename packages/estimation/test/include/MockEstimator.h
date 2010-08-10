/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/test/include/MockEstimator.h
 */

#ifndef RAM_ESTIMATION_MOCKESTIMATOR_H
#define RAM_ESTIMATION_MOCKESTIMATOR_H

// STD Includes
#include <assert.h>

// Library Includes

// Project Includes
#include "estimation/include/IStateEstimator.h"

class MockEstimator : public ram::estimation::IStateEstimator
{
public:
    MockEstimator() :
        IStateEstimator(),
        estPosition(ram::math::Vector2::ZERO),
        estVelocity(ram::math::Vector2::ZERO),
        estLinearAccel(ram::math::Vector3::ZERO),
        estAngularRate(ram::math::Vector3::ZERO),
        estOrientation(ram::math::Quaternion::IDENTITY),
        estDepth(0),
        estDepthDot(0)
    {}

    virtual ram::math::Vector2 getEstimatedPosition() 
    {return estPosition;}
    virtual ram::math::Vector2 getEstimatedVelocity() 
    {return estVelocity;}
    virtual ram::math::Vector3 getEstimatedLinearAcceleration() 
    {return estLinearAccel;}
    virtual ram::math::Vector3 getEstimatedAngularRate() 
    {return estAngularRate;}
    virtual ram::math::Quaternion getEstimatedOrientation()
    {return estOrientation;}
    virtual double getEstimatedDepth() 
    {return estDepth;}
    virtual double getEstimatedDepthDot()
    {return estDepthDot;}
    
    /* Implementations of IStateEstimator should store the information about course
       obstacles.  These functions allow interaction with each obstacle. */
    virtual void addObstacle(std::string name,
                             ram::estimation::ObstaclePtr obstacle) {}
    virtual ram::math::Vector2 getObstaclePosition(std::string name)
    {return ram::math::Vector2::ZERO;}
    virtual double getObstacleDepth(std::string name) 
    {return 0;}

    /* Make it seem like the mock estimator is actually accessing an 
     * estimated state but just make it all local and public so
     * tests can easily access everything. */

    ram::math::Vector2 estPosition;
    ram::math::Vector2 estVelocity;
    ram::math::Vector3 estLinearAccel;
    ram::math::Vector3 estAngularRate;
    ram::math::Quaternion estOrientation;
    double estDepth;
    double estDepthDot;
};


#endif // RAM_ESTIMATION_MOCKESTIMATOR_H
