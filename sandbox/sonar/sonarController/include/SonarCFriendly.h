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


#ifndef SONARCFRIENDLY_H
#define SONARCFRIENDLY_H


#include <stdint.h>


typedef int8_t adcdata_t;
typedef int32_t adcmath_t;
typedef int32_t adcsampleindex_t;


#define ADCDATA_MAXAMPLITUDE ((int) ((1 << (sizeof(adcdata_t) * 8 - 1)) - 1))
#define BITS_ADCCOEFF ((int) 8);


#endif
