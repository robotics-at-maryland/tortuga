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
#include "math/include/MatrixN.h"
#include "iostream"

namespace ram {
namespace estimation {
math::Quaternion Utility::questQuaternionFromMagAccel(const math::Vector3& mag,
                                                      const math::Vector3& accel,
                                                      double a1,double a2)
{
    //rest of the terms of this experession are either zero or 1
    double cos_func= mag.crossProduct(accel).normalise();
    //paper refers to this as lamda max
    double eig_val_max=sqrt(a1*a1+2*a1*a2*cos_func+a2*a2);
    math::Vector3 mag_nf(1,0,0);
    math::Vector3 accel_nf(0,0,-1);
    math::Matrix3 B=a1*math::Matrix3::fromOuterProduct(accel_nf,accel)+
        a2*math::Matrix3::fromOuterProduct(mag_nf,mag);
    math::Matrix3 S=B+B.Transpose();
    //have to manually evaluate the trace since matrix3 doesn't support it
    double trace=0;
    math::Vector3 Z = a1*mag.crossProduct(mag_nf) + 
        a2*accel.crossProduct(accel_nf);
    for(int i=0; i<3; i++)
    {
        trace=S[i][i]+trace;
    }
    double sigma=.5*trace;
    double delta=S.Determinant();
    math::Matrix3 temp=delta*S.Inverse();
    double kappa=0;
    for(int i=0; i<3; i++)
    {
        kappa=temp[i][i]+kappa;
    }
    double alpha=(eig_val_max*eig_val_max)-(sigma*sigma)+kappa;
    double beta=eig_val_max-sigma;
    double gamma=(eig_val_max+sigma)*alpha-delta;

    math::Vector3 X=(alpha*math::Matrix3::IDENTITY+(beta*S)+(S*S))*Z;
    math::Quaternion qNew(X[0],X[1],X[2],gamma);
    qNew.normalise();
    return qNew;

}
math::Quaternion Utility::quaternionFromMagAccel(const math::Vector3& mag,
                                                 const math::Vector3& accel)
{
    // this is an implementation of the TRIAD algorithm that calculates
    // the orientation based on magnetometer and accelerometer measurements
    math::Vector3 n3 = accel * -1;
    n3.normalise();
    
    math::Vector3 n2 = mag.normalisedCopy().crossProduct(accel);
    n2.normalise();
    
    math::Vector3 n1 = n2.crossProduct(n3);
    n1.normalise();

    math::Matrix3 bCn = math::Matrix3::ZERO;
    bCn.SetColumn(0, n1);
    bCn.SetColumn(1, n2);
    bCn.SetColumn(2, n3);
    
    math::Matrix3 nCb = bCn.Transpose();
    math::Quaternion result = math::Quaternion::fromDirectionCosineMatrix(nCb);

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

    // numerical integration
    math::Quaternion qNew = qOld + (qDot * deltaT);
    qNew.normalise();

    // return the normalized orientaiton estimate
    return qNew;
}
    
} // namespace estimation
} // namespace ram
