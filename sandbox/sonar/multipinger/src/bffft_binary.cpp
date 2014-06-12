/**
 * @file bffft_binary.cpp
 * BlackFin FFT
 * or Best Freakin' Friends Forever, Totally
 *
 */

#include "fft.h"

#include <stdio.h>
#include <stdint.h>

#define BLOCKSIZE   512
#define NCHANNELS   4

ram::sonar::fft_fract16<BLOCKSIZE> fft;

static fract16 dftIn[BLOCKSIZE];
static fract16 readIn[BLOCKSIZE][NCHANNELS];
static uint64_t totalPower[BLOCKSIZE];

int main(int argc, char* argv[])
{
    while (1 == fread(*readIn, sizeof(readIn), 1, stdin))
    {
        for (int channel = 0 ; channel < NCHANNELS ; channel ++)
        {
            for (int i = 0 ; i < BLOCKSIZE ; i ++)
                dftIn[i] = readIn[i][channel];
            
            fft.doDFT(dftIn);
            
            for (int k = 0 ; k < BLOCKSIZE ; k ++)
                totalPower[k] += (uint64_t)((int32_t)fft.dftOut[k].re*fft.dftOut[k].re)
                               + (uint64_t)((int32_t)fft.dftOut[k].im*fft.dftOut[k].im);
        }
    }
    
    fwrite(totalPower, sizeof(*totalPower), BLOCKSIZE, stdout);
    
    return 0;
}
