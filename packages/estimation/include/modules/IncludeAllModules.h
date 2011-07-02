/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/ModularStateEstimator.h
 */

/* Make it easy for other files to include all the estimation modules without
 * having to change them all every time.  Instead, add all new modules to this
 * file.
*/

// Project Includes
#include "estimation/include/modules/BasicDVLEstimationModule.h"
#include "estimation/include/modules/BasicIMUEstimationModule.h"
#include "estimation/include/modules/BasicDepthEstimationModule.h"
#include "estimation/include/modules/DepthKalmanModule.h"
#include "estimation/include/modules/VisionEstimationModule.h"
#include "estimation/include/modules/DepthAveragingModule.h"
#include "estimation/include/modules/DepthSGolayModule.h"
#include "estimation/include/modules/IMUSGolayModule.h"
#include "estimation/include/modules/ParticleVisionEstimationModule.h"
#include "estimation/include/modules/ParticleBuoyEstimationModule.h"
