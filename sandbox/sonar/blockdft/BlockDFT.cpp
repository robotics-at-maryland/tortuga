/**
 * @file BlockDFT.cpp
 * @author Leo Singer
 *
 */

#include "BlockDFT.h"
#include <stdio.h>
#include <string.h>

static int16_t iabs(int16_t a)
{
    if (a < 0)
        return -a;
    else
        return a;
}

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
    currentBlock = 0;
    bzero(**blocks, sizeof(blocks));
    blockIndex = 0;
}

void BlockDFT::processBlock()
{
    readBlock();

    if (holdoff == 0)
    {
        bool pingFound = true;
        bool loudEnough = false;
        int globalMaxPowerIndex;
        for (int channel = 0 ; channel < NCHANNELS ; channel ++)
        {
            // compute DFT of the current channel
            doDFT(channel);
            
            // Compute power in each channel (some arbitrary scaling to prevent overflow)
            for (int k = 0 ; k < BLOCKSIZE ; k ++)
            {
                power[k] = (uint16_t)(((uint32_t)iabs(dft[k].re)*(uint16_t)iabs(dft[k].re)) >> 16)
                         + (uint16_t)(((uint32_t)iabs(dft[k].im)*(uint16_t)iabs(dft[k].im)) >> 16);
            }
            
            // Compute total power in spectrum by summing the power at every harmonic
            // Note: could use Parseval's theorem, except for round-off error
            uint32_t totalPower = 0;
            for (int k = 0 ; k < BLOCKSIZE ; k ++)
                totalPower += power[k];
            
            // Compute the maximum harmonic and its index
            int maxPowerIndex = HARMONIC_MIN;
            uint16_t maxPowerValue = power[HARMONIC_MIN];
            for (int k = HARMONIC_MIN+1 ; k < HARMONIC_MAX ; k ++)
            {
                uint16_t powerValue = power[k];
                if (powerValue > maxPowerValue)
                {
                    maxPowerValue = powerValue;
                    maxPowerIndex = k;
                }
            }
            
            // If the harmonic with the most power accounted for less than a
            // third of the signal's total power, then discard the block.
            if ((uint32_t)maxPowerValue * 3 >= totalPower)
                loudEnough = true;
            
            // If the maximum harmonic was different for any channel, then
            // discard the block.
            if (channel == 0)
            {
                globalMaxPowerIndex = maxPowerIndex;
            } else if (globalMaxPowerIndex != maxPowerIndex) {
                pingFound = false;
                break;
            }
        }
        
        // If this might be a ping, then search backward for the rising edges.
        if (pingFound && loudEnough)
        {
            // Start the holdoff counter.
            holdoff = HOLDOFF_BLOCKCOUNT;
            
            bool allLagsFound = true;
            int lags[NCHANNELS];
            
            // Start search from the end of the current block.
            int originSample = (currentBlock + 1) * BLOCKSIZE - 1;
            
            // Loop over all channels.
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
            {
                // Create a pointer to the time series for the current channel.
                int16_t* series = blocks[channel][0];
                bool isAboveNoiseThresh[BLOCKSIZE];
                
                // Initialize a running mean over the last BLOCKSIZE samples.
                int32_t lookBackTotal = 0;
                for (int i = 0 ; i < BLOCKSIZE ; i ++)
                    lookBackTotal += blocks[channel][currentBlock][i];
                int16_t lookBackMean = lookBackTotal >> LOG2_BLOCKSIZE;
                
                // Initialize a count of how many of the last BLOCKSIZE samples
                // are QUIET_THRESH counts away from the mean.
                for (int i = 0 ; i < BLOCKSIZE ; i ++)
                    isAboveNoiseThresh[i] = (iabs(blocks[channel][currentBlock][i] - lookBackMean) > QUIET_THRESH);
                
                int countAboveThresh = 0;
                for (int i = 0 ; i < BLOCKSIZE ; i ++)
                    countAboveThresh += isAboveNoiseThresh[i];
                
                // Slide backward through the time series, 
                // updating lookBackMean and countAboveThresh, 
                // until a rising edge is found.
                int lookBack;
                for (lookBack = 1 ; lookBack < LOOKBACKSAMPLES ; lookBack ++)
                {
                    // If this is a rising edge, break off the search; we're done.
                    if (countAboveThresh < (BLOCKSIZE / 20))
                        break;
                    
                    // Index to the latest sample in the sliding sums; 
                    // the one we are about to remove.
                    int oldSampleIdx = (LOOKBACKSAMPLES + originSample - lookBack + 1) % LOOKBACKSAMPLES;
                    
                    // Index to the earliest sample not in the sliding sums; 
                    // the one we are about to add.
                    int newSampleIdx = (LOOKBACKSAMPLES + originSample - lookBack - BLOCKSIZE + 1) % LOOKBACKSAMPLES;
                    
                    // Update the sliding sum and sliding mean.
                    lookBackTotal -= series[oldSampleIdx];
                    lookBackTotal += series[newSampleIdx];
                    lookBackMean = lookBackTotal >> LOG2_BLOCKSIZE;
                    
                    bool& threshFlag = isAboveNoiseThresh[(LOOKBACKSAMPLES-lookBack) % BLOCKSIZE];
                    countAboveThresh -= threshFlag;
                    threshFlag = (iabs(series[newSampleIdx] - lookBackMean) > QUIET_THRESH);
                    countAboveThresh += threshFlag;
                }
                
                if (lookBack == LOOKBACKSAMPLES)
                    allLagsFound = false;
                else
                    lags[channel] = lookBack;
            }
            
            if (allLagsFound)
            {
                float freq = (float)globalMaxPowerIndex*SAMPFREQ/BLOCKSIZE*0.001f;
                printf("signal at %02.3f kHz, block %d\n", freq, blockIndex);
                for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                    printf("   channel %d at lag %d\n", channel, lags[channel]);
                printf("   TDOAS: %d %d %d\n", lags[1]-lags[0],lags[2]-lags[0],lags[3]-lags[0]);
            }
            
        }
    } else
        --holdoff;
    
    if (++currentBlock >= BLOCKLOOKBACK)
        currentBlock = 0;
    
    ++blockIndex;
}
