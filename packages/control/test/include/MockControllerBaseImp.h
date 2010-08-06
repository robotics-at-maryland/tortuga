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

#include <assert.h>

// Project Includes
#include "core/include/EventHub.h"
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
        desiredState(ram::controltest::DesiredStatePtr(
                            new ram::controltest::DesiredState(
                                config["DesiredState"], ram::core::EventHubPtr()))),
        translationalForceOut(0,0,0),
        rotationalTorqueOut(0,0,0)
    {
        assert(desiredState);
    }
    
    virtual ~MockControllerBaseImp() {}
            
    virtual void doUpdate(const double& timestep_,
                          const ram::math::Vector3& linearAcceleration_,
                          const ram::math::Quaternion& orientation_,
                          const ram::math::Vector3& angularRate_,
                          const double& depth_,
                          const ram::math::Vector2& position_,
                          const ram::math::Vector2& velocity_,
                          ram::math::Vector3& translationalForceOut_,
                          ram::math::Vector3& rotationalTorqueOut_)
    {
        timestep = timestep_;
        linearAcceleration = linearAcceleration_;
        orientation = orientation_;
        angularRate = angularRate_;
        depth = depth_;
        position = position_;
        velocity = velocity_;
        translationalForceOut_ = translationalForceOut;
        rotationalTorqueOut_ = rotationalTorqueOut;
    }

    // virtual void _newDepthSet(const double& newDepth)
    // {
    //     newDepthSet(newDepth);
    // }

    // virtual void _newDesiredOrientationSet(
    //     const ram::math::Quaternion& newOrientation)
    // {
    //     newDesiredOrientationSet(newOrientation);
    // }

    // virtual void _newDesiredVelocitySet(const ram::math::Vector2& newVelocity)
    // {
    //     newDesiredVelocitySet(newVelocity);
    // }

    // virtual void _newDesiredPositionSet(const ram::math::Vector2& newPosition)
    // {
    //     newDesiredPositionSet(newPosition);
    // }
                        

    virtual void setVelocity(ram::math::Vector2 /*velocity_*/) {}
    virtual ram::math::Vector2 getVelocity()
        { return ram::math::Vector2::ZERO; }
    virtual void setSpeed(double speed_) {}
    virtual void setSidewaysSpeed(double sidewaysSpeed_) {}
    virtual double getSpeed() { return 0; }
    virtual double getSidewaysSpeed() { return 0; }
    virtual void setDesiredVelocity(ram::math::Vector2 velocity,
                                    int frame){}
    virtual void setDesiredPosition(ram::math::Vector2 position,
                                    int frame){}
    virtual ram::math::Vector2 getDesiredVelocity(int frame)
    {return ram::math::Vector2::ZERO;}
    virtual ram::math::Vector2 getDesiredPosition(int frame)
    {return ram::math::Vector2::ZERO;}
    virtual void setDesiredPositionAndVelocity(ram::math::Vector2 position,
                                               ram::math::Vector2 velocity){}

    virtual bool atPosition(){return 0;}
    virtual bool atVelocity(){return 0;}
    
    virtual void setDepth(double) { return; }
    virtual double getDepth() { return 0; }
    virtual double getEstimatedDepth() { return 0; }
    virtual double getEstimatedDepthDot() { return 0; }
    virtual bool atDepth() { return atDepthValue; }
    virtual void holdCurrentDepth() {}

    virtual void yawVehicle(double /*degrees*/) {}
    virtual void pitchVehicle(double /*degrees*/) {}
    virtual void rollVehicle(double /*degrees*/) {}
    virtual ram::math::Quaternion getDesiredOrientation() {
        return ram::math::Quaternion::IDENTITY; }
    virtual void setDesiredOrientation(ram::math::Quaternion) {}
    virtual bool atOrientation() { return atOrientationValue; }
    virtual void holdCurrentHeading() {}
                        

    bool atDepthValue;
    bool atOrientationValue;

    ram::controltest::DesiredStatePtr desiredState;
    
    double timestep;
    ram::math::Vector3 linearAcceleration;
    ram::math::Quaternion orientation;
    ram::math::Vector3 angularRate;
    ram::math::Vector2 position;
    ram::math::Vector2 velocity;
    double depth;
    ram::math::Vector3 translationalForceOut;
    ram::math::Vector3 rotationalTorqueOut;
};

#endif	// RAM_CONTROL_TEST_MOCKCONTROLLERBASEIMP_02_28_2009
