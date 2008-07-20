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


typedef adctype<16>::SIGNED adcdata_t;
typedef adctype<16>::DOUBLE_WIDE::SIGNED adcmath_t;
typedef int32_t adcsampleindex_t;


/*static const int NCHANNELS = 4;*/
  static const int BITS_ADCCOEFF = 10;
static const int ADCDATA_MAXAMPLITUDE = (1 << (BITS_ADCCOEFF - 1)) - 1;
/*static const float PINGDURATION = 1.3e-3;
static const float MAX_SENSOR_SEPARATION = 0.2;
static const float NOMINAL_PING_DELAY = 2;
static const float MAXIMUM_SPEED = 3;
static const float SAMPRATE = 300000;
static const float SAMPFREQ = 1/SAMPRATE;
static const float TARGETFREQ = 30000;*/


int gcd(int a, int b);


template<class T>
	int8_t compare(T a, T b);


template<class T>
	int8_t sign(T);


//===============PINGER/SOUND PROPERTIES============
static const int SPEED_OF_SOUND = 1500; //m/s
static const int NOMINAL_PING_DELAY = 2000; //ms, delay between pings
static const int frequencyOfInterest = 25000;

//===============ARRAY PROPERTIES==================
static const int NCHANNELS = 4; // CHANGING THIS PARAMETER IS NOT SUPPORTED
static const int SAMPRATE = 500000; //Hz, sampling rate of the array
static const int MAX_SENSOR_SEPARATION = 1000; //mm, determines maximum alllowable distance between pings, make it slightly larger than the real value
static const int MAX_PING_SEP=(SAMPRATE*MAX_SENSOR_SEPARATION)/(1000*SPEED_OF_SOUND); //pts., maximum separation between adjacent pings
static const double hydroPlanarArray[3][2] =  //m, coordinate of hydrophones with respect to hydrophone 0
{
    {0, 0.4},	//hydrophone 1  {x, y} units is meters
    {0.4596, 0},		//hydrophone 2  {x, y}
    {0.4568, 0.403}		//hydrophone 3  {x, y}
};

//================PROGRAM SETTINGS=================
//-----------------DFT SETTINGS----------------------
static const int DFT_FRAME=512; //pts., size of interval to do fourier over.  NOTE: MUST BE A POWER OF 2!
static const int kBandOfInterest = (int) (frequencyOfInterest*DFT_FRAME/SAMPRATE);
static const int kBandOffCenterAmount = 3;
static const int nKBands = 3; //number of frequency bands to examine
static const int kBands[]= {kBandOfInterest, kBandOfInterest - kBandOffCenterAmount, kBandOfInterest + kBandOffCenterAmount}; //Kbands relevant for DFT

//---------------PING DETECTOR SETTINGS---------------
static const int PING_DETECT_FRAME=200; //pts.,  size of frames to do max/mins over
static const int LARGE_DATASET=int(1.25*NOMINAL_PING_DELAY*SAMPRATE/1000); //pts., size of dataset acquired to find the first ping
static const int SMALL_DATASET=int(0.2*NOMINAL_PING_DELAY*SAMPRATE/1000+DFT_FRAME); //pts., datasets acquired once we know the approximate ping position
static const int PD_THRESHOLDS[]={30,30,30,30}; //thresholds for ping detection on each channel, ratios of signal to noise
static const int ENV_CALC_FRAME=1024; //the length of the piece that is extracted for calculating the ping more precisely

//======================= OLD PARAMS ===============

static const double hydroStructureArray[3][3] =  //units currently in inches but it shouldn't matter
{
    {0,      0.984, 0,   },    //hydrophone 1  {x, y, z}
    {0.492,  0.492, 0.696},		//hydrophone 2  {x, y, z}
    {-0.492, 0.492, 0.696}		//hydrophone 3  {x, y, z}
};

//static const double hydroPlanarArray[3][2] =
//{
    //{0.566, 0.698},	//hydrophone 1  {x, y} units is meters
    //{0, 0.698},		//hydrophone 2  {x, y}
    //{0.561, 0}		//hydrophone 3  {x, y}
//};

} // namespace sonar
} // namespace ram


#endif
