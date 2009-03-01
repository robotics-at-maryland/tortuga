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
    MockRotationalController(ram::core::ConfigNode) :
        yaw(0),
        pitch(0),
        roll(0),
        desiredOrientation(ram::math::Quaternion::IDENTITY),
        desiredOrientationSet(ram::math::Quaternion::IDENTITY),
        atOrientationValue(false),
        holdCurrentHeadingCount(0),
        timestep(0),
        orientation(ram::math::Quaternion::IDENTITY),
        angularRate(0, 0, 0),
        torque(0, 0, 0)
        {}
    
    virtual ~MockRotationalController() {}

    virtual void yawVehicle(double degrees) { yaw = degrees; }

    virtual void pitchVehicle(double degrees) { pitch = degrees; }

    virtual void rollVehicle(double degrees) { roll = degrees; }

    virtual ram::math::Quaternion getDesiredOrientation() {
        return desiredOrientation; }
    
    virtual void setDesiredOrientation(ram::math::Quaternion orientation) {
        desiredOrientationSet = orientation; }
    
    virtual bool atOrientation() { return atOrientationValue; }

    virtual void holdCurrentHeading() { holdCurrentHeadingCount++; }

    virtual ram::math::Vector3 rotationalUpdate(double timestep_,
        ram::math::Quaternion orientation_,
        ram::math::Vector3 angularRate_) {
        timestep = timestep_;
        orientation = orientation_;
        angularRate = angularRate_;
        return torque; }

    double yaw;
    double pitch;
    double roll;
    ram::math::Quaternion desiredOrientation;
    ram::math::Quaternion desiredOrientationSet;
    bool atOrientationValue;
    int holdCurrentHeadingCount;
    double timestep;
    ram::math::Quaternion orientation;
    ram::math::Vector3 angularRate;
    ram::math::Vector3 torque;
};

#endif	// RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008
