/**
 * @file BlockDFT.h
 * @author Leo Singer
 *
 */

#ifndef _RAM_SONAR_BLOCKDFT_H
#define _RAM_SONAR_BLOCKDFT_H

#include <stdint.h>

enum
{
    LOG2_BLOCKSIZE = 8,
    BLOCKSIZE = 2 << (LOG2_BLOCKSIZE - 1), // 256
    BLOCKLOOKBACK = 8,
    NCHANNELS = 4,
    SAMPFREQ = 500000,
    
    LOOKBACKSAMPLES = BLOCKLOOKBACK * BLOCKSIZE,
    
    HARMONIC_MIN = 10,
    HARMONIC_MAX = 20,
    HARMONIC_COUNT = HARMONIC_MAX - HARMONIC_MIN,
    
    HOLDOFF_BLOCKCOUNT = (int)(0.1 * SAMPFREQ / BLOCKSIZE), // 100 millisecond holdoff
    QUIET_THRESH = 150
    
};

typedef struct {
    int16_t re, im;
} complex_int16_t;

class BlockDFT {
protected:
    int16_t blocks[NCHANNELS][BLOCKLOOKBACK][BLOCKSIZE];
    complex_int16_t dft[BLOCKSIZE];
    int currentBlock;
    unsigned long blockIndex;
    
    virtual void doDFT(int channel) =0;
    virtual void readBlock() =0;
private:
    uint16_t power[BLOCKSIZE];
    int16_t holdoff;
public:
    void processBlock();
    BlockDFT();
    virtual ~BlockDFT();
    virtual void purge();
};

#endif
