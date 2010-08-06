/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/modules/DepthKalmanModule.cpp
 */

/* This module implements a simple Kalman filter to estimate depth and depth_dot */
// Library Includes
#include <iostream>
#include <boost/smart_cast.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/modules/DepthKalmanModule.h"
#include "math/include/Matrix2.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDepth"));

namespace ram {
namespace estimation {

DepthKalmanModule::DepthKalmanModule(core::ConfigNode config, 
                                     core::EventHubPtr eventHub) :
    EstimationModule(eventHub, "DepthKalmanModule"),
    m_location(math::Vector3::ZERO),
    m_calibSlope(0),
    m_calibIntercept(0),
    m_mass(30),
    m_x0(math::Vector2::ZERO),
    m_P0(math::Matrix2(0.5,0.,0.,0.)),
    m_uPrev(0)
{
    /* initialization from config values should be done here */

    LOGGER.info("% Name EstDepth");
}


void DepthKalmanModule::init(core::EventPtr event)
{
    // receive the sensor config parameters
    vehicle::DepthSensorInitEventPtr ievent = 
        boost::dynamic_pointer_cast<vehicle::DepthSensorInitEvent>(event);

    if(!event) {
        std::cerr << "DepthKalmanModule: init: Invalid Event Type"
                  << std::endl; 
        return;
    }

    m_name = ievent->name;
    m_location = ievent->location;
    m_calibSlope = ievent->depthCalibSlope;
    m_calibIntercept = ievent->depthCalibIntercept;
}

void DepthKalmanModule::update(core::EventPtr event, 
                               EstimatedStatePtr estimatedState)
{
    /* Attempt to cast the event to a RawDepthSensorDataEventPtr */
    vehicle::RawDepthSensorDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDepthSensorDataEvent>(event);

    /* Return if the cast failed and let people know about it. */
    if(!ievent){
        std::cerr << "DepthKalmanModule: update: Invalid Event Type" 
                  << std::endl;
        return;
    }

    /* This is where the estimation should be done
       The result should be stored in estimatedState */

    // Determine depth correction
    math::Vector3 currentSensorLocation = 
      estimatedState->getEstOrientation() * m_location;
    math::Vector3 sensorMovement = 
      currentSensorLocation - m_location;
    double correction = sensorMovement.z;
    
    // Grab the depth
    double depth = ievent->rawDepth;
    // Add since depth is positive
    double x_meas = depth + correction;

    double dt = ievent->timestep;
    double dt_sq = dt * dt;


    // State Transition Model
    math::Matrix2 Ak(1, dt,
		     0, 1 - (m_drag * dt_sq) / (2 * m_mass));

    // Control Input Model - F_thrusters * Bk = x change from control
    math::Vector2 Bk(dt_sq /(2*m_mass), dt / m_mass);

    // Observation Model
    math::Vector2 Hk(1, 0);

    math::Matrix2 Q(math::Matrix2::ZERO);
    math::Vector2 Rn(math::Vector2::ZERO);

    // Update the predicted state
    math::Vector2 x_pred = Ak * m_xPrev + Bk * m_uPrev + Bk * (-m_buoyancy);
    math::Matrix2 P_pred = m_Ak_prev * m_PPrev * m_Ak_prev.Transpose() + Q;

    // Update the estimated state
    // Hk is automatically treated as a row vector for Hk*P_pred
    math::Vector2 K = P_pred*Hk*/*inverse*/(Hk*P_pred*Hk + Rn);
    m_PPrev = (1 - K.dotProduct(Hk))*P_pred;
    math::Vector2 x_curr = x_pred + K*(x_meas - Hk.dotProduct(x_pred));

    // Update stored quantities
    m_xPrev = x_curr;
    m_Ak_prev = Ak;

    // Set the estimated depth
    estimatedState->setEstDepth(x_curr[0]);
    estimatedState->setEstDepthDot(x_curr[1]);

    LOGGER.infoStream() << m_name << " "
                        << depth  << " "
			<< correction << " "
			<< x_curr[0] << " "
			<< x_curr[1] << " ";
}

} // namespace estimation
} // namespace ram
