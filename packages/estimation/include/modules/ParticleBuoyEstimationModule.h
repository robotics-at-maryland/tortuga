/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/src/modules/ParticleBuoyEstimationModule.h
 */


#ifndef RAM_ESTIMATION_PARTICLEBUOYESTIMATIONMODULE_H
#define RAM_ESTIMATION_PARTICLEBUOYESTIMATIONMODULE_H

// STD Includes
#include <vector>

// Library Includes
// #include <boost/math/distributions/normal.hpp>

// Project Includes
#include "estimation/include/EstimationModule.h"
#include "estimation/include/EstimatedState.h"
#include "estimation/include/Obstacle.h"
#include "estimation/include/Particle3D.h"

#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "core/include/ReadWriteMutex.h"

#include "math/include/Math.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"
#include "math/include/Matrix3.h"
#include "math/include/Quaternion.h"
#include "math/include/Events.h"

#include "vision/include/VisionSystem.h"

namespace ram {
namespace estimation {

class ParticleBuoyEstimationModule : public EstimationModule
{
public:
    ParticleBuoyEstimationModule(core::ConfigNode config,
                                   core::EventHubPtr eventHub,
                                   EstimatedStatePtr estState,
                                   Obstacle::ObstacleType obstacle,
                                   core::Event::EventType inputEventType);

    virtual ~ParticleBuoyEstimationModule(){};

    virtual void update(core::EventPtr event);
        
private:
    double likelihood2D(math::Vector2 mean, math::Matrix2 covariance,
                       math::Vector2 location);

    double likelihood3D(math::Vector3 mean, math::Matrix3 covariance,
                       math::Vector3 location);

    double gaussian1DLikelihood(double mean, double stdDev, double location);

    math::Vector3 getBestEstimate();
    math::Matrix3 getCovariance();

    void normalizeWeights();


    static bool particleComparator(Particle3D p1, Particle3D p2)
    {
        return p1.likelihood > p2.likelihood;
    }

    // the obstacle name so we can access its data
    Obstacle::ObstacleType m_obstacle;

    // the initial guess of where the obstacle is
    math::Vector3 m_initialGuess;

    // a simplified model of uncertainty.  
    // we can move to a full covarinace matrix in the future
    math::Matrix3 m_initialUncertainty;

    // // simplified model of uncertainty for the camera
    // // assumes that these uncertainties are decoupled
    // // this assumption should result in faster likelihood computation
    // double m_imgXUncertainty;
    // double m_imgYUncertainty;
    // double m_distanceUncertainty;
    
    // camera parameters for pinhole camera model
    math::Radian m_xFOV, m_yFOV;
    double m_camWidth, m_camHeight;
    math::Matrix3 m_intrinsicParameters;
    math::Matrix3 m_invIntrinsicParameters;

    // the number of particles to maintain
    size_t m_numParticles;
    size_t m_prevEffectiveParticles;

    // storage for the particles
    std::vector<Particle3D> m_particles;
    std::vector<Particle3D> m_placeholder;

    // using double to reduce computation precision
    // we should not need the double precision to get good results
    // boost::math::normal_distribution<double> m_stdNormal;
};
} // namespace estimation
} // namespace ram


#endif // RAM_ESTIMATION_PARTICLEBUOYESTIMATIONMODULE_H
