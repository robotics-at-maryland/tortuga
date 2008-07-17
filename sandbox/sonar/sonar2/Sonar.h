/**
 * @file Sonar.h
 *
 * by Michael Levashov, a heavily butchered version of Sonar.h oroginally started by Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Common constants
 * 
 */

#ifndef _RAM_SONAR_SONAR_H
#define _RAM_SONAR_SONAR_H

#include <stdint.h>
#include <assert.h>

using namespace std;

namespace ram {
namespace sonar {

//===============TYPE DEFINES=================
typedef int16_t adcdata_t;
typedef int32_t adcmath_t;
//typedef int32_t adcsampleindex_t;

//===============PINGER/SOUND PROPERTIES============
static const int SPEED_OF_SOUND = 1500; //m/s
static const int NOMINAL_PING_DELAY = 2000; //ms, delay between pings
static const int frequencyOfInterest = 25000;

//===============ARRAY PROPERTIES==================
static const int NCHANNELS = 4; //CHANGING THIS PARAMETER IS NOT SUPPORTED
static const int SAMPRATE = 500000; //Hz, sampling rate of the array
static const int MAX_SENSOR_SEPARATION = 1000; //mm, determines maximum alllowable distance between pings, make it slightly larger than the real value
static const int MAX_PING_SEP=(SAMPRATE*MAX_SENSOR_SEPARATION)/(1000*SPEED_OF_SOUND); //pts., maximum separation between adjacent pings
static const double hydroPlanarArray[3][2] =  //m, coordinate of hydrophones with respect to hydrophone 0
{
    {0.508, 0},	//hydrophone 1  {x, y} units is meters
    {0.51, 0.241},		//hydrophone 2  {x, y}
    {0, 0.241}		//hydrophone 3  {x, y}
};

//================PROGRAM SETTINGS=================
//-----------------DFT SETTINGS----------------------
static const int DFT_FRAME=512; //pts., size of interval to do fourier over.  NOTE: MUST BE A POWER OF 2!
static const int kBandOfInterest = (int) (frequencyOfInterest*DFT_FRAME/SAMPRATE);
static const int kBandOffCenterAmount = 10;
static const int nKBands = 3; //number of frequency bands to examine
static const int kBands[]= {kBandOfInterest, kBandOfInterest - kBandOffCenterAmount, kBandOfInterest + kBandOffCenterAmount}; //Kbands relevant for DFT

//---------------PING DETECTOR SETTINGS---------------
static const int PING_DETECT_FRAME=200; //pts.,  size of frames to do max/mins over
static const int LARGE_DATASET=int(1.25*NOMINAL_PING_DELAY*SAMPRATE/1000); //pts., size of dataset acquired to find the first ping
static const int SMALL_DATASET=int(0.2*NOMINAL_PING_DELAY*SAMPRATE/1000+DFT_FRAME); //pts., datasets acquired once we know the approximate ping position
static const int PD_THRESHOLDS[]={30,30,30,30}; //thresholds for ping detection on each channel, ratios of signal to noise

//======================= OLD PARAMS ===============
static const int ENV_CALC_FRAME=1024; //must be a power of 2

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
