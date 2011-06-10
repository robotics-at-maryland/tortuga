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

Obstacle::Obstacle()
    : m_position(math::Vector3::ZERO),
      m_attackOrientation(math::Quaternion::IDENTITY)
{
}

Obstacle::~Obstacle()
{
}

double Obstacle::getDepth()
{
    return m_position.z;
}

math::Vector2 Obstacle::getPosition()
{
    return math::Vector2(m_position.x, m_position.y);
}

math::Quaternion Obstacle::getAttackOrientation()
{
    return m_attackOrientation;
}

void Obstacle::setDepth(double depth)
{
    m_position.z = depth;
}

void Obstacle::setPosition(math::Vector2 position)
{
    m_position.x = position.x;
    m_position.y = position.y;
}

void Obstacle::setAttackOrientation(math::Quaternion orientation)
{
    m_attackOrientation = orientation;
}

} // namespace estimation
} // namespace ram
