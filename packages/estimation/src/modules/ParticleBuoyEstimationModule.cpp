/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/src/modules/ParticleBuoyEstimationModule.cpp
 */

// STD Includes
#include <cmath>
#include <algorithm>
#include <iostream>

// Library Includes
#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "estimation/include/modules/ParticleBuoyEstimationModule.h"
#include "estimation/include/Events.h"
#include "vision/include/Events.h"
#include "math/include/Helpers.h"
#include "math/include/Events.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("StEstBuoy"));

namespace ram {
namespace estimation {

ParticleBuoyEstimationModule::ParticleBuoyEstimationModule(
    core::ConfigNode config,
    core::EventHubPtr eventHub,
    EstimatedStatePtr estState,
    Obstacle::ObstacleType obstacle,
    core::Event::EventType inputEventType) :
    EstimationModule(eventHub, "ParticleBuoyEstimationModule",
                     estState, inputEventType),
    m_initialized(false),
    m_obstacle(obstacle),
    m_intrinsicParameters(math::Matrix3::IDENTITY),
    m_numParticles(config["numParticles"].asInt(200))
{
    // get the camera intrinsic paramters
    math::Radian xFOV = vision::VisionSystem::getFrontHorizontalFieldOfView();
    math::Radian yFOV = vision::VisionSystem::getFrontVerticalFieldOfView();

    double m_camWidth = vision::VisionSystem::getFrontHorizontalPixelResolution();
    double m_camHeight = vision::VisionSystem::getFrontVerticalPixelResolution();

    double cameraFocalX = m_camWidth / std::tan(xFOV.valueRadians() / 2.0);
    double cameraFocalY = m_camHeight / std::tan(yFOV.valueRadians() / 2.0);

    m_intrinsicParameters = math::Matrix3(cameraFocalX, 0, 0,
                                          0, cameraFocalY, 0,
                                          0, 0, 1);

    m_invIntrinsicParameters = m_intrinsicParameters;
    m_invIntrinsicParameters.Inverse();

    // the measurement likelihood represents the likelihood that a given
    // measurement will be made assuming a fixed value of the state
    // we are representing this with a multivariate normal distribution
    // for the time being centered around the value of the measurement
    // (mean of [0,0,0]).  this is not a great approximation but its
    // probably the easiest place to start.  this is a covariance matrix
    // for the multivariate normal distribution
    m_measurementLikelihood_i = math::Matrix3(25, 0, 0,
                                              0, 25, 0,
                                              0, 0, 1);
}

void ParticleBuoyEstimationModule::update(core::EventPtr event)
{
    vision::BuoyEventPtr buoyEvent = 
        boost::dynamic_pointer_cast<vision::BuoyEvent>(event);

    if(!buoyEvent)
    {
        std::cerr << "Invalid Event Type" << std::endl;
        return;
    }
    
    // if the buoy is touching the edge the distance estimate will be dangerous
    // we want to avoid this situation even if it means ignoring a found event
    if(buoyEvent->touchingEdge)
        return;

    // form the measurement in the image coordinate frame
    math::Vector3 measurement_i(buoyEvent->x * m_camWidth,
                                buoyEvent->y * m_camHeight,
                                buoyEvent->range);

    // get the current estimated state
    math::Vector2 robotPosition = m_estimatedState->getEstimatedPosition();
    double robotDepth = m_estimatedState->getEstimatedDepth();

    // TODO: need to get offsets for how far camera is from cg
    // should be config values
    math::Vector3 cameraLocation(robotPosition[0], robotPosition[1], robotDepth);
    math::Quaternion cameraOrientation = m_estimatedState->getEstimatedOrientation();

    /*******************************************************/
    /********* Initialization ******************************/
    /*******************************************************/

    if(!m_initialized)
    {
        // generate the initial particles based on the initial measurement
        // if our measurement likelihood is overestimated and we have enough
        // particles, this type of initialization should result in the
        // algorithm converging to an accurate solution.
        // this currently assumes no correlation between coordinates in the image frame
        boost::normal_distribution<double>
            xdist_i(measurement_i[0], m_measurementLikelihood_i[0][0]),
            ydist_i(measurement_i[1], m_measurementLikelihood_i[1][1]),
            rdist_i(measurement_i[2], m_measurementLikelihood_i[2][2]);

        boost::mt19937 rng;
        boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > 
            xgen_i(rng, xdist_i),
            ygen_i(rng, ydist_i),
            rgen_i(rng, rdist_i);

        for(size_t i = 0; i < m_numParticles; i++)
        {
            Particle3D newParticle;

            // generate sample coordinates from the distributions
            newParticle.coords_i[0] = xgen_i();
            newParticle.coords_i[1] = ygen_i();
            newParticle.coords_i[2] = rgen_i();

            // calculate the world frame coordiantes
            newParticle.coords_w = img2world(newParticle.coords_i,
                                             cameraLocation,
                                             cameraOrientation,
                                             m_invIntrinsicParameters);

            // calculate the likelihood based on the measurement likelihood model
            newParticle.likelihood = (1 / m_numParticles);

            m_particles.push_back(newParticle);
        }
        normalizeWeights();
        m_initialized = true;
        return;
    }

    /*******************************************************/
    /********* Algorithm ***********************************/
    /*******************************************************/
    /* Right now this is an implementation of the sequential
     * importance sampling algorithm for parameter estimation.
     */

    double sumL = 0;    
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        // calculate the expected image coordinates
        particle.coords_i = world2img(particle.coords_w,
                                      cameraLocation,
                                      cameraOrientation,
                                      m_intrinsicParameters);
        
        // update the likelihood of each particle
        particle.likelihood *= likelihood3D(measurement_i,
                                            m_measurementLikelihood_i,
                                            particle.coords_i);

        // keep track of sum of likelihoods to make sure that
        // all weights are not zero
        sumL += particle.likelihood;
    }
    
