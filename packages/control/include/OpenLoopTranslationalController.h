/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/OpenLoopTranslationalController.h
 */

#ifndef RAM_CONTROL_OPENLOOPTRANSLATIONALCONTROLLER_H_07_07_2009
#define RAM_CONTROL_OPENLOOPTRANSLATIONALCONTROLLER_H_07_07_2009

// Project Includes
#include "control/include/TranslationalControllerBase.h"

namespace ram {
namespace control {    

/** A translational controller which only supports speed changes */
class OpenLoopTranslationalController : public TranslationalControllerBase
{
public:
    OpenLoopTranslationalController(ram::core::ConfigNode config);

    virtual math::Vector3 translationalUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);

private:
    /** Gain for the normal speed */
    double m_speedPGain;
    
    /** Gain for the sideways speed */
    double m_sidewaysSpeedPGain;
};

} // namespace control
} // namespace ram
    
#endif // RAM_CONTROL_OPENLOOPTRANSLATIONALCONTROLLER_H_07_07_2009
