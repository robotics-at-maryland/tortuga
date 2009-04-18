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

#include <queue>
#include <vector>
#include <stdio.h>

using namespace std;

static const unsigned int LOG2_BLOCKSIZE = 8;
static const unsigned int LOG2_NCHANNELS = 2;
static const unsigned int NCHANNELS = 4;
static const unsigned int BLOCKSIZE = 1 << LOG2_BLOCKSIZE;
static const unsigned int HOLDOFF_BLOCKCOUNT  = 128;
static const unsigned int LOOKBACK_BLOCKCOUNT = 16;
static const unsigned int STATRECORD_BLOCKCOUNT = 4096;
static const unsigned int NOISEFLOOR_BLOCKCOUNT = 256;
static const unsigned int NOISEFLOOR_WORKSIZE = 64;

struct BlockStatRecord {
    int16_t mean[NCHANNELS];
    uint32_t meanOfSquares[NCHANNELS];
    uint32_t averageVariance;
};

struct BlockStatRecordSynopsis {
    uint32_t averageVariance;
    BlockStatRecord* stat;
    
    BlockStatRecordSynopsis() : averageVariance(0), stat(NULL) {}
    
    BlockStatRecordSynopsis(const BlockStatRecordSynopsis& rhs) : averageVariance(rhs.averageVariance), stat(rhs.stat) {}
    
    BlockStatRecordSynopsis(BlockStatRecord& record) : averageVariance(record.averageVariance), stat(&record) {}
    
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
unsigned short noiseFloorWorkIndex = 0;
unsigned short noiseFloorWorkOffset = 0;
unsigned short savedBlockIndex = 0;

int16_t blocks[NCHANNELS][HOLDOFF_BLOCKCOUNT][BLOCKSIZE];

/**
 * Determine if a block can be read from the ADCs.
 * @return true if at least @c BLOCKSIZE samples are available to be read,
 *              false if otherwise
 */
bool isBlockAvailable()
{
    /// TODO: implement for Blackfin
    return true;
}

void readBlock(int blockIndex)
{
    /// TODO: implement for Blackfin
    int16_t temp[BLOCKSIZE][NCHANNELS];
    
    fread(*temp, sizeof(temp), 1, stdin);
    
    for (int i = 0 ; i < BLOCKSIZE ; i ++)
        for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
            blocks[channel][blockIndex][i] = temp[i][channel];
}

void waitBlockAvailable()
{
    while (!isBlockAvailable())
        ; // Busy wait
}

int main(int argc, char** argv)
{
    while (1)
    {
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
                    int16_t& sample = blocks[channel][blockIndex][i];
                    sum += sample;
                    sumOfSquares += (int32_t)sample*sample;
                }
                
                int16_t mean = sum >> LOG2_BLOCKSIZE;
                uint32_t meanOfSquares = sumOfSquares >> LOG2_BLOCKSIZE;
                
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
            // First step: determine if the average variance is at least 4 times
            // the average variance of the noise floor.
            if (stat.averageVariance > 4 * noiseFloor.averageVariance)
            {
                bool triggered;
                
                // TODO: evaluate spectral density check
                if (triggered)
                {
                    noiseFloorWorkIndex = 0;
                    noiseFloorWorkOffset = blockStatIndex;
                    savedBlockIndex = blockIndex;
                    holdoff = HOLDOFF_BLOCKCOUNT;
                }
            }
        }
        
        if (holdoff > 0)
        {
            if (noiseFloorWorkIndex == 0)
            {
                for ( ; noiseFloorWorkIndex < NOISEFLOOR_BLOCKCOUNT ; noiseFloorWorkIndex ++)
                    noiseFloorBlocks[noiseFloorWorkIndex] = BlockStatRecordSynopsis(blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + noiseFloorWorkOffset - noiseFloorWorkIndex) % STATRECORD_BLOCKCOUNT]);
                make_heap(noiseFloorBlocks.begin(), noiseFloorBlocks.end());
            } else if (noiseFloorWorkIndex < NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT) {
                do {
                    unsigned short lastNoiseFloorWorkIndex =
                    min(noiseFloorWorkIndex + NOISEFLOOR_WORKSIZE, NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT);
                    for ( ; noiseFloorWorkIndex < lastNoiseFloorWorkIndex ; noiseFloorWorkIndex ++)
                    {
                        BlockStatRecord& temp = blockStatRecordBuffer[(STATRECORD_BLOCKCOUNT + noiseFloorWorkOffset - noiseFloorWorkIndex) % STATRECORD_BLOCKCOUNT];
                        if (temp.averageVariance < noiseFloorBlocks.begin()->averageVariance)
                        {
                            pop_heap(noiseFloorBlocks.begin(), noiseFloorBlocks.end());
                            noiseFloorBlocks.back() = BlockStatRecordSynopsis(temp);
                            push_heap(noiseFloorBlocks.begin(), noiseFloorBlocks.end());
                        }
                    }
                } while (!isBlockAvailable());
            } else if (noiseFloorWorkIndex == NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT) {
                
                int32_t sumOfMeans[NCHANNELS];
                uint64_t sumOfMeansOfSquares[NCHANNELS];
                
                for (vector<BlockStatRecordSynopsis>::iterator it = noiseFloorBlocks.begin() ;
                     it != noiseFloorBlocks.end() ; ++it)
                {
                    bzero(sumOfMeans, sizeof(*sumOfMeans)*NCHANNELS);
                    bzero(sumOfMeansOfSquares, sizeof(*sumOfMeansOfSquares)*NCHANNELS);
                    
                    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                    {
                        BlockStatRecord& rec = *(it->stat);
                        sumOfMeans[channel] += rec.mean[channel];
                        sumOfMeansOfSquares[channel] += rec.meanOfSquares[channel];
                    }
                }
                
                for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                {
                    noiseFloor.mean[channel] = sumOfMeans[channel] >> LOG2_NCHANNELS;
                    noiseFloor.meanOfSquares[channel] = sumOfMeansOfSquares[channel] >> LOG2_NCHANNELS;
                }
                
                ++noiseFloorWorkIndex;
                
            } else if (noiseFloorWorkIndex == NOISEFLOOR_BLOCKCOUNT - HOLDOFF_BLOCKCOUNT + 1) {
                
                // TODO: look back through the list to find the rising edge, then report.
                for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                {
                    for (unsigned int lookBackBlock = 0 ; lookBackBlock < LOOKBACK_BLOCKCOUNT ; lookBackBlock ++)
                    {
                        
                    }
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
