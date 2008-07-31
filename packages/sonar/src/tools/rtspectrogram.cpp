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

#include "drivers/bfin_spartan/include/dataset.h"
#include "drivers/bfin_spartan/include/spartan.h"

using namespace ram::sonar;

static const int N = DFT_FRAME;
static const int numRows = 23;
static const int numCols = 70;
static const int zerothBand = 20;
static const char channelMark[] = "0123";

typedef adc<16> myadc;

int grabSamples(struct dataset* data, myadc::SIGNED* samples, int i)
{
    for (int channel = 0 ; channel < NCHANNELS ; channel ++)
    {
        myadc::SIGNED result = getSample(data, channel, i);
        if (result == -1)
            return false;
        else
            samples[channel] = result;
    }
    return true;
}

int main(int argc, char *argv[])
{
    bool do_loop = false;
    struct dataset* data;
    if (argc == 2)
    {
        data = loadDataset(argv[1]);
    } else {
        data = createDataset(0xA0000);
        captureSamples(data);
    }
    
    int myKBands[numRows];
    float freqs[numRows];
    myadc::QUADRUPLE_WIDE::SIGNED hist[NCHANNELS][numRows];
    bzero(*hist, sizeof(**hist) * NCHANNELS * numRows);
    
    int sampleCount = 0;
    for (int i = 0 ; i < numRows ; i ++)
    {
        myKBands[i] = zerothBand + i;
        freqs[i] = (float)myKBands[i] / DFT_FRAME * SAMPRATE / 1000;
    }
    
    do
    {
        SparseSDFTSpectrum<myadc, N, NCHANNELS, numRows> spectrum(myKBands);
        myadc::SIGNED sample[NCHANNELS];
        while (grabSamples(data, sample, sampleCount))
        {
            ++sampleCount;
            //	Update spectrogram
            spectrum.update(sample);
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                for (int i = 0 ; i < numRows ; i ++)
                    hist[channel][i] += fixed::magL1(spectrum.getAmplitudeForBinIndex(i, channel));
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
                const int numMarks = (int) ((double)hist[channel][i] / histMax * numCols);
                const char markChar = channelMark[channel];
                for (int j = 0 ; j < numMarks; j ++)
                    std::cout << markChar;
            }
            std::cout << std::endl;
        }
        
        if (do_loop)
        {
            captureSamples(data);
            spectrum.purge();
        }
    } while (do_loop);
    destroyDataset(data);
    return 0;
}
