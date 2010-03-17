/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/AggregateController.cpp
 */

// STD Includes
#include <cmath>
#include <cstdio>
#include <iostream>

#ifdef RAM_WINDOWS
#define M_PI 3.14159265358979323846
#endif

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "control/include/AggregateController.h"
#include "control/include/ControlFunctions.h"
#include "control/include/ControllerMaker.h"

#include "vehicle/include/IVehicle.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
#include "core/include/TimeVal.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Events.h"
//#include "imu/include/imuapi.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::control::AggregateController, AggregateController);

// Create category for logging
//static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Controller"));
//static log4cpp::Category& ADPT_LOGGER(log4cpp::Category::getInstance("AdaptCtrl"));

using namespace std;

namespace ram {
  namespace control {

    AggregateController::AggregateController(vehicle::IVehiclePtr vehicle,
					     core::ConfigNode config) :
      ControllerBase(vehicle, config)
    {   
      init(config); 
    }

    AggregateController::AggregateController(core::ConfigNode config,
					     core::SubsystemList deps) :
      ControllerBase(config, deps)
    {
      init(config);
    }

    AggregateController::~AggregateController()
    {
      unbackground(true);
    }

    void AggregateController::init(core::ConfigNode config)
    {
      // Create In plane controller
      core::ConfigNode node(config["TranslationalController"]);
      m_transController = TranslationalControllerImpMaker::newObject(node);
    
      // Create depth controller
      node = config["DepthController"];
      m_depthController = DepthControllerImpMaker::newObject(node);
    
      // Create rotational controller
      node = config["RotationalController"];
      m_rotController = RotationalControllerImpMaker::newObject(node);
    }


    // TRANSLATIONAL CONTROLLER FUNCTIONS
    void AggregateController::setVelocity(math::Vector2 velocity)
    {
      m_transController->setVelocity(velocity);
    }

    math::Vector2 AggregateController::getVelocity()
    {
      return m_transController->getVelocity();
    }
   
    void AggregateController::setSpeed(double speed)
    {
      m_transController->setSpeed(speed);
    }

    void AggregateController::setSidewaysSpeed(double speed)
    {
      m_transController->setSidewaysSpeed(speed);
    }

    double AggregateController::getSpeed()
    {
      return m_transController->getSpeed();
    }

    double AggregateController::getSidewaysSpeed()
    {
      return m_transController->getSidewaysSpeed();
    }

    void AggregateController::holdCurrentPosition()
    {
      m_transController->holdCurrentPosition();
    }

    /** Sets desired velocity and velocity based control for new controllers */
    void AggregateController::setDesiredVelocity(math::Vector2 velocity)
    {
      m_transController->setDesiredVelocity(velocity);
    }
    
    /** Sets desired position and position based control for new controllers */
    void AggregateController::setDesiredPosition(math::Vector2 position)
    {
      m_transController->setDesiredPosition(position);
    }
 
    /** Sets a desired position and velocity for controling of both simultaneously */
    void AggregateController::setDesiredPositionAndVelocity(math::Vector2 position,
							    math::Vector2 velocity)
    {
      m_transController->setDesiredPositionAndVelocity(position,velocity);
    }

    /** Gets desired velocity */
    math::Vector2 AggregateController::getDesiredVelocity()
    {
      return m_transController->getDesiredVelocity();
    }

    /** Gets desired position */
    math::Vector2 AggregateController::getDesiredPosition()
    {
      return m_transController->getDesiredPosition();
    }

    bool AggregateController::atPosition()
    {
      return m_transController->atPosition();
    }

    bool AggregateController::atVelocity()
    {
      return m_transController->atVelocity();
    }


    // DEPTH CONTROLLER FUNCTIONS
    void AggregateController::setDepth(double depth)
    {
      m_depthController->setDepth(depth);
      newDepthSet(depth);
    }

    double AggregateController::getDepth()
    {
      return m_depthController->getDepth();
    }

    double AggregateController::getEstimatedDepth()
    {
      return m_depthController->getEstimatedDepth();
    }

    double AggregateController::getEstimatedDepthDot()
    {
      return m_depthController->getEstimatedDepthDot();
    }

    bool AggregateController::atDepth()
    {
      return m_depthController->atDepth();
    }

    void AggregateController::holdCurrentDepth()
    {
      m_depthController->holdCurrentDepth();
    }




    // ROTATIONAL CONTROLLER FUNCTIONS
    void AggregateController::rollVehicle(double degrees)
    {
      m_rotController->rollVehicle(degrees);
    }

    void AggregateController::pitchVehicle(double degrees)
    {
      m_rotController->pitchVehicle(degrees);
    }

    void AggregateController::yawVehicle(double degrees)
    {
      m_rotController->yawVehicle(degrees);
    }

    void AggregateController::setDesiredOrientation(math::Quaternion newQuaternion)
    {
      m_rotController->setDesiredOrientation(newQuaternion);
      newDesiredOrientationSet(newQuaternion);
    }

    bool AggregateController::atOrientation()
    {
      return m_rotController->atOrientation();
    }

    math::Quaternion AggregateController::getDesiredOrientation()
    {
      return m_rotController->getDesiredOrientation();
    }

    void AggregateController::holdCurrentHeading()
    {
      m_rotController->holdCurrentHeading();
    }
    /*
    void AggregateController::setHeading(double degrees)
    {

    }

    double AggregateController::getHeading()
    {  
      return 0;
    }

    void AggregateController::setBuoyantTorqueCorrection(double x, double y, double z)
    {

    }
    */
    void AggregateController::doUpdate(const double& timestep,
                          const math::Vector3& linearAcceleration,
                          const math::Quaternion& orientation,
                          const math::Vector3& angularRate,
                          const double& depth,
                          const math::Vector2& position,
                          const math::Vector2& velocity,
                          math::Vector3& translationalForceOut,
                          math::Vector3& rotationalTorqueOut)
    {
    // Update controllers
    math::Vector3 inPlaneControlForce(
        m_transController->translationalUpdate(timestep, linearAcceleration,
                                               orientation, position,
                                               velocity));
    math::Vector3 depthControlForce(
        m_depthController->depthUpdate(timestep, depth, orientation));
    math::Vector3 rotControlTorque(
        m_rotController->rotationalUpdate(timestep, orientation, angularRate));
    
    // Combine into desired rotational control and torque
    translationalForceOut = inPlaneControlForce + depthControlForce;
    rotationalTorqueOut = rotControlTorque;
    }



    ITranslationalControllerPtr AggregateController::getTranslationalController()
    {
      return m_transController;
    }
    
    IDepthControllerPtr AggregateController::getDepthController()
    {
      return m_depthController;
    }
    
    IRotationalControllerPtr AggregateController::getRotationalController()
    {
      return m_rotController;
    }

 
  } // namespace control
} // namespace ram
