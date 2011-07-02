/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/include/Particle3D.h
 */

#include "math/include/Vector3.h"
#include "math/include/Vector2.h"

#ifndef RAM_ESTIMATION_PARTICLE3D_H
#define RAM_ESTIMATION_PARTICLE3D_H

namespace ram {
namespace estimation {

struct Particle3D
{
    math::Vector3 location;
    math::Vector2 imgCoords;
    float likelihood;
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_PARTICLE3D_H
