/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockControlBaseImp.h
 */

#ifndef RAM_CONTROL_TEST_MOCKCONTROLLERBASEIMP_02_28_2009
#define RAM_CONTROL_TEST_MOCKCONTROLLERBASEIMP_02_28_2009

// Project Includes
#include "control/include/ControllerBase.h"
#include "core/include/ConfigNode.h"

class MockControllerBaseImp : public ram::control::ControllerBase
{
public:
    MockControllerBaseImp(ram::vehicle::IVehiclePtr vehicle,
                          ram::core::ConfigNode config) :
        ram::control::ControllerBase(vehicle, config),
        atDepthValue(false),
        atOrientationValue(false),
        desiredDepth(0),
        desiredOrientation(ram::math::Quaternion::IDENTITY),
        translationalForceOut(0,0,0),
        rotationalTorqueOut(0,0,0)
        {}
    
    virtual ~MockControllerBaseImp() {}

    virtual void doUpdate(const double& timestep_,
                          const ram::math::Vector3& linearAcceleration_,
                          const ram::math::Quaternion& orientation_,
                          const ram::math::Vector3& angularRate_,
                          const double& depth_,
                          ram::math::Vector3& translationalForceOut_,
                          ram::math::Vector3& rotationalTorqueOut_)
    {
        timestep = timestep_;
        linearAcceleration = linearAcceleration_;
        orientation = orientation_;
        angularRate = angularRate_;
        depth = depth_;
        translationalForceOut_ = translationalForceOut;
        rotationalTorqueOut_ = rotationalTorqueOut;
    }

    virtual void _newDepthSet(const double& newDepth)
    {
        newDepthSet(newDepth);
    }

    virtual void _newDesiredOrientationSet(
        const ram::math::Quaternion& newOrientation)
    {
        newDesiredOrientationSet(newOrientation);
    }
   

    virtual void setSpeed(double speed_) {}
    virtual void setSidewaysSpeed(double sidewaysSpeed_) {}
    virtual double getSpeed() { return 0; }
    virtual double getSidewaysSpeed() { return 0; }
    
    virtual void setDepth(double) { return; }
    virtual double getDepth() { return desiredDepth; }
    virtual double getEstimatedDepth() { return 0; }
    virtual double getEstimatedDepthDot() { return 0; }
    virtual bool atDepth() { return atDepthValue; }
    virtual void holdCurrentDepth() {}

    virtual void yawVehicle(double /*degrees*/) {}
    virtual void pitchVehicle(double /*degrees*/) {}
    virtual void rollVehicle(double /*degrees*/) {}
    virtual ram::math::Quaternion getDesiredOrientation() {
        return desiredOrientation; }
    virtual void setDesiredOrientation(ram::math::Quaternion) {}
    virtual bool atOrientation() { return atOrientationValue; }
    virtual void holdCurrentHeading() {}


    bool atDepthValue;
    bool atOrientationValue;

    double desiredDepth;
    ram::math::Quaternion desiredOrientation;

    double timestep;
    ram::math::Vector3 linearAcceleration;
    ram::math::Quaternion orientation;
    ram::math::Vector3 angularRate;
    double depth;
    ram::math::Vector3 translationalForceOut;
    ram::math::Vector3 rotationalTorqueOut;
};

#endif	// RAM_CONTROL_TEST_MOCKCONTROLLERBASEIMP_02_28_2009
