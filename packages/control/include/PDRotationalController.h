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

namespace ram {
namespace control {    

class PDRotationalController : public RotationalControllerBase
{
public:
    PDRotationalController(ram::core::ConfigNode config);
    ~PDRotationalController() {}
};

} // namespace control
} // namespace ram

#endif // RAM_CONTROL_PDROTATIONALCONTROLLER_H
