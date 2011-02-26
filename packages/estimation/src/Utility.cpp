/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/estimation/src/Utility.cpp
 */

// Project Includes
#include "estimation/include/Utility.h"
#include "math/include/Matrix3.h"
#include "math/include/Helpers.h"

namespace ram {
namespace estimation {

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
    
    return result;
}

math::Quaternion Utility::quaternionFromRate(const math::Quaternion& quatOld, 
                                             const math::Vector3& angRate,
                                             double deltaT)
{
    // work with a copy of the quaternion
    math::Quaternion qOld(quatOld);

    // find quaternion derivative based off old quaternion and ang rate
    math::Quaternion qDot = qOld.derivative(angRate);

    // trapezoidal integration
    math::Quaternion qNew = qOld + qDot*deltaT;

    qNew.normalise();
    // return the normalized orientaiton estimate
    return qNew;
}
    
} // namespace estimation
} // namespace ram
