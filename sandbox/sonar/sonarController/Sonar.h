/*
 *  Sonar.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Common typedefs and constants
 *
 *  All physical constants, dimensions, and times in this header file are in SI
 *  units.
 */


#ifndef SONAR_H
#define SONAR_H


#include "stdint.h"


typedef int8_t adcdata_t;
typedef int32_t adcmath_t;
typedef int32_t adcsampleindex_t;


#define BITS_ADCCOEFF 8
#define PINGDURATION 1.3e-3
#define MAX_SENSOR_SEPARATION 0.2
#define SPEED_OF_SOUND 1500
#define NOMINAL_PING_DELAY 2
#define MAXIMUM_SPEED 3
#define SAMPRATE 1000000


template<class T>
int8_t compare(T a, T b);


template<class T>
int8_t sign(T);


#endif
