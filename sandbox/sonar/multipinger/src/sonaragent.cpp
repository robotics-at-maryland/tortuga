/**
 * sonaragent.cpp
 *
 * @author Leo Singer
 * @author Copyright 2009 Robotics@Maryland. All rights reserved.
 *
 * General strategy:
 *   Process ADC data in chunks of BLOCKSIZE samples of each of the 4 channels.
 *   Compute the SUM and SUM OF SQUARES over the waveform of each channel
 *      during that block.  Compute the SUM OF SUMS over all four channels.
 *   Take the FFT of each channel.
 *   If each channel has the greatest spectral density at the same harmonic,
 *      AND that harmonic is within the range of harmonics of interest,
 *      AND the greatest spectral density is at least 0.3, then TRIGGER.
 *   Else, go on to the next block.
 *
 *   On TRIGGER:
 *   Determine which of the past STATRECORD_BLOCKCOUNT had the 
 *      NOISEFLOOR_BLOCKCOUNT smallest SUM OF SUMS, and then compute the 
 *      MEAN and VARIANCE of the noise from those.
 *   Next, subtract the MEAN from all of the past LOOKBACK_BLOCKCOUNT blocks.
 *   Compute the VARIANCE_THRESHOLD using the VARIANCE of the noise.
 *   For each channel:
 *      Determine which of the past LOOKBACK_BLOCKCOUNT blocks was the latest
 *          one where the per-block threshold rises above the VARIANCE_THRESHOLD.
 *      Search backward from the end of that block until a per-sample rising edge
 *          in sliding-window variance is found.  The number of samples that you
 *          had to shift backward is your time delay.
 *
 */

static const unsigned int LOG2_BLOCKSIZE = 8;
static const unsigned int LOG2_NCHANNELS = 2;
static const unsigned int NCHANNELS = 4;
static const unsigned int BLOCKSIZE = 1 << LOG2_BLOCKSIZE;
static const unsigned int HOLDOFF_BLOCKCOUNT  = 128;
static const unsigned int LOOKBACK_BLOCKCOUNT = 16;
static const unsigned int STATRECORD_BLOCKCOUNT = 4096;
static const unsigned int NOISEFLOOR_BLOCKCOUNT = 256;
static const unsigned int NOISEFLOOR_WORKSIZE = 64;

static const unsigned int HARMONIC_MIN = 10;
static const unsigned int HARMONIC_MAX = 20;
static const unsigned int HARMONIC_COUNT = HARMONIC_MAX - HARMONIC_MIN;

#include <queue>
#include <vector>
#include <stdio.h>

#ifdef __BFIN
#include "addresses.h"
#include <filter.h>
#include <fract_complex.h>
#include <fract_math.h>

complex_fract16 rfftTwid[BLOCKSIZE];

#else
#include <fftw3.h>

fftw_plan fftwPlan;
double fftwIn[BLOCKSIZE];
fftw_complex fftwOut[BLOCKSIZE];

typedef struct {
    int16_t re, im;
} complex_int16_t;

#endif

using namespace std;

struct BlockStatRecord {
    int16_t mean[NCHANNELS];
    uint32_t meanOfSquares[NCHANNELS];
    uint32_t averageVariance;
};

struct BlockStatRecordSynopsis {
    uint32_t averageVariance;
    const BlockStatRecord* stat;
    
    BlockStatRecordSynopsis() : averageVariance(0), stat(NULL) {}
    
    BlockStatRecordSynopsis(const BlockStatRecordSynopsis& rhs)
    : averageVariance(rhs.averageVariance), stat(rhs.stat) {}
    
    BlockStatRecordSynopsis(const BlockStatRecord& record)
    : averageVariance(record.averageVariance), stat(&record) {}
    
    bool operator < (const BlockStatRecordSynopsis& rhs) const {
        return averageVariance < rhs.averageVariance;
    }
};

BlockStatRecord noiseFloor;
BlockStatRecord blockStatRecordBuffer[STATRECORD_BLOCKCOUNT];
vector<BlockStatRecordSynopsis> noiseFloorBlocks(NOISEFLOOR_BLOCKCOUNT);
int holdoff = 0;
unsigned short blockIndex = 0;
unsigned short blockStatIndex = 0;
unsigned short workIndex = 0;
unsigned short triggerOriginBlockIndex = 0;
unsigned short savedBlockIndex = 0;

