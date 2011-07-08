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
    m_obstacle(obstacle),
    m_initialGuess(estState->getObstacleLocation(obstacle)),
    m_initialUncertainty(estState->getObstacleLocationCovariance(obstacle)),
    m_intrinsicParameters(math::Matrix3::IDENTITY),
    m_numParticles(100),
    m_prevEffectiveParticles(100)
{
    m_numParticles = config["numParticles"].asInt(200);

    // generate the initial particles
    boost::normal_distribution<double>
        initialX(m_initialGuess[0], m_initialUncertainty[0][0]),
        initialY(m_initialGuess[1], m_initialUncertainty[1][1]),
        initialZ(m_initialGuess[2], m_initialUncertainty[2][2]);

    boost::mt19937 rng;
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > 
        initialXGen(rng, initialX),
        initialYGen(rng, initialY),
        initialZGen(rng, initialZ);

    for(size_t i = 0; i < m_numParticles; i++)
    {
        Particle3D newParticle;
        newParticle.location[0] = initialXGen();
        newParticle.location[1] = initialYGen();
        newParticle.location[2] = initialZGen();
        newParticle.likelihood = likelihood3D(m_initialGuess,
                                              m_initialUncertainty,
                                              newParticle.location);

        m_particles.push_back(newParticle);
    }
    normalizeWeights();
    
    // get the camera intrinsic paramters
    math::Radian m_xFOV = vision::VisionSystem::getFrontHorizontalFieldOfView();
    math::Radian m_yFOV = vision::VisionSystem::getFrontVerticalFieldOfView();

    double m_camWidth = vision::VisionSystem::getFrontHorizontalPixelResolution();
    double m_camHeight = vision::VisionSystem::getFrontVerticalPixelResolution();

    double cameraFocalX = m_camWidth / std::tan(m_xFOV.valueRadians() / 2.0);
    double cameraFocalY = m_camHeight / std::tan(m_yFOV.valueRadians() / 2.0);

    m_intrinsicParameters = math::Matrix3(cameraFocalX, 0, 0,
                                          0, cameraFocalY, 0,
                                          0, 0, 1);

    m_invIntrinsicParameters = math::Matrix3(cameraFocalX, 0, 0,
                                             0, cameraFocalY, 0,
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
    
    vision::Color::ColorType color = buoyEvent->color;
    color = color;

    unsigned int xPixelCoord = buoyEvent->x * m_camWidth;
    unsigned int yPixelCoord = buoyEvent->y * m_camHeight;

    math::Vector2 objImageCoords(xPixelCoord, yPixelCoord);
    math::Matrix2 objImageCoordsUncertainty(10, 0,
                                            0, 10);

    // get the current estimated state
    math::Vector2 estPosition = m_estimatedState->getEstimatedPosition();
    double estDepth = m_estimatedState->getEstimatedDepth();
    math::Vector3 estCameraLocation(estPosition[0], estPosition[1], estDepth);
    math::Quaternion estOrientation = m_estimatedState->getEstimatedOrientation();

    // calculate the expected image coordinates
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        particle.imgCoords = world2img(
            particle.location,
            estCameraLocation,
            estOrientation,
            m_intrinsicParameters);
        
        
        particle.likelihood *= likelihood2D(objImageCoords,
                                            objImageCoordsUncertainty,
                                            particle.imgCoords);
    }
    normalizeWeights();

    // compute a measure of the number of particles with non-negligible weights
    double sumLhSq = 0;
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        sumLhSq += particle.likelihood * particle.likelihood;
    }
    size_t numEffective = std::ceil(1 / sumLhSq);

    // always keep at least 10 particles
    if(numEffective < 10)
        numEffective = 10;

    math::Vector3 bestEstimate(math::Vector3::ZERO);
    math::Matrix3 spread(math::Matrix3::ZERO);

    // particle rejuvination
    if(numEffective < m_numParticles / 3)
    {
        // sort the vector
        std::partial_sort(m_particles.begin(),
                          m_particles.begin() + numEffective,
                          m_particles.end(),
                          ParticleBuoyEstimationModule::particleComparator);
    
        // remove all but the numEffective particles with the highest likelihood
        m_placeholder.assign(m_particles.begin(), m_particles.begin() + numEffective);
        m_particles.swap(m_placeholder);

        bestEstimate = getBestEstimate();
        spread = getCovariance();

        std::cout << "Best Estimate: ";
        std::cout << bestEstimate << std::endl;
        std::cout << spread << std::endl;
    
        std::cout << "Particles Left" << std::endl;
        BOOST_FOREACH(Particle3D particle, m_particles)
        {
            std::cout << particle.location.x << " " 
                      << particle.location.y << " " 
                      << particle.location.z << " " 
                      << particle.likelihood << std::endl;
        }
        
        // std::cout << "generating new particles" << std::endl;
        // generate new particles
        boost::normal_distribution<double>
            resampleXDist(bestEstimate[0], spread[0][0]),
            resampleYDist(bestEstimate[1], spread[1][1]),
            resampleZDist(bestEstimate[2], spread[2][2]);

        boost::mt19937 rng;
        boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > 
            resampleXGen(rng, resampleXDist),
            resampleYGen(rng, resampleYDist),
            resampleZGen(rng, resampleZDist);

        int particlesToGenerate = m_numParticles - m_particles.size();
        for(int i = 0; i < particlesToGenerate; i++)
        {
            Particle3D newParticle;
            newParticle.location[0] = resampleXGen();
            newParticle.location[1] = resampleYGen();
            newParticle.location[2] = resampleZGen();
            newParticle.likelihood = likelihood3D(bestEstimate,
                                                  spread,
                                                  newParticle.location);
            m_particles.push_back(newParticle);
        }
        normalizeWeights();
    }

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
    std::cout << "2D \n" << mean << "\n" << covariance << "\n" << location << std::endl;
    double xL = gaussian1DLikelihood(mean[0], covariance[0][0], location[0]);
    double yL = gaussian1DLikelihood(mean[1], covariance[1][1], location[1]);

    return xL * yL;
}


double ParticleBuoyEstimationModule::likelihood3D(
    math::Vector3 mean, math::Matrix3 covariance,
    math::Vector3 location)
{
    std::cout << "3D \n" << mean << "\n" << covariance << "\n" << location << std::endl;
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
        sumXW += particle.location[0] * lh;
        sumYW += particle.location[1] * lh;
        sumZW += particle.location[2] * lh;
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
        expValues += particle.location;
    }
    expValues /= n;
    
    math::Matrix3 covariance(math::Matrix3::ZERO);
    BOOST_FOREACH(Particle3D &particle, m_particles)
    {
        for(int x = 0; x < 3; x++)
        {
            for(int y = 0; y < 3; y++)
            {
                double xval = particle.location[x] - expValues[x];
                double yval = particle.location[y] - expValues[y];
                covariance[x][y] += (xval * yval / n);
            }
        }
    }
    return covariance;
}

} // namespace estimation
} // namespace ram
