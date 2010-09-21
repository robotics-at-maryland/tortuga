/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/estimation/include/Utility.h
 */

#ifndef RAM_ESTIMATION_UTILITY_H_04_22_2009
#define RAM_ESTIMATION_UTILITY_H_04_22_2009

// Project Includes
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace estimation {

struct Utility
{
    /** This uses the Triad algorithm to determine the orientaiton
     *
     *  This relies on the acceleration vector to define the absolute down
     *  direction, it then uses the mag vector to figure out where we are
     *  pointing relative to that down vector.
     */
    static math::Quaternion quaternionFromMagAccel(const math::Vector3& mag,
                                                   const math::Vector3& accel);

    /** Compute an orientation estimate from the previous orientation and
     * the angular rate.
     */
    static math::Quaternion quaternionFromRate(const math::Quaternion &quatOld,
                                               const math::Vector3 &angRate,
                                               double deltaT);
};
    
} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_UTILITY_H_04_22_2009
