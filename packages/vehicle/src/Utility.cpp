/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/Utility.cpp
 */

// Project Includes
#include "vehicle/include/Utility.h"
#include "math/include/Matrix3.h"
#include "math/include/Helpers.h"

namespace ram {
namespace vehicle {

math::Quaternion Utility::quaternionFromMagAccel(const math::Vector3& mag,
                                                 const math::Vector3& accel)
{
    math::Vector3 n3 = accel * -1;
    n3.normalise();
    
    math::Vector3 n2 = mag.normalisedCopy().crossProduct(accel);
    n2.normalise();
    
    math::Vector3 n1 = n2.crossProduct(n3);
    n1.normalise();

    math::Matrix3 bCn;
    bCn.SetColumn(0, n1);
    bCn.SetColumn(1, n2);
    bCn.SetColumn(2, n3);
    
    math::Matrix3 nCb = bCn.Transpose();

    math::Quaternion result;
    math::quaternionFromnCb((double (*)[3])(nCb[0]), result.ptr());

		//shouldn't it be this though???!?!?!?!?!
		//math::quaternionFromnCb((double (*)[3])(bCn[0]), result.ptr());
    
    return result;
}
    
} // namespace vehicle
} // namespace ram
