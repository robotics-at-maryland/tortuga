/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/modules/DepthKalmanModule.cpp
 */

/* This module implements a simple Kalman filter to estimate
   depth and depth_dot */

// STD Includes
#include <iostream>

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/modules/DepthKalmanModule.h"
#include "math/include/Matrix2.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "vehicle/include/device/IDepthSensor.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstDepth"));

namespace ram {
namespace estimation {

DepthKalmanModule::DepthKalmanModule(core::ConfigNode config, 
                                     core::EventHubPtr eventHub,
                                     EstimatedStatePtr estState) :
    EstimationModule(eventHub, "DepthKalmanModule", estState,
                     vehicle::device::IDepthSensor::RAW_UPDATE),
    m_xHat(math::Vector2::ZERO),
    m_Pk(math::Matrix2(2, 0, 0, 1)),
    m_filteredDepth(math::SGolaySmoothingFilterPtr(
                        new math::SGolaySmoothingFilter(5, 2))),
    m_filteredDepthDot(math::SGolaySmoothingFilterPtr(
                           new math::SGolaySmoothingFilter(5, 2)))
{
    LOGGER.info("% DepthKalmanModule depth correction zk xHat xHatDot");
}

void DepthKalmanModule::update(core::EventPtr event)
{
    // attempt to cast the event to a RawDepthSensorDataEventPtr
    vehicle::RawDepthSensorDataEventPtr ievent =
        boost::dynamic_pointer_cast<vehicle::RawDepthSensorDataEvent>(event);

    // return if the cast failed and let people know about it.
    if(!ievent){
        LOGGER.warn("Invalid Event Type");
        return;
    }

    // determine depth correction
    math::Vector3 location = ievent->sensorLocation;
    math::Vector3 currentSensorLocation = 
        m_estimatedState->getEstimatedOrientation() * location;
    math::Vector3 sensorMovement = currentSensorLocation - location;
    double correction = sensorMovement.z;

    // grab the depth and calculate the depth rate
    double rawDepth = ievent->rawDepth;

    // correction is added because positive depth is down
    double zk = rawDepth + correction;
    double dt = ievent->timestep;

    math::Matrix2 Ak(1, dt, 0, 1);       // state transition model
    math::Vector2 Hk(1, 0);              // observation model
    math::Matrix2 Qk(0.005, 0, 0, 0.002);  // process covariance
    double Rk = 0.1;                     // measurement covariance

    // prediction step
    math::Vector2 xHat = Ak * m_xHat;
    math::Matrix2 Pk = Ak * m_Pk * Ak.Transpose() + Qk;

    // update step
    double yk = zk - Hk.dotProduct(xHat);
    double Sk = Hk.dotProduct(Pk * Hk) + Rk;
    math::Vector2 K = Pk * Hk * (1 / Sk);
    xHat = xHat + K * yk;

    math::Matrix2 KHk = math::Matrix2::fromOuterProduct(K, Hk);
    Pk = (math::Matrix2::IDENTITY - KHk) * Pk;

    // store values
    m_Pk = Pk;
    m_xHat = xHat;

    // put the depth into the averaging filter
    m_filteredDepth->addValue(xHat[0]);
    m_filteredDepthDot->addValue(xHat[1]);

    // get the best estimates from the filter
    double estDepth = m_filteredDepth->getValue();
    double estDepthRate = m_filteredDepth->getValue();

    // Set the estimated depth
    m_estimatedState->setEstimatedDepth(estDepth);
    m_estimatedState->setEstimatedDepthRate(estDepthRate);

    LOGGER.infoStream() << rawDepth  << " "
                        << correction << " "
                        << zk << " "
                        << xHat[0] << " "
                        << xHat[1] << " "
                        << estDepth << " "
                        << estDepthRate << " ";
}

} // namespace estimation
} // namespace ram
