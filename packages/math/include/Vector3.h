/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/include/Vector3.h
 */

#ifndef RAM_MATH_VECTOR3_07_09_2007
#define RAM_MATH_VECTOR3_07_09_2007

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
    
} // namespace math
} // namespace ram
    
#endif // RAM_MATH_VECTOR3_07_09_2007
