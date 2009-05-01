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
static const unsigned int LOG2_NOISEFLOOR_BLOCKCOUNT = 8;
static const unsigned int NCHANNELS = 1 << LOG2_NCHANNELS; // 4
static const unsigned int BLOCKSIZE = 1 << LOG2_BLOCKSIZE; // 256
static const unsigned int HOLDOFF_BLOCKCOUNT  = 64;
static const unsigned int LOOKBACK_BLOCKCOUNT = 16;
static const unsigned int STATRECORD_BLOCKCOUNT = 4096;
static const unsigned int NOISEFLOOR_BLOCKCOUNT = 1 << LOG2_NOISEFLOOR_BLOCKCOUNT; // 256
static const unsigned int NOISEFLOOR_WORKSIZE = 64;

static const unsigned int HARMONIC_MIN = 10;
static const unsigned int HARMONIC_MAX = 20;
static const unsigned int HARMONIC_COUNT = HARMONIC_MAX - HARMONIC_MIN;
static const unsigned int SAMPFREQ = 500000;

#include "fft.h"
#include <queue>
#include <vector>
#include <stdio.h>
#include <assert.h>

#ifdef DEBUG
#include <cmath>
#endif

#ifdef __BFIN
#include "addresses.h"
#endif

using namespace std;

struct BlockStatRecord {
    int32_t sum[NCHANNELS];
    uint64_t sumOfSquares[NCHANNELS];
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

static BlockStatRecord noiseFloor;
static BlockStatRecord blockStatRecordBuffer[STATRECORD_BLOCKCOUNT];
static vector<BlockStatRecordSynopsis> noiseFloorBlocks(NOISEFLOOR_BLOCKCOUNT);
static int holdoff = 0;
static int triggerHarmonic;
static unsigned short blockIndex = 0;
static unsigned short blockStatIndex = 0;
static unsigned short workIndex = 0;
static unsigned short savedBlockStatIndex = 0;
static unsigned short savedBlockIndex = 0;

static int16_t blocks[NCHANNELS][HOLDOFF_BLOCKCOUNT][BLOCKSIZE];


// Storage for the DFT
static ram::sonar::fft_fract16<BLOCKSIZE> fft;
static uint16_t power[BLOCKSIZE];




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

enum {
    SAVE_WAVEFORM, DISCARD_WAVEFORM
};




static inline int16_t getNextSample(uint8_t channel)
{
#ifdef __BFIN
    return REG((ADDR_FIFO_OUT1B - ADDR_FIFO_OUT1A) * channel + ADDR_FIFO_OUT1A);
#else
    static int16_t buf[BLOCKSIZE][NCHANNELS];
    static unsigned int idx[NCHANNELS];
    static bool initialized = false;
    if (!initialized)
    {
        for (unsigned int ch = 0 ; ch < NCHANNELS ; ch ++)
            idx[ch] = BLOCKSIZE;
        initialized = true;
    }
    
    if (idx[channel] == BLOCKSIZE)
    {
        // Make sure that if we are advancing past the current block that every 
        // channel has been fully consumed
        for (unsigned int ch = 0 ; ch < NCHANNELS ; ch ++)
            assert(idx[channel] == BLOCKSIZE);
        
        if (1 != fread(*buf, sizeof(buf), 1, stdin))
        {
            printf("--------------------\n");
            exit(0);
        }
        
        for (unsigned int ch = 0 ; ch < NCHANNELS ; ch ++)
            idx[ch] = 0;
    }
    
    return buf[idx[channel]++][channel];
#endif
}



template<int mode>
static void readBlockAndUpdateStats()
{
    // Update statistics for this block
    BlockStatRecord& stat = blockStatRecordBuffer[blockStatIndex];
    
    uint64_t sumOfVariances = 0;
    
    for (uint8_t channel = 0 ; channel < NCHANNELS ; channel ++)
    {
        int32_t sum = 0;
        uint64_t sumOfSquares = 0;
        
        for (unsigned int i = 0 ; i < BLOCKSIZE ; i ++)
        {
            const int16_t sample = getNextSample(channel);
            sum += sample;
            sumOfSquares += (int32_t)sample*sample;
            
            if (mode == SAVE_WAVEFORM)
                blocks[channel][blockIndex][i] = sample;
        }
        
        stat.sum[channel] = sum;
        stat.sumOfSquares[channel] = sumOfSquares;
        
        sumOfVariances += sumOfSquares - (((uint64_t)((int64_t)sum*sum)) >> LOG2_BLOCKSIZE);
    }
    
    stat.averageVariance = sumOfVariances >> LOG2_BLOCKSIZE;
}

/**
 * Determine which harmonic, if any, produced a trigger.
 *
 * @return an integer in [HARMONIC_MIN, HARMONIC_MAX] if a trigger occurred
 *         -1 if no triger occurred
 */
static int getTriggerHarmonic(int blockIndex)
{
    if (blockStatRecordBuffer[blockStatIndex].averageVariance > 1e6)
        return 0;
    else
        return -1;
#if 0
    bool pingFound = true;
    bool loudEnough = false;
    int globalMaxPowerIndex;
    
    // Evaluate spectral density check
    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
    {
        // Compute DFT of current channel
        fft.doDFT(blocks[channel][blockIndex]);
        
        // Compute power in each channel (some arbitrary scaling to prevent overflow)
        for (unsigned int k = 0 ; k < BLOCKSIZE ; k ++)
            power[k] = (uint16_t)(((uint32_t)iabs(fft.dftOut[k].re)*(uint16_t)iabs(fft.dftOut[k].re)) >> 16)
            + (uint16_t)(((uint32_t)iabs(fft.dftOut[k].im)*(uint16_t)iabs(fft.dftOut[k].im)) >> 16);
        
        // Compute total power in spectrum by summing the power at every harmonic
        // Note: could use Parseval's theorem, except that round-off error might
        // cause trouble.
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
        //if ((uint32_t)maxPowerValue * 3 >= totalPower)
        //    loudEnough = true;
        
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
    
    if (pingFound && loudEnough)
        return globalMaxPowerIndex;
    else
        return -1;
#endif
}

int main(int argc, char** argv)
{
    
    // Initialize noise floor statistics and also waveform records
    {
        for (blockStatIndex = 0 ; blockStatIndex < STATRECORD_BLOCKCOUNT ; blockStatIndex ++)
        {
            waitBlockAvailable();
            readBlockAndUpdateStats<DISCARD_WAVEFORM>();
        }
        blockStatIndex = 0;
    }
    
    
    // Main loop
    while (1)
    {
        // Wait for data available.
        waitBlockAvailable();
        
        // If the holdoff counter is <= 0, then we should be seeking a trigger.
        if (holdoff <= 0)
        {
            readBlockAndUpdateStats<SAVE_WAVEFORM>();
            
            // Wait LOOKBACK_BLOCKCOUNT blocks after a holdoff period has ended
            // so that the waveform record gets re-filled
            if ((unsigned int)(-holdoff) < LOOKBACK_BLOCKCOUNT)
            {
                --holdoff;
            } else {
                triggerHarmonic = getTriggerHarmonic(blockIndex);
                if (triggerHarmonic != -1)
                {
                    workIndex = 0;
                    savedBlockStatIndex = blockStatIndex;
                    savedBlockIndex = blockIndex;
                    holdoff = HOLDOFF_BLOCKCOUNT;
                }
            }
        } else
            readBlockAndUpdateStats<DISCARD_WAVEFORM>();
        
        
        
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
                    noiseFloorBlocks[workIndex] = blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + savedBlockStatIndex - workIndex) % STATRECORD_BLOCKCOUNT];
                
                // Max-heapify them.
                make_heap(noiseFloorBlocks.begin(), noiseFloorBlocks.end());
                
            } else if (workIndex < STATRECORD_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT) {
                
                // Spread the following work over however many iterations of the
                // main loop that it takes.  After every NOISEFLOOR_WORKSIZE
                // statistics records, check to see if the ADCs are ready to
                // give us more data, and yield to the ADCs if necessary.
                
                do { /* while (!isBlockAvailable()) */
                    
                    unsigned short endIndex =
                    min(workIndex + NOISEFLOOR_WORKSIZE,
                        STATRECORD_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT);
                    
                    // Process the next NOISEFLOOR_WORKSIZE statistics records.
                    for ( ; workIndex < endIndex ; workIndex ++)
                    {
                        // Get a reference to the next record.
                        const BlockStatRecord& temp = blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + savedBlockStatIndex - workIndex) % STATRECORD_BLOCKCOUNT];
                        
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
                
            } else if (workIndex == STATRECORD_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT) {
                
                // After the least NOISEFLOOR_BLOCKCOUNT of the past 
                // STATRECORD_BLOCKCOUNT statistics records have been
                // found, estimate the mean and variance of the noise floor.
                
                int64_t sumOfSums[NCHANNELS];
                uint64_t sumOfSumsOfSquares[NCHANNELS];
                bzero(sumOfSums, sizeof(*sumOfSums)*NCHANNELS);
                bzero(sumOfSumsOfSquares, sizeof(*sumOfSumsOfSquares)*NCHANNELS);
                
                for (vector<BlockStatRecordSynopsis>::iterator it = noiseFloorBlocks.begin() ;
                     it != noiseFloorBlocks.end() ; ++it)
                {
                    
                    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                    {
                        const BlockStatRecord& rec = *(it->stat);
                        sumOfSums[channel] += rec.sum[channel];
                        sumOfSumsOfSquares[channel] += rec.sumOfSquares[channel];
                    }
                }
                
                for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                {
                    noiseFloor.sum[channel] = sumOfSums[channel] >> LOG2_NOISEFLOOR_BLOCKCOUNT;
                    noiseFloor.sumOfSquares[channel] = sumOfSumsOfSquares[channel] >> LOG2_NOISEFLOOR_BLOCKCOUNT;
                }
                
                ++workIndex;
                
            } else if (workIndex == STATRECORD_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT + 1) {
                
                bool pingFound = true;
                
#ifdef DEBUG
                    printf("Ch | Noise mean | Noise variance\n");
#endif
                
                
                uint16_t lags[NCHANNELS];
                bzero(lags, sizeof(*lags)*NCHANNELS);
                
                // Look back through the list to find the rising edge, then report.
                for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                {
                    const uint64_t& noiseSumOfSquares = noiseFloor.sumOfSquares[channel];
                    const int32_t& noiseSum = noiseFloor.sum[channel];
                    const uint64_t noiseSumSquared = (int64_t)noiseSum*noiseSum;
                    const int64_t noiseTwiceSum = 2 * noiseSum;
                    
                    const int16_t noiseMean = noiseSum >> LOG2_BLOCKSIZE;
                    
                    const uint64_t noiseVariance = noiseSumOfSquares - (noiseSumSquared >> LOG2_BLOCKSIZE);
                    const uint64_t noiseThreshold = 3 * noiseVariance;
                    
#ifdef DEBUG
                    printf(" %d | %10d | %d \n", channel, noiseMean, (int)std::sqrt((uint32_t)(noiseVariance >> LOG2_BLOCKSIZE)));
#endif
                    
                    unsigned int lookBackBlock;
                    
                    // Find the latest block which is completely "quiet".
                    for (lookBackBlock = 0 ; lookBackBlock < LOOKBACK_BLOCKCOUNT - 1 ; lookBackBlock ++)
                    {
                        const BlockStatRecord& lookBackStat = blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + savedBlockStatIndex - lookBackBlock) % STATRECORD_BLOCKCOUNT];
                        const uint64_t blockVariance = (int64_t)lookBackStat.sumOfSquares[channel] + (((int64_t)noiseSumSquared - noiseTwiceSum*lookBackStat.sum[channel]) >> LOG2_BLOCKSIZE);
                        
                        if (blockVariance <= noiseThreshold)
                            break;
                    }
                    
