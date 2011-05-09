/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/math/include/Primitive.h
 */

#ifndef RAM_MATH_IPRIMITIVE3D_H
#define RAM_MATH_IPRIMITIVE3D_H

// STD Includes

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "math/include/Vector3.h"

namespace ram {
namespace math {

class IPrimitive3D;
typedef boost::shared_ptr<IPrimitive3D> IPrimitive3DPtr;

class IPrimitive3D
{
public:
    IPrimitive3D() {}
    virtual ~IPrimitive3D() {}

    // compute the value of the implicit function which characterizes
    // where the point is with respect to the primitives implicit surface
    //     when the result is less than some constant c, the point lies
    //         within the implicit primitive surface
    //     when the result is greater than some constant c, the point lies
    //         outside the implicit primitive surface
    //     when the result is equal to some constant c, the point lies on
    //         the implicit primitive surface
    virtual double implicitFunctionValue(Vector3 p) = 0;
};

} // namespace math
} // namespace ram

#endif // RAM_MATH_IPRIMITIVE
