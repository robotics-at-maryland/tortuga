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


#include <stdint.h>


namespace ram {
namespace sonar {


typedef int8_t adcdata_t;
typedef int32_t adcmath_t;
typedef int32_t adcsampleindex_t;


static const int ADCDATA_MAXAMPLITUDE = (1 << (sizeof(adcdata_t) * 8 - 1)) - 1;
static const int NCHANNELS = 4;
static const int BITS_ADCCOEFF = 8;
static const float PINGDURATION = 1.3e-3;
static const float MAX_SENSOR_SEPARATION = 0.2;
static const float SPEED_OF_SOUND = 1500;
static const float NOMINAL_PING_DELAY = 2;
static const float MAXIMUM_SPEED = 3;
static const float SAMPRATE = 300000;
static const float SAMPFREQ = 1/SAMPRATE;
static const float TARGETFREQ = 30000;


int gcd(int a, int b);


template<class T>
	int8_t compare(T a, T b);


template<class T>
	int8_t sign(T);


} // namespace sonar
} // namespace ram


#endif