                    // If no "quiet" block was found, then we didn't find a ping.
                    if (lookBackBlock == LOOKBACK_BLOCKCOUNT - 1)
                    {
                        pingFound = false;
                        break;
                    }
                    
                    const int absLookBackBlock = (LOOKBACK_BLOCKCOUNT + savedBlockIndex - lookBackBlock + 1) % LOOKBACK_BLOCKCOUNT;
                    const int absLookBackBlock2 = (LOOKBACK_BLOCKCOUNT + savedBlockIndex - lookBackBlock) % LOOKBACK_BLOCKCOUNT;
                    
                    uint32_t diffsSquared[BLOCKSIZE];
                    for (uint16_t i = 0 ; i < BLOCKSIZE ; i ++)
                    {
                        const int16_t& sample = blocks[channel][absLookBackBlock][i];
                        const int32_t diff = sample - noiseMean;
                        diffsSquared[i] = (int32_t)diff*diff;
                    }
                    
                    uint64_t sumOfDiffsSquared = 0;
                    for (uint16_t i = 0 ; i < BLOCKSIZE ; i ++)
                        sumOfDiffsSquared += diffsSquared[i];
                    
                    {
                        int16_t i;
                        for (i = BLOCKSIZE - 1 ; i >= 0 ; i--)
                        {
                            if (sumOfDiffsSquared <= noiseThreshold)
                            {
                                lags[channel] = LOOKBACK_BLOCKCOUNT*BLOCKSIZE - (- 1 - i + lookBackBlock*BLOCKSIZE);
                                break;
                            }
                            
                            sumOfDiffsSquared -= diffsSquared[i];
                            const int16_t& sample = blocks[channel][absLookBackBlock2][i];
                            const int32_t diff = sample - noiseMean;
                            sumOfDiffsSquared += (int32_t)diff*diff;
                        }
                        if (i == -1)
                            pingFound = false;
                    }
                    
                }
                
                if (pingFound)
                {
                    // TODO: report ping
                    
#ifdef DEBUG
                    static int pingCount = 0;
                    
                    float freq = (float)triggerHarmonic*SAMPFREQ/BLOCKSIZE*0.001f;
                    printf("signal at %02.3f kHz, block %d\n", freq, blockIndex);
                    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                        printf("   channel %d at lag %d\n", channel, lags[channel]);
                    printf("   TDOAS: %d %d %d\n", lags[1]-lags[0],lags[2]-lags[0],lags[3]-lags[0]);
                    
                    char fname[256];
                    sprintf(fname, "ping%d.out", pingCount++);
                    
                    FILE* fid = fopen(fname, "w");
                    if (fid == NULL)
                    {
                        perror("fopen");
                        exit(EXIT_FAILURE);
                    }
                    fwrite(lags, sizeof(lags), 1, fid);
                    for (int i = 0 ; i < LOOKBACK_BLOCKCOUNT ; i ++)
                    {
                        int absBlock = (savedBlockIndex + i + 1) % LOOKBACK_BLOCKCOUNT;
                        int16_t buf[BLOCKSIZE][NCHANNELS];
                        for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                            for (int j = 0 ; j < BLOCKSIZE ; j ++)
                                buf[j][channel] = blocks[channel][absBlock][j];
                        fwrite(buf, sizeof(buf), 1, fid);
                    }
                    fclose(fid);
#endif
                    
                }
                
                ++workIndex;
                
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
