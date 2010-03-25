/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/TrackingTranslationalController.cpp
 */

#include "control/include/TrackingTranslationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Vector3.h"

namespace ram {
  namespace control {

    static TranslationalControllerImpMakerTemplate<TrackingTranslationalController>
    registerTrackingTranslationalController("TrackingTranslationalController");

    TrackingTranslationalController::TrackingTranslationalController(core::ConfigNode config) :
      TranslationalControllerBase(config),
      m_prevPositionError(math::Vector2::ZERO),
      m_prevVelocityError(math::Vector2::ZERO),
      m_iPositionError(math::Vector2::ZERO),
      m_iVelocityError(math::Vector2::ZERO),
      x1kp(0), x1ki(0), x1kd(0),
      x2kp(0), x2ki(0), x2kd(0)
    {
      x1kp = config["x1kp"].asDouble();
      x1ki = config["x1ki"].asDouble();
      x1kd = config["x1kd"].asDouble();
      x2kp = config["x2kp"].asDouble();
      x2ki = config["x2ki"].asDouble();
      x2kd = config["x2kd"].asDouble();
    }

    math::Vector3 TrackingTranslationalController::
    translationalUpdate(double timestep,
			math::Vector3 linearAcceleration,
			math::Quaternion orientation,
			math::Vector2 position,
			math::Vector2 velocity)
       {
	 /* TODO */
	 /* Convert input based on current orientation */
	 /* TODO */

	 m_currentPosition = position; // assuming inertial frame
	 m_currentVelocity = velocity; // assuming inertial frame
	 
	 math::Vector2 positionPError(0,0), positionIError(0,0), positionDError(0,0);
	 math::Vector2 velocityPError(0,0), velocityIError(0,0), velocityDError(0,0);

	 positionPError = m_desiredPosition - m_currentPosition;
	 velocityPError = m_desiredVelocity - m_currentVelocity;

	 positionDError = (positionPError - m_prevPositionError)/timestep;
	 velocityDError = (velocityPError - m_prevVelocityError)/timestep;

	 positionIError = m_iPositionError + positionPError * timestep;
	 velocityIError = m_iVelocityError + velocityPError * timestep;
	 
	 double positionSig_x1, positionSig_x2;
	 double velocitySig_x1, velocitySig_x2;

	 positionSig_x1 = 
	   x1kp * positionPError[0] +
	   x1kd * positionDError[0] +
	   x1ki * positionIError[0];

	 positionSig_x2 = 
	   x2kp * positionPError[1] +
	   x2kd * positionDError[1] +
	   x2ki * positionIError[1];

	 velocitySig_x1 = 
	   x1kp * velocityPError[0] +
	   x1kd * positionDError[0] +
	   x1ki * positionIError[0];

	 velocitySig_x2 = 
	   x2kp * velocityPError[1] +
	   x2kd * positionDError[1] +
	   x2ki * positionIError[1];

	 double positionSignal[3] = {positionSig_x1, positionSig_x2, 0};
	 double velocitySignal[3] = {velocitySig_x1, velocitySig_x2, 0};
	 double combinedSignal[3] = {positionSig_x1 + velocitySig_x1,
				     positionSig_x2 + velocitySig_x2, 0};
	
	 double signal[3] = {0,0,0};

	 switch(m_controlMode){
	 case ControlMode::POSITION:
	   signal[0] = positionSignal[0];
	   signal[1] = positionSignal[1];
	   signal[2] = positionSignal[2];
	   break;
	 case ControlMode::VELOCITY:
	   signal[0] = velocitySignal[0];
	   signal[1] = velocitySignal[1];
	   signal[2] = velocitySignal[2];
	   break;
	 case ControlMode::POSITIONANDVELOCITY:
	   signal[0] = combinedSignal[0];
	   signal[1] = combinedSignal[1];
	   signal[2] = combinedSignal[2];
	   break;
	 case ControlMode::OPEN_LOOP:
	   break;
	 }

	 math::Vector3 translationalSignal(signal);

	 /* TODO */
	 /* At this point, convert translationalSignal back to the body frame */
	 /* TODO */

	 m_prevPositionError = positionPError;
	 m_prevVelocityError = velocityPError;

	 return translationalSignal;
       }

  } // namespace control
} // namespace ram
