/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/PIDDepthController.cpp
 */

#include "control/include/PDRotationalController.h"
#include "control/include/ControllerMaker.h"

namespace ram {
  namespace control {

    static RotationalControllerImpMakerTemplate<PDRotationalController>
    registerPDRotationalController("PDRotationalController");

    PDRotationalController::PDRotationalController(core::ConfigNode config) :
      RotationalControllerBase(config) {}
    
  } // namespace control
} // namespace ram