    // all particles having likelihood of 0 is BAD and will cause problems
    if(sumL == 0)
    {
        // for now we will deal with this by reinitializing the particles
        // the algorithm not converging is better than div/0 problems
        m_particles.clear();
        m_initialized = false;
        return;
    }

    normalizeWeights();

    math::Vector3 bestEstimate = getBestEstimate();
    math::Matrix3 spread = getCovariance();

    m_estimatedState->setObstacleLocation(m_obstacle, bestEstimate);
    m_estimatedState->setObstacleLocationCovariance(m_obstacle, spread);

    ObstacleEventPtr obstacleEvent = ObstacleEventPtr(new ObstacleEvent());
    obstacleEvent->obstacle = m_obstacle;
    obstacleEvent->location = bestEstimate;
    obstacleEvent->covariance = spread;

    publish(IStateEstimator::ESTIMATED_OBSTACLE_UPDATE, obstacleEvent);
}

double ParticleBuoyEstimationModule::likelihood2D(
    math::Vector2 mean, math::Matrix2 covariance,
    math::Vector2 location)
{
    // this is currently assuming the cross terms of the covariance matrix
    // are zero.  this is a bad assumption but easier to code for the first
    // try
    double xL = gaussian1DLikelihood(mean[0], covariance[0][0], location[0]);
    double yL = gaussian1DLikelihood(mean[1], covariance[1][1], location[1]);

    return xL * yL;
}


double ParticleBuoyEstimationModule::likelihood3D(
    math::Vector3 mean, math::Matrix3 covariance,
    math::Vector3 location)
{
    // this is currently assuming the cross terms of the covariance matrix
    // are zero.  this is a bad assumption but easier to code for the first
    // try
    double xL = gaussian1DLikelihood(mean[0], covariance[0][0], location[0]);
    double yL = gaussian1DLikelihood(mean[1], covariance[1][1], location[1]);
    double zL = gaussian1DLikelihood(mean[2], covariance[2][2], location[2]);

    return xL * yL * zL;
}


double ParticleBuoyEstimationModule::gaussian1DLikelihood(
    double mean, double stdDev, double location)
{
    assert(!isnan(mean) && "mean is nan");
    assert(!isnan(stdDev) && "stdDev is nan");
    assert(!isnan(location) && "location is nan");

    double exponent = location - mean;
    exponent *= -exponent;
    exponent /= (2 * stdDev * stdDev);
    double result = std::exp(exponent);
    result /= (stdDev * std::sqrt(2 * M_PI));
    return result;
}

math::Vector3 ParticleBuoyEstimationModule::getBestEstimate()
{
    double sumXW = 0, sumYW = 0, sumZW = 0, sumW = 0;
    
    // compute the weighted average for each coordinate
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        double lh = particle.likelihood;
        sumXW += particle.coords_w[0] * lh;
        sumYW += particle.coords_w[1] * lh;
        sumZW += particle.coords_w[2] * lh;
        sumW += lh;
    }
    assert(sumW != 0 && "trying to divide by zero");
    return math::Vector3(sumXW / sumW, sumYW / sumW, sumZW / sumW);
}

void ParticleBuoyEstimationModule::normalizeWeights()
{
    double sumL = 0;
    // get the sum of all likelihoods
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        sumL += particle.likelihood;
    }
    assert(sumL != 0 && "likelihoods sum to 0");
    // divide the likelihood of each particle by the sum
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        particle.likelihood /= sumL;
    }
}

math::Matrix3 ParticleBuoyEstimationModule::getCovariance()
{
    size_t n = m_particles.size();

    math::Vector3 expValues = math::Vector3::ZERO;
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        expValues += particle.coords_w;
    }
    expValues /= n;
    
    math::Matrix3 covariance(math::Matrix3::ZERO);
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        for(int x = 0; x < 3; x++)
        {
            for(int y = 0; y < 3; y++)
            {
                double xval = particle.coords_w[x] - expValues[x];
                double yval = particle.coords_w[y] - expValues[y];
                covariance[x][y] += (xval * yval / n);
            }
        }
    }
    return covariance;
}

} // namespace estimation
} // namespace ram