int16_t blocks[NCHANNELS][HOLDOFF_BLOCKCOUNT][BLOCKSIZE];


// Storage for the DFT
complex_int16_t dft[BLOCKSIZE];
uint16_t power[BLOCKSIZE];


/**
 * Determine if a block can be read from the ADCs.
 * @return true if at least @c BLOCKSIZE samples are available to be read,
 *              false if otherwise
 */
static bool isBlockAvailable()
{
#ifdef __BFIN
    return REG(ADDR_FIFO_COUNT1A) >= BLOCKSIZE;
#else
    return true;
#endif
}

static void readBlock(int blockIndex)
{
#ifdef __BFIN
    for (int i = 0 ; i < BLOCKSIZE ; i ++)
    {
        blocks[0][blockIndex][i] = REG(ADDR_FIFO_OUT1A);
        blocks[1][blockIndex][i] = REG(ADDR_FIFO_OUT1B);
        blocks[2][blockIndex][i] = REG(ADDR_FIFO_OUT2A);
        blocks[3][blockIndex][i] = REG(ADDR_FIFO_OUT2B);
    }
#else
    int16_t temp[BLOCKSIZE][NCHANNELS];
    
    if (1 != fread(*temp, sizeof(temp), 1, stdin))
    {
        printf("--------------------\n");
        exit(0);
    }
    
    for (uint16_t i = 0 ; i < BLOCKSIZE ; i ++)
        for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
            blocks[channel][blockIndex][i] = temp[i][channel];
#endif
}

static void initDFT()
{
#ifdef __BFIN
    // Generate twiddle factor table for FFT
    twidfftrad4_fr16(fftTwid, BLOCKSIZE);
#else
    fftwPlan = fftw_plan_dft_r2c_1d(BLOCKSIZE, fftwIn, fftwOut, FFTW_PATIENT);
#endif
}

static void doDFT(int channel)
{
#ifdef __BFIN
    int block_exponent;
    // Fourier transform the current block.
    rfft_fr16(blocks[channel][blockIndex], dft, rfftTwid, 1, BLOCKSIZE, &block_exponent, 0);    
#else
    for (unsigned int i = 0 ; i < BLOCKSIZE ; i ++)
        fftwIn[i] = blocks[channel][blockIndex][i];
    
    fftw_execute(fftwPlan);
    
    for (unsigned int i = 0 ; i < BLOCKSIZE ; i ++)
    {
        dft[i].re = fftwOut[i][0];
        dft[i].im = fftwOut[i][1];
    }    
#endif
}

static void waitBlockAvailable()
{
    while (!isBlockAvailable())
        ; // Busy wait
}

static int16_t iabs(int16_t a)
{
    if (a < 0)
        return -a;
    else
        return a;
}

