/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockTranslationalController.h
 */

#ifndef RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008
#define RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008

// Project Includes
#include "control/include/ITranslationalController.h"
#include "core/include/ConfigNode.h"

class MockTranslationalController :
    public ram::control::ITranslationalControllerImp
{
public:
    MockTranslationalController(ram::core::ConfigNode)
        {}
    
    virtual ~MockTranslationalController() {}

    virtual void setSpeed(double) {}

    virtual void setSidewaysSpeed(double) {}

    virtual double getSpeed() { return 0; }

    virtual double getSidewaysSpeed() { return 0; }

    virtual ram::math::Vector3 translationalUpdate(ram::math::Quaternion) {
        return ram::math::Vector3::ZERO; }
};

#endif	// RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008
