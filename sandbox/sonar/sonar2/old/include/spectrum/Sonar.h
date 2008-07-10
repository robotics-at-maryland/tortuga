/**
 * @file Sonar.h
 *
 * by Michael Levashov, a heavily butchered version of Sonar.h by Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Common constants
 *
 * All physical constants, dimensions, and times in this header file are in SI units.
 * 
 */

#ifndef _RAM_SONAR_SONAR_H
#define _RAM_SONAR_SONAR_H

#include <stdint.h>
#include <assert.h>

using namespace std;

namespace ram {
namespace sonar {


typedef int16_t adcdata_t;
typedef int32_t adcmath_t;
//typedef int32_t adcsampleindex_t;

static const int NCHANNELS = 4;
//static const int BITS_ADCCOEFF = 10;
//static const int ADCDATA_MAXAMPLITUDE = (1 << (BITS_ADCCOEFF - 1)) - 1;
//static const float PINGDURATION = 1.3e-3;
//static const float MAX_SENSOR_SEPARATION = 0.2;
static const int SPEED_OF_SOUND = 1500;
static const int NOMINAL_PING_DELAY = 2000; //in milliseconds
static const int MAX_SAMPLE_LENGTH = 3000; //in milliseconds
static const int SAMPRATE = 500000;
static const int PING_DETECT_FRAME=200;
//static const int PING_FREQ_CHANNEL=25;
static const int DFT_FRAME=512; //must be a power of 2!
//static const float MAXIMUM_SPEED = 3;
//static const float SAMPFREQ = 1/SAMPRATE;
//static const float TARGETFREQ = 30000;

//For pingDetect stuff
//static const int frequencyOfInterest = 25000;
static const int frequencyOfInterest = 20000;
static const int kBandOfInterest = (int) (frequencyOfInterest*DFT_FRAME/SAMPRATE);
static const int kBandOffCenterAmount = 10;
static const int nKBands = 3; //number of frequency bands to examine

static const int kBands[]= {kBandOfInterest, kBandOfInterest - kBandOffCenterAmount, kBandOfInterest + kBandOffCenterAmount};

} // namespace sonar
} // namespace ram

#endif
