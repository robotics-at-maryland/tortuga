/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockRotationalController.h
 */

#ifndef RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008
#define RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008

// Project Includes
#include "control/include/IRotationalController.h"
#include "core/include/ConfigNode.h"

class MockRotationalController :
    public ram::control::IRotationalControllerImp
{
public:
    MockRotationalController(ram::core::ConfigNode)
        {}
    
    virtual ~MockRotationalController() {}

    virtual void yawVehicle(double /*degrees*/) {}

    virtual void pitchVehicle(double /*degrees*/) {}

    virtual void rollVehicle(double /*degrees*/) {}

    virtual ram::math::Quaternion getDesiredOrientation() {
        return ram::math::Quaternion::IDENTITY; }
    
    virtual void setDesiredOrientation(ram::math::Quaternion) {}
    
    virtual bool atOrientation() { return true; }

    virtual void holdCurrentHeading() {}

    virtual ram::math::Vector3 rotationalUpdate(
        ram::math::Quaternion /*orientation*/,
        ram::math::Vector3 /*angularRate*/) { return ram::math::Vector3::ZERO; }
};

#endif	// RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008
