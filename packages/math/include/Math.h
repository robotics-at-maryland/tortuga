/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/include/Math.h
 */

#ifndef RAM_MATH_MATH_06_22_2006
#define RAM_MATH_MATH_06_22_2006

namespace ram {
namespace math {

class Vector3
{
public:
    Vector3(double _e1 = 0, double _e2 = 0, double _e3 = 0) :
        e1(_e1), e2(_e2), e3(_e3) {}

    double e1;
    double e2;
    double e3;
};

class Quaternion
{
public:
    Quaternion(double _q1 = 0, double _q2 = 0, double _q3 = 0, double _q4 = 0) :
        q1(_q1), q2(_q2), q3(_q3), q4(_q4) {}

    double q1;
    double q2;
    double q3;
    double q4;
};
    
} // namespace math
} // namespace ram
    
#endif // RAM_MATH_MATH_06_22_2006
