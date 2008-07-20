/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/src/PingDetect.cpp
 */

// STD Includes
#include <cmath>

// Project Includes
#include "sonar/include/Sonar.h"
#include "sonar/include/fixed/fixed.h"
#include "sonar/include/PingDetect.h"

using namespace ram::sonar;

/* Constructor for pingDetect class.  Initializes the Fourier Transform,
 * counting variables
 *
 * @param bands An array of frequency index bands to examine.
 *              The zeroth band is taken to be the "band of interest".
 *              A "ping" is registered only if the k-band of interest has the
 *              greatest magnitude of all the k-bands provided.
 */
pingDetect::pingDetect(const int* hydro_threshold, int nchan, const int* bands, int p_detect_frame)
	: spectrum(bands)
{
    numchan=nchan;
    ping_detect_frame=p_detect_frame;

    count=0;
    detected=0;

    for(int k=0; k<numchan; k++)
    {
        threshold[k]=hydro_threshold[k];
        currmax[k] = 0;
        minmax[k] = adcmath_t(1) << 30;
    }
}
        
pingDetect::~pingDetect()
{
}

/* Re-initializes the parameters for a new calculation.
 * Similar to the constructor
 */
void pingDetect::zero_values()
{
    count=0;
    detected=0;

    for(int k=0; k<numchan; k++)
    {
        currmax[k] = 0;
        minmax[k] = adcmath_t(1) << 30;
    }
}
        
/* Updates the Fourier Transform with sample then updates the min-max
 * algorithm.
 * It returns a sum of the channel values indicating if ping was found.
 * 1 -for channel 1, 2 -for ch 2, 4-for ch 3, 8- for chan 4.
 * So, the value is 0 if there were no pings found, 15 if all 4 found.
 */
int
pingDetect::p_update(adcdata_t *sample)
{
    detected=0;
    spectrum.update(sample);
    for(int channel=0; channel<numchan; channel++)
    {
        adc<16>::QUADRUPLE_WIDE::SIGNED temp=adcmath_t(fixed::magL1(spectrum.getAmplitudeForBinIndex(0,channel)));
        if(temp>currmax[channel])
        {
            //bool firstBandIsLoudest = true;
            //for (int kBand = 1 ; kBand < nKBands ; kBand ++)
                //if (adcmath_t(fixed::magL1(spectrum.getAmplitudeForBinIndex(kBand, channel))) > temp)
                    //firstBandIsLoudest = false;
            //if (firstBandIsLoudest)
                currmax[channel]=temp; //update the maximum
        }
    }

    ++count;
    if(count==ping_detect_frame) //if at the end of max frame
    {
        count=0;
        for(int channel=0; channel<numchan; channel++)
        {
            if(currmax[channel]<minmax[channel])
                minmax[channel]=currmax[channel];
            else if(currmax[channel] > (threshold[channel]*minmax[channel]))
                detected += (1 << channel); //Adds 1 for channel 1, 2 for 2, 4 for 3, 8 for 4
            currmax[channel]=0; //reset max, so that it works with the update max for loop
        }
    }

    return detected;
}

/* Resets the smallest maxima to high values.
 * Calling this helps the function eliminate high-amplitude noise
 * and recalibrate itself to the new noise level
 * Normally, would be called after a ping was detected
 */
void
pingDetect::reset_minmax()
{
    for(int k=0; k<numchan; k++)
        minmax[k] = adcmath_t(1) << 30;
}
