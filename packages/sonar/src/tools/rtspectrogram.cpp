/**
 * @file rtspectrogram.cpp
 * Real time command line spectrogram
 *
 * @author Leo Singer
 * Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "spectrum/SparseSDFTSpectrum.h"
#include "Sonar.h"
#include "fixed/fixed.h"

using namespace ram::sonar;

static const int N = DFT_FRAME;
static const int numRows = 23;
static const int numCols = 70;
static const int zerothBand = 20;
static const char channelMark[] = "0123";

typedef adc<16> myadc;

int main(int argc, char *argv[])
{
	//long skip, begin, end;
    
    int myKBands[numRows];
    float freqs[numRows];
    myadc::QUADRUPLE_WIDE::SIGNED hist[NCHANNELS][numRows];
    bzero(*hist, sizeof(**hist) * NCHANNELS * numRows);
    for (int i = 0 ; i < numRows ; i ++)
    {
        myKBands[i] = zerothBand + i;
        freqs[i] = (float)myKBands[i] / DFT_FRAME * SAMPRATE / 1000;
    }
    
    {
        SparseSDFTSpectrum<myadc, N, NCHANNELS, numRows> spectrum(myKBands);
        myadc::SIGNED sample[NCHANNELS];
        while (fread(sample, sizeof(myadc::SIGNED), NCHANNELS, stdin) == (size_t)NCHANNELS)
        {
            //	Update spectrogram
            spectrum.update(sample);
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                for (int i = 0 ; i < numRows ; i ++)
                    hist[channel][i] += fixed::magL1(spectrum.getAmplitudeForBinIndex(i, channel));
        }
    }
    myadc::QUADRUPLE_WIDE::SIGNED histMax = 1;
    for (int i = 0 ; i < numRows ; i ++)
    {
        myadc::QUADRUPLE_WIDE::SIGNED histSum = 0;
        for (int channel = 0 ; channel < NCHANNELS ; channel ++)
            histSum += hist[channel][i];
        if (histSum > histMax)
            histMax = histSum;
    }
    for (int i = 0 ; i < numRows ; i ++)
    {
        std::cout << ' ' << std::setw(4) << std::setprecision(3) << freqs[i] << " |";
        for (int channel = 0 ; channel < NCHANNELS ; channel ++)
        {
            const int numMarks = (double)hist[channel][i] / histMax * numCols;
            const char markChar = channelMark[channel];
            for (int j = 0 ; j < numMarks; j ++)
                std::cout << markChar;
        }
        std::cout << std::endl;
    }
	return 0;
}
