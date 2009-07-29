/**
 * @file Sonar.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Common typedefs and constants for both C and C++
 *
 * All physical constants, dimensions, and times in this header file are in SI
 * units.
 * 
 */


#ifndef _RAM_SONAR_SONAR_H
#define _RAM_SONAR_SONAR_H

// STD Includes
#include <stdint.h>

// Project Includes
#include "adctypes.h"

namespace ram {
namespace sonar {


typedef adc<16>::SIGNED adcdata_t;
typedef adc<16>::DOUBLE_WIDE::SIGNED adcmath_t;
typedef int32_t adcsampleindex_t;


//===============PINGER/SOUND PROPERTIES============
static const int SPEED_OF_SOUND = 1500; //m/s
static const int NOMINAL_PING_DELAY = 2000; //ms, delay between pings
static const double frequencyOfInterest = 25000;

//===============PRACTICE PINGER (RED HERRING) PROPERTIES==============
static const double frequencyRedHerring = 27000;

//===============ARRAY PROPERTIES==================
static const int NCHANNELS = 4; // CHANGING THIS PARAMETER IS NOT SUPPORTED
static const int SAMPRATE = 500000; //Hz, sampling rate of the array
static const int MAX_SENSOR_SEPARATION = 1000; //mm, determines maximum alllowable distance between pings, make it slightly larger than the real value
static const int MAX_PING_SEP=(SAMPRATE*MAX_SENSOR_SEPARATION)/(1000*SPEED_OF_SOUND); //pts., maximum separation between adjacent pings
static const double hydroPlanarArray[3][2] =  //m, coordinate of hydrophones with respect to hydrophone 0
{
    {0, 0.4},			//hydrophone 1  {x, y} units is meters
    {0.4596, 0},		//hydrophone 2  {x, y}
    {0.4568, 0.403}		//hydrophone 3  {x, y}
};


//================PROGRAM SETTINGS=================
//-----------------DFT SETTINGS----------------------
static const int DFT_FRAME=500; //pts., size of interval to do fourier over
static const int kBandOfInterest = (int) (frequencyOfInterest*DFT_FRAME/SAMPRATE);
static const int kBandRedHerring = (int) (frequencyRedHerring*DFT_FRAME/SAMPRATE);
static const int nKBands = 2; //number of frequency bands to examine
static const int kBands[]= {kBandOfInterest, kBandRedHerring}; //Kbands relevant for DFT

//---------------PING DETECTOR SETTINGS---------------
static const int PING_DETECT_FRAME=200; //pts.,  size of frames to do max/mins over
static const int LARGE_DATASET=int(1.25*NOMINAL_PING_DELAY*SAMPRATE/1000); //pts., size of dataset acquired to find the first ping
static const int SMALL_DATASET=int(0.2*NOMINAL_PING_DELAY*SAMPRATE/1000);//+DFT_FRAME); //pts., datasets acquired once we know the approximate ping position
static const int PD_THRESHOLDS[]={30,30,30,30}; //thresholds for ping detection on each channel, ratios of signal to noise
static const int ENV_CALC_FRAME=1024; //the length of the piece that is extracted for calculating the ping more precisely
//---------------PING QUALITY SETTINGS-----------------
static const float PING_FIT_THRESHOLD=0.1;
static const float VECTOR_QUAL_THRESHOLD=1.2;
static const int FREQ_REJECT_RATIO=2; //the ratio of primary to secondary (false) frequencies for which the ping is assumed to be false and is rejected

} // namespace sonar
} // namespace ram


#endif
