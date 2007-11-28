/*
 *  Sonar.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Common typedefs and constants
 *
 */

#ifndef SONAR_H
#define SONAR_H

#include "stdint.h"

typedef int8_t adcdata_t;
typedef int32_t adcmath_t;
typedef uint32_t adcsampleindex_t;

#define BITS_ADCCOEFF 8
#define PINGDURATION 1.3e-3

#endif
