/**
 * @file BlockDFT.cpp
 * @author Leo Singer
 *
 */

#include "BlockDFT.h"
#include <stdio.h>

BlockDFT::BlockDFT()
{
    purge();
}

BlockDFT::~BlockDFT()
{
}

void BlockDFT::purge()
{
    holdoff = 0;
}

void BlockDFT::processBlock()
{
    readBlock();

    if (holdoff == 0)
    {
        bool pingFound = true;
        int globalMaxPowerIndex;
        for (int channel = 0 ; channel < NCHANNELS ; channel ++)
        {
            // compute DFT of the current channel
            doDFT(channel);
            
            // Compute power in each channel (some arbitrary scaling to prevent overflow)
            for (int k = 0 ; k < BLOCKSIZE ; k ++)
            {
                power[k] = (((int32_t)dft[k].re*dft[k].re) >> 17)
                         + (((int32_t)dft[k].im*dft[k].im) >> 17);
            }
            
            // Compute total power in spectrum
            int32_t totalPower = 0;
            for (int k = 0 ; k < BLOCKSIZE ; k ++)
                totalPower += power[k];
            
            int maxPowerIndex = HARMONIC_MIN;
            int16_t maxPowerValue = power[HARMONIC_MIN];
            for (int k = HARMONIC_MIN+1 ; k < HARMONIC_MAX ; k ++)
            {
                int16_t powerValue = power[k];
                if (powerValue > maxPowerValue)
                {
                    maxPowerValue = powerValue;
                    maxPowerIndex = k;
                }
            }
            
            if ((int32_t)maxPowerValue * 3 < totalPower)
            {
                pingFound = false;
                break;
            }
            
            if (channel == 0)
                globalMaxPowerIndex = maxPowerIndex;
            else if (globalMaxPowerIndex != maxPowerIndex)
            {
                pingFound = false;
                break;
            }
        }
        
        if (pingFound)
        {
            holdoff = HOLDOFF_BLOCKCOUNT;
            float freq = (float)globalMaxPowerIndex*SAMPFREQ/BLOCKSIZE*0.001f;
            printf("signal at %0.3f kHz\n", freq);
            
            
        }
    } else
        --holdoff;
}
