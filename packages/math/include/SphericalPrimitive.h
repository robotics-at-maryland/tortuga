/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/math/include/SphericalPrimitive.h
 */

#ifndef RAM_MATH_SPHERICALPRIMITIVE_H
#define RAM_MATH_SPHERICALPRIMITIVE_H

// STD Includes

// Library Includes

// Project Includes
#include "math/include/IPrimitive3D.h"

namespace ram {
namespace math {

class SphericalPrimitive : public IPrimitive3D
{
public:
    SphericalPrimitive(Vector3 center, double radius) :
        IPrimitive3D(),
        m_center(center),
        m_radius(radius),
        m_radiusSquared(radius * radius) {}

    virtual ~SphericalPrimitive() {}

    virtual double implicitFunctionValue(Vector3 p)
    {
        return m_center.squaredDistance(p) / m_radiusSquared;
    }

    Vector3 center() {return m_center;}
    double radius() {return m_radius;}

private:
    Vector3 m_center;
    double m_radius;
    double m_radiusSquared;
};

} // namespace math
} // namespace ram

#endif // RAM_MATH_SPHERICALPRIMITIVE_H
