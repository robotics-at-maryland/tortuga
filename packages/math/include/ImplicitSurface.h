/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/math/include/ImplicitSurface.h
 */

#ifndef RAM_MATH_IMPLICITSURFACE_H
#define RAM_MATH_IMPLICITSURFACE_H

// STD Includes
#include <vector>
#include <cmath>

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "math/include/IPrimitive3D.h"

namespace ram {
namespace math {

// An 3D implicit surface is a 2D function defined implicity by a level
// curve such as f(x,y,z) = c where the function f(x,y,z) < c inside
// the surface, f(x,y,z) > c outside the surface.

// See Accurate Color Classification and Segmentation for Mobile Robots
// Authors: Alvarez, Millán, Aceves-López, and Swain-Oropeza

class ImplicitSurface
{
public:
    ImplicitSurface(std::vector<IPrimitive3DPtr> primitives,
                    float blendingFactor) :
        m_primitives(primitives),
        m_blendingFactor(blendingFactor) {}

    virtual inline float implicitFunctionValue(Vector3 p)
    {
        float invSum = 0;
        if(fabs(m_blendingFactor - 1.0)  < 0.0001)
        {
            // we need to blend all of the primitives
            BOOST_FOREACH(IPrimitive3DPtr it, m_primitives)
            {
                invSum += 1 / it->implicitFunctionValue(p);
            }
            return 1 / invSum;
        }
        else
        {
            // we need to blend all of the primitives
            BOOST_FOREACH(IPrimitive3DPtr it, m_primitives)
            {
                invSum += 1 / pow(it->implicitFunctionValue(p),
                                  m_blendingFactor);
            }
        
            return pow(invSum, (-1.0 / m_blendingFactor));
        }
    }

private:
    std::vector<IPrimitive3DPtr> m_primitives;
    float m_blendingFactor;
};

} // namespace math
} // namespace ram

#endif // RAM_MATH_IMPLICITSURFACE_H
