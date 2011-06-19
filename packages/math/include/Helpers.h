/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Gland <jgland@ssl.umd.edu>
 * File:  packages/math/include/Helpers.h
 */

#ifndef RAM_MATH_H_07_07_2007
#define RAM_MATH_H_07_07_2007

// Project Includes
#include "math/include/Matrix2.h"

// Must Be Included last
#include "math/include/Export.h"
#include "math/include/Matrix2.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace math {

int sign(double variable);

// factorial for smallish numbers
int factorial(int num);


math::Vector2 world2img(math::Vector3 worldCoordinates,
                        math::Vector3 cameraPosition,
                        math::Quaternion cameraOrientation,
                        math::Matrix3 intrinsicParameters);

math::Vector3 img2world(math::Vector2 imgCoordinates,
                        double distance,
                        math::Vector3 cameraPosition,
                        math::Quaternion cameraOrientation,
                        math::Matrix3 invIntrinsicParameters);

} // namespace math
} // namespace ram
    
#endif // RAM_MATH_H_07_07_2007
