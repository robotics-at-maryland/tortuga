/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders88@gmail.com>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  packages/estimation/src/modules/ParticleBuoyEstimationModule.h
 */

/* This algorithm is an implementation of particle filtering.  Particle filters
 * are probabilistic algorithms that approximate underlying probability 
 * distributions with a weighted set of delta functions.
 * 
 * There are essentially 4 distributions we are concerned with.
 * Prior - probability of the state given prior information
 * Posterior - probability of the state prior and current information
 * Conditional Transition Density - models time dependence of state
 * Measurement Likelihood - models likelihood of measurement assuming some state
 *
 * Initially we will represent the Prior and Posterior with particles
 * The Conditional Transition Density is a delta function for this application
 * because the underlying state does not change with time
 * The Measurement Likelihood will initially be represented by a multivariate gaussian
 * This is an ok approximation for a starting point but it should be refined 
 * in the future by taking data under known conditions and looking at the 
 * distribution of the measurements.  We can approximate the distribution
 * from an experiment with a normalized histogram and should do so in the future.
 *
 * Particle filters for this type of system are suceptible to certain phenomena.
 * The first is particle degeneracy.  This happens when the weights of all but
 * one or a few particles are effectively zero.  This prevents those particles
 * from giving further help towards modeling the underlying distribution. There
 * are several possible ways to aleviate this effect.  This first is by increasing
 * the number of particles.  Keep in mind that this increases the computational
 * cost (linearly).  Another way is called resampling.  A new set of particles
 * is drawn from the previous set with probabilty equal to the weight of each
 * previous particle.  This leads to another problem called sample impoverishment.
 * This is when the particles collapse to one or a few locations.  Obviously having
 * particles at only 1 or 2 locations is unlikely to accurately model the underlying
 * distribution.  We can attack this problem several ways. The first method is
 * to introduce artificial process noise such that the particles would disperse
 * at each timestep.  The second method is to perform full blown Markov Chain
 * Monte Carlo (MCMC) when this impoverishment is detected.  My initial work with trying
 * MCMC has been disappointing.  It still may prove to be useful but keep in mind
 * that MCMC is *very* expensive compared to anything else we are doing here so
 * it will be necessary to profile the impact on the time it takes to run the
 * algorithm.
 *
 * Initialization.  There are several ways that we can approch initializing the
 * probability distributions with particles.  The first is to make measurements
 * beforehand from the shore.  The second is to take the first one or few
 * measurements from the vision system to generate an initial set of particles.
 * I currently favor the second approach because we could easily run into problems
 * if we do not have a dense enough particle distribution and the measurements from
 * the shore would likely take many more particles to give us a reasonable chance
 * of getting an accurate estimate of the buoy position.
 *
 * Fail-Safe
 * It is possible that we could end up with all particles attaining zero weight
 * which can cause div by 0 errors and other bad things.  The algorithm just 
 * cant function without nonzero particle weights.  In this case we can start
 * over by taking the next found measurement to generate new particles by 
 * sampling from the measurement likelihood disribution.  The problem with 
 * this is that there is really no guarentee that the algorithm will converge if
 * we end up doing this many times.  So we should tune the algorithm so as to avoid
 * running into this type of situation.
 * 
 * References:
 *     Wikipedia: Particle Filters, Kernel Density Estimation, Markov
 *                Chain Monte Carlo, etc.
 *
 *     Dominic S. Lee and Nicholas K. K. Chia "A Particle Algorithm
 *                for Sequential Bayesian Parameter Estimation and Model Selection"
 *
 *     M. Sanjeev Arulampalam, Simon Maskell, Neil Gordon, and Tim
 *                Clapp "A Tutorial on Particle Filters for Online
 *                Nonlinear/Non-Gaussian Bayesian Tracking"
 *
 *     Haug, A.J. "A Tutorial on Bayesian Estimation and Tracking Techniques 
 *                Applicable to Nonlinear and Non-Gaussian Processes"
 *
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

    // have we generated initial particles yet
    bool m_initialized;

    // the obstacle name so we can access its data
    Obstacle::ObstacleType m_obstacle;

    // camera parameters for pinhole camera model
    double m_camWidth, m_camHeight;
    math::Matrix3 m_intrinsicParameters;
    math::Matrix3 m_invIntrinsicParameters;

    // the number of particles to maintain
    size_t m_numParticles;

    // storage for the particles
    std::vector<Particle3D> m_particles;

    // measurement likelihood distribution
    math::Matrix3 m_measurementLikelihood_i;
};

} // namespace estimation
} // namespace ram


#endif // RAM_ESTIMATION_PARTICLEBUOYESTIMATIONMODULE_H