int main(int argc, char** argv)
{
    initDFT();
    
    while (1)
    {
        // Wait for data available, then read it.
        waitBlockAvailable();
        readBlock(blockIndex);
        
        
        
        
        // Update statistics for this block
        BlockStatRecord& stat = blockStatRecordBuffer[blockStatIndex];
        {
            int32_t sumOfMeans = 0;
            uint64_t sumOfMeansOfSquares = 0;
            
            for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
            {
                int32_t sum = 0;
                uint64_t sumOfSquares = 0;
                
                for (unsigned int i = 0 ; i < BLOCKSIZE ; i ++)
                {
                    const int16_t& sample = blocks[channel][blockIndex][i];
                    sum += sample;
                    sumOfSquares += (int32_t)sample*sample;
                }
                
                const int16_t mean = sum >> LOG2_BLOCKSIZE;
                const uint32_t meanOfSquares = sumOfSquares >> LOG2_BLOCKSIZE;
                
                stat.mean[channel] = mean;
                stat.meanOfSquares[channel] = meanOfSquares;
                sumOfMeans += mean;
                sumOfMeansOfSquares += meanOfSquares;
            }
            
            stat.averageVariance =
            ((uint32_t)(sumOfMeansOfSquares >> LOG2_NCHANNELS)) - 
            ((uint32_t)(((uint64_t)((int64_t)sumOfMeans*sumOfMeans)) >> (LOG2_NCHANNELS * 2)));
        }
        
        
        
        
        // If the holdoff counter is <= 0, then we should be seeking a trigger.
        if (holdoff <= 0)
        {
            bool triggered;
            {
                bool pingFound = true;
                bool loudEnough = false;
                int globalMaxPowerIndex;
                
                // Evaluate spectral density check
                for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                {
                    // Compute DFT of current channel
                    doDFT(channel);
                    
                    // Compute power in each channel (some arbitrary scaling to prevent overflow)
                    for (unsigned int k = 0 ; k < BLOCKSIZE ; k ++)
                        power[k] = (uint16_t)(((uint32_t)iabs(dft[k].re)*(uint16_t)iabs(dft[k].re)) >> 16)
                                 + (uint16_t)(((uint32_t)iabs(dft[k].im)*(uint16_t)iabs(dft[k].im)) >> 16);
                    
                    // Compute total power in spectrum by summing the power at every harmonic
                    // Note: could use Parseval's theorem, except for round-off error
                    uint32_t totalPower = 0;
                    for (unsigned int k = 0 ; k < BLOCKSIZE ; k ++)
                        totalPower += power[k];
                    
                    // Compute the maximum harmonic and its index
                    int maxPowerIndex = HARMONIC_MIN;
                    uint16_t maxPowerValue = power[HARMONIC_MIN];
                    for (unsigned int k = HARMONIC_MIN+1 ; k < HARMONIC_MAX ; k ++)
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
                
                triggered = pingFound && loudEnough;
            }
            
            
            if (triggered)
            {
                workIndex = 0;
                triggerOriginBlockIndex = blockStatIndex;
                savedBlockIndex = blockIndex;
                holdoff = HOLDOFF_BLOCKCOUNT;
            }
        }
        
        
        
        // If a trigger has occurred and the holdoff counter is greater than zero,
        // then we need to track a ping.  Because this is a very time consuming 
        // step, we are going to split the work over several blocks.  That way,
        // we can keep maintaining statistics about the signal as we work.
        if (holdoff > 0)
        {
            
            if (workIndex == 0)
            {
                
                // First step: take the NOISEFLOOR_BLOCKCOUNT most recent
                // of the past STATRECORD_BLOCKCOUNT statistics records.
                for ( ; workIndex < NOISEFLOOR_BLOCKCOUNT ; workIndex ++)
                    noiseFloorBlocks[workIndex] = blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + triggerOriginBlockIndex - workIndex) % STATRECORD_BLOCKCOUNT];
                
                // Max-heapify them.
                make_heap(noiseFloorBlocks.begin(), noiseFloorBlocks.end());
                
            } else if (workIndex < NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT) {
                
                // Spread the following work over however many iterations of the
                // main loop that it takes.  After every NOISEFLOOR_WORKSIZE
                // statistics records, check to see if the ADCs are ready to
                // give us more data, and yield to the ADCs if necessary.
                
                do { /* while (!isBlockAvailable()) */
                    
                    unsigned short endIndex =
                    min(workIndex + NOISEFLOOR_WORKSIZE,
                        NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT);
                    
                    // Process the next NOISEFLOOR_WORKSIZE statistics records.
                    for ( ; workIndex < endIndex ; workIndex ++)
                    {
                        // Get a reference to the next record.
                        const BlockStatRecord& temp = blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + triggerOriginBlockIndex - workIndex) % STATRECORD_BLOCKCOUNT];
                        
                        // If this block has a lower average variance than the
                        // current max in the heap, then replace the max in the
                        // heap with it.
                        if (temp.averageVariance < noiseFloorBlocks.begin()->averageVariance)
                        {
                            pop_heap(noiseFloorBlocks.begin(), noiseFloorBlocks.end());
                            noiseFloorBlocks.back() = temp;
                            push_heap(noiseFloorBlocks.begin(), noiseFloorBlocks.end());
                        }
                    }
                    
                } while (!isBlockAvailable());
                
            } else if (workIndex == NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT) {
                
                // After the least NOISEFLOOR_BLOCKCOUNT of the past 
                // STATRECORD_BLOCKCOUNT statistics records have been
                // found, estimate the mean and variance of the noise floor.
                
                int32_t sumOfMeans[NCHANNELS];
                uint64_t sumOfMeansOfSquares[NCHANNELS];
                bzero(sumOfMeans, sizeof(*sumOfMeans)*NCHANNELS);
                bzero(sumOfMeansOfSquares, sizeof(*sumOfMeansOfSquares)*NCHANNELS);
                
                for (vector<BlockStatRecordSynopsis>::iterator it = noiseFloorBlocks.begin() ;
                     it != noiseFloorBlocks.end() ; ++it)
                {
                    
                    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                    {
                        const BlockStatRecord& rec = *(it->stat);
                        sumOfMeans[channel] += rec.mean[channel];
                        sumOfMeansOfSquares[channel] += rec.meanOfSquares[channel];
                    }
                }
                
                for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                {
                    noiseFloor.mean[channel] = sumOfMeans[channel] >> LOG2_NCHANNELS;
                    noiseFloor.meanOfSquares[channel] = sumOfMeansOfSquares[channel] >> LOG2_NCHANNELS;
                }
                
                ++workIndex;
                
            } else if (workIndex == NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT + 1) {
                
                bool pingFound = true;
                
                
                uint16_t lags[NCHANNELS];
                bzero(lags, sizeof(*lags)*NCHANNELS);
                
                // Look back through the list to find the rising edge, then report.
                for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                {
                    const int16_t& mean = noiseFloor.mean[channel];
                    const uint32_t& meanOfSquares = noiseFloor.meanOfSquares[channel];
                    const uint32_t noiseMeanSquared = (int32_t)mean*mean;
                    const uint32_t twiceNoiseMean = 2 * mean;
                    const uint32_t noiseVariance = meanOfSquares - noiseMeanSquared;
                    const uint32_t twiceNoiseVariance = 2*noiseVariance;
                    
                    unsigned int lookBackBlock;
                    
                    // Find the latest block which is completely "quiet".
                    for (lookBackBlock = 0 ; lookBackBlock < LOOKBACK_BLOCKCOUNT - 1 ; lookBackBlock ++)
                    {
                        const BlockStatRecord& lookBackStat = blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + triggerOriginBlockIndex - lookBackBlock) % STATRECORD_BLOCKCOUNT];
                        const uint32_t blockVariance = (uint64_t)lookBackStat.meanOfSquares[channel] + noiseMeanSquared - twiceNoiseMean * lookBackStat.mean[channel];
                        
                        if (blockVariance <= twiceNoiseVariance)
                            break;
                    }
                    
                    // If no "quiet" block was found, then we didn't find a ping.
                    if (lookBackBlock == LOOKBACK_BLOCKCOUNT - 1)
                    {
                        pingFound = false;
                        break;
                    }
                    
                    const int absLookBackBlock = (STATRECORD_BLOCKCOUNT + triggerOriginBlockIndex - lookBackBlock + 1) % STATRECORD_BLOCKCOUNT;
                    const int absLookBackBlock2 = (STATRECORD_BLOCKCOUNT + triggerOriginBlockIndex - lookBackBlock ) % STATRECORD_BLOCKCOUNT;
                    
                    uint32_t diffsSquared[BLOCKSIZE];
                    for (uint16_t i = 0 ; i < BLOCKSIZE ; i ++)
                    {
                        const int16_t& sample = blocks[channel][absLookBackBlock][i];
                        const int16_t diff = sample - mean;
                        diffsSquared[i] = (int32_t)diff*diff;
                    }
                    
                    uint64_t sumOfDiffsSquared = 0;
                    for (uint16_t i = 0 ; i < BLOCKSIZE ; i ++)
                        sumOfDiffsSquared += diffsSquared[i];
                    
                    for (int16_t i = BLOCKSIZE - 1 ; i >= 0 ; i--)
                    {
                        if ((sumOfDiffsSquared >> LOG2_BLOCKSIZE) <= twiceNoiseVariance)
                            lags[channel] = BLOCKSIZE - 1 - i + lookBackBlock*BLOCKSIZE;
                        
                        sumOfDiffsSquared -= diffsSquared[i];
                        const int16_t& sample = blocks[channel][absLookBackBlock2][i];
                        const int16_t diff = sample - mean;
                        sumOfDiffsSquared += (int32_t)diff*diff;
                    }
                }
                
                if (pingFound)
                {
                    // TODO: report ping
                }
                
            }
            
            --holdoff;
        }
        
        // Update indices into circular buffers
        if (++blockIndex >= LOOKBACK_BLOCKCOUNT)
            blockIndex = 0;
        if (++blockStatIndex >= STATRECORD_BLOCKCOUNT)
            blockStatIndex = 0;
    }
    
    return 0;
}
