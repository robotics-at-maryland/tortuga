/**
 * @file DesktopBlockDFT.cpp
 * @author Leo Singer
 *
 */


#include "DesktopBlockDFT.h"
#include <stdlib.h>

DesktopBlockDFT::DesktopBlockDFT()
{
    fftwPlan = fftw_plan_dft_r2c_1d(BLOCKSIZE, fftwIn, fftwOut, FFTW_PATIENT);
}

DesktopBlockDFT::~DesktopBlockDFT()
{
    fftw_destroy_plan(fftwPlan);
}

void DesktopBlockDFT::readBlock()
{
    // read block from stdin
    int16_t blockTemp[BLOCKSIZE][NCHANNELS];
    if (1 != fread(&blockTemp[0][0], sizeof(blockTemp), 1, stdin))
    {
        printf("--------------------\n");
        exit(0);
    }
    
    for (int channel = 0 ; channel < NCHANNELS ; channel ++)
        for (int i = 0 ; i < BLOCKSIZE ; i ++)
            blocks[channel][currentBlock][i] = blockTemp[i][channel];
}

void DesktopBlockDFT::doDFT(int channel)
{
    for (int i = 0 ; i < BLOCKSIZE ; i ++)
        fftwIn[i] = blocks[channel][currentBlock][i];
    
    fftw_execute(fftwPlan);
    
    for (int i = 0 ; i < BLOCKSIZE ; i ++)
    {
        dft[i].re = fftwOut[i][0];
        dft[i].im = fftwOut[i][1];
    }
}
