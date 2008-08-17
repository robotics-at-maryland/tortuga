/**
 * @file packages/sonar/src/GetPingChunk.cpp
 *
 * @author Michael Levashov
 * @author Copyright (C) 2008 Robotics at Maryland
 * @author Copyright (C) 2008 Michael Levashov
 * @author All rights reserved.
 */

// STD Includes
#include <complex>
//#include <iostream>

// Project Includes
#include "sonar/include/PingDetect.h"
#include "sonar/include/Sonar.h"
#include "sonar/include/GetPingChunk.h"

#include "drivers/bfin_spartan/include/spartan.h"
#include "drivers/bfin_spartan/include/dataset.h"

namespace ram {
namespace sonar {

getPingChunk::getPingChunk(const int* kBands)
	: pdetect(PD_THRESHOLDS, kBands, PING_DETECT_FRAME)
{}

int
getPingChunk::getChunk(adcdata_t** data, int* locations, struct dataset* dataSet)
{
    //Initialize some things
    int lastDetectedIndex = 0;
    int lastPingIndex[NCHANNELS];
    std::bitset<NCHANNELS> lastDetectedFlag;
    pdetect.purge(); //re-zero the parameters, even if I already have done it again.  Doesn't hurt that much, since it was done once
    bzero(lastPingIndex, sizeof(*lastPingIndex) * NCHANNELS);

    adcdata_t sample[NCHANNELS];
    for(int i=0 ; i < dataSet->size ; i++)
    {
        for (int channel = 0 ; channel < NCHANNELS ; channel++)
            sample[channel] = getSample(dataSet, channel, i);

        const std::bitset<NCHANNELS> detectedFlag = pdetect.p_update(sample);

        if (i < DFT_FRAME) //The DFT initializes to 0, so I ignore all points before it
            continue;
        else if (i == DFT_FRAME)
            pdetect.reset_minmax();

        if (i - lastDetectedIndex > MAX_PING_SEP)
        {
            lastDetectedFlag.reset();
            lastDetectedIndex = 0;
        }

        if (detectedFlag.any())
        {
            lastDetectedIndex = i;
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                if (detectedFlag[channel] && !lastDetectedFlag[channel])
                    lastPingIndex[channel] = i;
            lastDetectedFlag |= detectedFlag;
        }

        if (lastDetectedFlag.count() == NCHANNELS)
        {
            bool tooCloseToStart = false;
            for (int channel = 0 ; channel < NCHANNELS ; channel++)
                if (lastPingIndex[channel] < ENV_CALC_FRAME)
                {
                    tooCloseToStart = true;
                    break;
                }
            
            if (tooCloseToStart) //too close to the start, skip it
            {
                pdetect.reset_minmax();
                lastDetectedFlag.reset();
            }
            else
            {
                for(int channel = 0 ; channel < NCHANNELS ; channel++)
                {
                    for(int k=0; k < ENV_CALC_FRAME ; k++)
                        data[channel][k] = getSample(dataSet, channel, k+lastPingIndex[channel]-ENV_CALC_FRAME+1+DFT_FRAME/2); //might need to be tweaked
                    locations[channel] = lastPingIndex[channel]-ENV_CALC_FRAME+1;
                }
                //std::cout << "Ping Detected at " << locations[0] << std::endl;

                return 1;
            }
        }
    }
    return 0;
}

} // namespace sonar
} // namespace ram
