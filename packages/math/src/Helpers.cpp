/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Gland <jgland@ssl.umd.edu>
 * File:  packages/math/src/Helpers.cpp
 */
 
// STD Includes
#include <cmath>

// Project Includes
#include "core/include/Platform.h"
#include "math/include/Helpers.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"
#include "math/include/Matrix2.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace math {

/*
 * the sign function
 *
 * returns 1 if variable is positive
 * returns -1 if variable is negative
 * returns 0 if variale is zero
 */
int sign(double x)
{
    return (x > 0) - (x < 0);
}

int factorial(int num)
{
    // dont use this function for big numbers
    int ans = 1;

    while(num > 1){
        ans *= num;
        num--;
    }

    return ans;
}

Vector3 world2img(Vector3 coords_w,
                  Vector3 cameraPosition,
                  Quaternion cameraOrientation,
                  Matrix3 intrinsicParameters)
{
    // compute vector from camera to object and rotate to camera frame
    Vector3 r = cameraOrientation * (coords_w - cameraPosition);
    
    // apply intrinsic camera parameters
    Vector3 u = intrinsicParameters * r;

    // form the image coordinates
    Vector3 coords_i(u[0] / u[2], u[1] / u[2], u[2]);

    return coords_i;
}

Vector3 img2world(Vector3 coords_i,
                  Vector3 cameraPosition,
                  Quaternion cameraOrientation,
                  Matrix3 invIntrinsicParameters)
{
    // form a vector representing the object in the camera frame
    Vector3 u(coords_i[0], coords_i[1], 1);

    // take away the intrinsic parameter transform
    Vector3 r = invIntrinsicParameters * u;
    
    // rotate to the world coordinates and add back in the camera position
    Vector3 coords_w = coords_i[2] * (cameraOrientation.UnitInverse() * r) + cameraPosition;

    return coords_w;
}


} // namespace math
} // namespace ram
