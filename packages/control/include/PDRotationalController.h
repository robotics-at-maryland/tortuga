/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/PIDDepthController.cpp
 */


#ifndef RAM_CONTROL_PDROTATIONALCONTROLLER_H
#define RAM_CONTROL_PDROTATIONALCONTROLLER_H

#include "control/include/RotationalControllerBase.h"

#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"

namespace ram {
namespace control {    

class PDRotationalController : public RotationalControllerBase
{
public:
    PDRotationalController(ram::core::ConfigNode config);
    ~PDRotationalController() {}

    virtual math::Vector3 rotationalUpdate(double timestep,
                                           math::Quaternion orientation,
                                           math::Vector3 angularRate);

private:
    math::Quaternion m_desiredOrientation;
    double dtMin, dtMax;
    double m_kp, m_kd;
    double inertiaEstimate[3][3];

    math::Vector3 w_error(double ar0, double ar1, double ar2);

};

} // namespace control
} // namespace ram

#endif // RAM_CONTROL_PDROTATIONALCONTROLLER_H
