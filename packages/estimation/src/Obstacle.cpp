/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/estimation/src/Obstacle.cpp
 */

// STD Includes
#include <stdexcept>

// Project Includes
#include "estimation/include/Obstacle.h"

#define THROW_ERROR( obj ) \
    throw std::runtime_error(# obj " is not a valid attribute for this obstacle")

namespace ram {
namespace estimation {

const int Obstacle::NONE = 0x0;
const int Obstacle::DEPTH = 0x1;
const int Obstacle::POSITION = 0x2;
const int Obstacle::VELOCITY = 0x4;
const int Obstacle::ORIENTATION = 0x8;

Obstacle::Obstacle(int attributes)
    : m_position(math::Vector3::ZERO)
    , m_velocity(math::Vector3::ZERO)
    , m_orientation(math::Quaternion::IDENTITY)
    , m_attribs(attributes)
{
}

Obstacle::~Obstacle()
{
}

double Obstacle::getDepth()
{
    if ( DEPTH & m_attribs )
        return m_position.z;
    else
        THROW_ERROR( Depth );
}

math::Vector2 Obstacle::getPosition()
{
    if ( POSITION & m_attribs )
        return math::Vector2(m_position.x, m_position.y);
    else
        THROW_ERROR( Position );
}

math::Vector3 Obstacle::getVelocity()
{
    if ( VELOCITY & m_attribs )
        return m_velocity;
    else
        THROW_ERROR( Velocity );
}

math::Quaternion Obstacle::getOrientation()
{
    if ( ORIENTATION & m_attribs )
        return m_orientation;
    else
        THROW_ERROR( Orientation );
}

void Obstacle::setDepth(double depth)
{
    m_attribs |= DEPTH;
    m_position.z = depth;
}

void Obstacle::setPosition(math::Vector2 position)
{
    m_attribs |= POSITION;
    m_position.x = position.x;
    m_position.y = position.y;
}

void Obstacle::setVelocity(math::Vector3 velocity)
{
    m_attribs |= VELOCITY;
    m_velocity = velocity;
}

void Obstacle::setOrientation(math::Quaternion orientation)
{
    m_attribs |= ORIENTATION;
    m_orientation = orientation;
}

} // namespace estimation
} // namespace ram
