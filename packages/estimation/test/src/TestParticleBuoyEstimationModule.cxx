/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/test/src/TestParticleBuoyEstimationModule.cxx
 */

// STD Includes

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

// Project Includes
#define private public
#include "estimation/include/modules/ParticleBuoyEstimationModule.h"
#undef private
#include "estimation/include/Particle3D.h"
#include "estimation/include/EstimatedState.h"
#include "core/include/EventHub.h"
#include "core/include/ConfigNode.h"
#include "vision/include/Events.h"

using namespace ram;
using namespace ram::core;
using namespace ram::estimation;

TEST(normalizeWeights)
{
    EventHubPtr eventHub = EventHubPtr(new EventHub());
    EstimatedStatePtr estState = EstimatedStatePtr(new EstimatedState(core::ConfigNode::fromString("{}"),
                                                                      eventHub));

    ObstaclePtr obstacle = ObstaclePtr(new Obstacle());
    estState->addObstacle(Obstacle::RED_BUOY, obstacle);
    ParticleBuoyEstimationModule mod = ParticleBuoyEstimationModule(core::ConfigNode::fromString(
                                                                        "{"
                                                                        "'RedBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    },"
                                                                        "'GreenBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    },"
                                                                        "'YellowBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    }"
                                                                        "}"),
                                                                    eventHub,
                                                                    estState,
                                                                    Obstacle::RED_BUOY,
                                                                    vision::EventType::BUOY_FOUND);


    Particle3D p1 = Particle3D();
    p1.likelihood = 3.5;

    Particle3D p2 = Particle3D();
    p2.likelihood = 13.8;

    Particle3D p3 = Particle3D();
    p3.likelihood = 0.34;

    mod.m_particles.clear();
    mod.m_particles.push_back(p1);
    mod.m_particles.push_back(p2);
    mod.m_particles.push_back(p3);

    mod.normalizeWeights();

    double sum = 0;
    BOOST_FOREACH(Particle3D particle, mod.m_particles)
    {
        sum += particle.likelihood;
    }

    CHECK_CLOSE(1.0, sum, 0.0001);
}

TEST(update)
{
    EventHubPtr eventHub = EventHubPtr(new EventHub());
    EstimatedStatePtr estState = EstimatedStatePtr(new EstimatedState(core::ConfigNode::fromString("{}"),
                                                                      eventHub));

    ObstaclePtr obstacle = ObstaclePtr(new Obstacle());
    estState->addObstacle(Obstacle::RED_BUOY, obstacle);
    ParticleBuoyEstimationModule mod = ParticleBuoyEstimationModule(core::ConfigNode::fromString(
                                                                        "{"
                                                                        "'RedBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    },"
                                                                        "'GreenBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    },"
                                                                        "'YellowBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    }"
                                                                        "}"),
                                                                    eventHub,
                                                                    estState,
                                                                    Obstacle::RED_BUOY,
                                                                    vision::EventType::BUOY_FOUND);


    ram::vision::BuoyEventPtr buoyEvent = ram::vision::BuoyEventPtr(new ram::vision::BuoyEvent());

    mod.update(buoyEvent);
    mod.update(buoyEvent);
    mod.update(buoyEvent);
    mod.update(buoyEvent);
}
TEST(particleComparator)
{
    Particle3D p1 = Particle3D();
    p1.likelihood = 3.5;

    Particle3D p2 = Particle3D();
    p2.likelihood = 13.8;

    Particle3D p3 = Particle3D();
    p3.likelihood = 0.34;

    Particle3D p4 = Particle3D();
    p4.likelihood = 3.5;

    CHECK(!ParticleBuoyEstimationModule::particleComparator(p4, p1));
    CHECK(ParticleBuoyEstimationModule::particleComparator(p2, p1));
    CHECK(!ParticleBuoyEstimationModule::particleComparator(p3, p4));
}

TEST(getBestEstimate)
{
    EventHubPtr eventHub = EventHubPtr(new EventHub());
    EstimatedStatePtr estState = EstimatedStatePtr(new EstimatedState(core::ConfigNode::fromString("{}"),
                                                                      eventHub));

    ObstaclePtr obstacle = ObstaclePtr(new Obstacle());
    estState->addObstacle(Obstacle::RED_BUOY, obstacle);
    ParticleBuoyEstimationModule mod = ParticleBuoyEstimationModule(core::ConfigNode::fromString("{"
                                                                        "'RedBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    },"
                                                                        "'GreenBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    },"
                                                                        "'YellowBuoy' : {"
                                                                        "    'location' : [0, 0, 0],"
                                                                        "    'covariance' : [[1, 0, 0],"
                                                                        "                    [0, 1, 0],"
                                                                        "                    [0, 0, 1]],"
                                                                        "    'attackHeading' : 0"
                                                                        "    }"
                                                                        "} "),
                                                                    eventHub,
                                                                    estState,
                                                                    Obstacle::RED_BUOY,
                                                                    vision::EventType::BUOY_FOUND);

    Particle3D p1 = Particle3D();
    p1.location = math::Vector3(1,1,1);
    p1.likelihood = 8;

    Particle3D p2 = Particle3D();
    p2.location = math::Vector3(2,2,2);
    p2.likelihood = 1;

    Particle3D p3 = Particle3D();
    p3.location = math::Vector3(4,4,4);
    p3.likelihood = 1;

    mod.m_particles.clear();
    mod.m_particles.push_back(p1);
    mod.m_particles.push_back(p2);
    mod.m_particles.push_back(p3);
    mod.normalizeWeights();
    math::Vector3 bestEst = mod.getBestEstimate();
    math::Vector3 expected(1.4, 1.4, 1.4);

    CHECK_ARRAY_CLOSE(expected.ptr(), bestEst.ptr(), 3, 0.0001);
}
