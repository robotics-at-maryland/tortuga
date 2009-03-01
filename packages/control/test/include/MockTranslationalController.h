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
    MockTranslationalController(ram::core::ConfigNode) :
        speedSet(0), speed(0), sidewaysSpeedSet(0), sidewaysSpeed(0),
        orientation(ram::math::Quaternion::IDENTITY),
        force(0, 0, 0)
        {}
    
    virtual ~MockTranslationalController() {}

    virtual void setSpeed(double speed_) { speedSet = speed_; }

    virtual void setSidewaysSpeed(double sidewaysSpeed_) {
        sidewaysSpeedSet = sidewaysSpeed_; }

    virtual double getSpeed() { return speed; }

    virtual double getSidewaysSpeed() { return sidewaysSpeed; }

    virtual ram::math::Vector3 translationalUpdate(
        ram::math::Quaternion orientation_) {
        orientation = orientation_;
        return force; }

    double speedSet;
    double speed;
    double sidewaysSpeedSet;
    double sidewaysSpeed;
    ram::math::Quaternion orientation;
    ram::math::Vector3 force;
};

#endif	// RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008
