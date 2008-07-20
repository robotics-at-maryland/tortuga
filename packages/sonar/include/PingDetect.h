/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/include/PingDetect.h
 */

#ifndef _RAM_SONAR_PINGD
#define _RAM_SONAR_PINGD

// Project Includes
#include "sonar/include/spectrum/SparseSDFTSpectrum.h"
#include "sonar/include/adctypes.h"
#include "sonar/include/Sonar.h"

namespace ram {
namespace sonar {

/**
 * A class to detect the pings produced by our hydrophones
 * It operates by xbfinding the maxima over intervals in the data.
 * From these, the smallest maximum is calculated.  When the ratio
 * of the smallest maximum to the current maximum is trigerred, the
 * ping is said to be found for that particular channel.
 * It then returns a value corresponding to the hydrophones that trigerred.
 */
class pingDetect
{
    int numchan; //number of channels actually used
    int count; //counts how many samples were received since last update
    int detected; //stores the hydrophones that detected the ping
    adcmath_t currmax[NCHANNELS]; //current maximum value
    adcmath_t minmax[NCHANNELS]; //minima over the frames
    int threshold[NCHANNELS]; //thresholds for detecting the pings
    int ping_detect_frame; //the width of frames over which the max is calculated
    SparseSDFTSpectrum<adc<16>, DFT_FRAME, NCHANNELS, nKBands> spectrum; //Fourier transform class

    public:
    pingDetect(const int* hydro_threshold, int nchan, const int* bands, int p_detect_frame);
    ~pingDetect();
    int p_update(adcdata_t *sample);
    void reset_minmax();
    void zero_values();
}; //pingDetect

}//sonar
}//ram

#endif
