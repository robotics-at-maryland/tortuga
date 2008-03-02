/*
 *  Sonar.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Common typedefs and constants for both C and C++
 *
 *  All physical constants, dimensions, and times in this header file are in SI
 *  units.
 * 
 */


#ifndef SONAR_H
#define SONAR_H


#include <stdint.h>


/* The PIC math.h is missing a few things; let's #include whatever 
 * math.h this system has and #define whatever we need that isn't
 * there already.
 */
#include <math.h>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971
#endif


#ifdef __cplusplus
namespace ram {
namespace sonar {
#endif


typedef int8_t adcdata_t;
typedef int32_t adcmath_t;
typedef int32_t adcsampleindex_t;


#ifndef __cplusplus
#define ADCDATA_MAXAMPLITUDE ((int) ((1 << (sizeof(adcdata_t) * 8 - 1)) - 1))
#define BITS_ADCCOEFF ((int) 8)
#endif


#ifdef __cplusplus
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


#endif
