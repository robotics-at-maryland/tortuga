/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/IStateEstimator.c
 */

#include "vehicle/estimator/include/IStateEstimator.h"

RAM_CORE_EVENT_TYPE(ram::estimator::IStateEstimator, ESTIMATED_DEPTH_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimator::IStateEstimator, ESTIMATED_ORIENTATION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimator::IStateEstimator, ESTIMATED_VELOCITY_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimator::IStateEstimator, ESTIMATED_POSITION_UPDATE);

RAM_CORE_EVENT_TYPE(ram::estimator::IStateEstimator, ESTIMATED_LINEARACCELERATION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimator::IStateEstimator, ESTIMATED_DEPTHDOT_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimator::IStateEstimator, ESTIMATED_ANGULARRATE_UPDATE);


