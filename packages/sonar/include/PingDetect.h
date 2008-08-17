/**
 * @file packages/sonar/include/PingDetect.h
 *
 * @author Michael Levashov
 * @author Copyright (C) 2008 Robotics at Maryland
 * @author Copyright (C) 2008 Michael Levashov
 * @author All rights reserved.
 */

#ifndef _RAM_SONAR_PINGDETECT_H
#define _RAM_SONAR_PINGDETECT_H

// Project Includes
#include "sonar/include/spectrum/SparseSDFTSpectrum.h"
#include "sonar/include/adctypes.h"
#include "sonar/include/Sonar.h"

#include <bitset>

namespace ram {
namespace sonar {

/**
 * A class to detect the pings produced by our hydrophones
 * It operates by finding the maxima over intervals in the data.
 * From these, the smallest maximum is calculated.  When the ratio
 * of the smallest maximum to the current maximum is trigerred, the
 * ping is said to be found for that particular channel.
 * It then returns a value corresponding to the hydrophones that trigerred.
 */
class pingDetect
{
    int count; //counts how many samples were received since last update
    std::bitset<NCHANNELS> detected; //stores the hydrophones that detected the ping
    
    /**
     * Array of the maximum Fourier magnitudes on each channel at each k-band
     */
    adcmath_t maxMag[NCHANNELS][nKBands];
    
    /**
     * Level of the smallest maximum in the zeroth k-band of each channel
     */
    adcmath_t minmax[NCHANNELS];
    
    int threshold[NCHANNELS]; //thresholds for detecting the pings
    int ping_detect_frame; //the width of frames over which the max is calculated
    SparseSDFTSpectrum<adc<16>, DFT_FRAME, NCHANNELS, nKBands> spectrum; //Fourier transform class

    public:
    pingDetect(const int* hydro_threshold, const int* bands, int p_detect_frame);
    const std::bitset<NCHANNELS>& p_update(adcdata_t *sample);
    void reset_minmax();
    void purge();
}; //pingDetect

}//sonar
}//ram

#endif
