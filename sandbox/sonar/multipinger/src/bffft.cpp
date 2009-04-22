/**
 * @file bffft.cpp
 * BlackFin FFT
 * or Best Freakin' Friends Forever, Totally
 *
 */

#include "fft.h"

#include <stdio.h>
#include <stdint.h>

#define BLOCKSIZE   512
#define NCHANNELS   4
#define NROWS       23
#define NCOLS       80

ram::sonar::fft_fract16<BLOCKSIZE> fft;

static fract16 dftIn[BLOCKSIZE];
static fract16 readIn[NCHANNELS][BLOCKSIZE];
static uint64_t totalPower[BLOCKSIZE];
static uint8_t hist[BLOCKSIZE];

int main(int argc, char* argv[])
{
    printf("Computing fft of channel 0 from stdin...\n");
    
    while (1 == fread(*readIn, sizeof(readIn), 1, stdin))
    {
        for (int i = 0 ; i < BLOCKSIZE ; i ++)
            dftIn[i] = readIn[0][i];
        
        fft.doDFT(dftIn);
        
        for (int k = 0 ; k < BLOCKSIZE ; k ++)
            totalPower[k] += (uint64_t)((int32_t)fft.dftOut[k].re*fft.dftOut[k].re)
                           + (uint64_t)((int32_t)fft.dftOut[k].im*fft.dftOut[k].im);
        
    }
    
    uint64_t maxPower = totalPower[1];
    for (int k = 2 ; k < NCOLS+1 ; k ++)
        if (totalPower[k] > maxPower)
            maxPower = totalPower[k];
    
    for (int k = 1 ; k < NCOLS+1 ; k ++)
        hist[k] = NROWS * totalPower[k] / maxPower;
    
    for (int row = 0 ; row < NROWS ; row ++)
    {
        for (int k = 1 ; k < NCOLS+1 ; k ++)
            putchar(hist[k] >= row ? '=' : ' ');
        putchar('\n');
    }
    
    return 0;
}
