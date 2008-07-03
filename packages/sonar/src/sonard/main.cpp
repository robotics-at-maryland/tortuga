/**
 * @file main.cpp
 *
 * @author Leo Singer
 * Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include <stdio.h>
#include <cmath>
#include <iostream>

#include "spectrum/SparseSDFTSpectrum.h"
#include "PulseTrigger.h"
#include "math/include/MatrixN.h"
#include "math/include/Vector3.h"

#include "fixed/fixed.h"

#include "dataset.h"
#include "spartan.h"
#include "trigger.h"

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

static const float c = 1500;		//	Speed of sound (meters per second)
static const float fs = 500000;		//	Sample rate (samples per second)
static const int N = 512;			//	Size of Fourier window
static const int nChannels = 4;		//	Number of hydrophones

static const float frequencyOfInterest = 25000;
static const int kBandOfInterest = (int) (frequencyOfInterest / fs * N);
static const int kBandOffCenterAmount = 10;
static const int nKBands = 3;		//	Number of frequency bands to examine
static const int kBands[] = {kBandOfInterest, kBandOfInterest - kBandOffCenterAmount, kBandOfInterest + kBandOffCenterAmount};

static const float holdoffTime = .1;//	Holdoff until looking for next ping (seconds)
static const size_t holdoffSamples = (size_t) (holdoffTime * fs);// Number of samples to holdoff


static const double hydroStructureArray[3][3] =
{
    {0,      0.984, 0,   },
    {0.492,  0.492, 0.696},
    {-0.492, 0.492, 0.696}
};

const MatrixN hydroStructure (*hydroStructureArray, 3, 3);

int main(int argc, char *argv[])
{
    // Does the sliding DFT on the incoming
    SparseSDFTSpectrum<512, nChannels, nKBands> spectrum(kBands);
    // Does threshold based triggering on the results of the DFT
    adcdata_t sample[nChannels];

    //size_t sampleIndex = 0;
    //size_t samplesSinceLastPing = 0;

    struct dataset * dataSet = NULL;

    if(argc == 1)
    {
        dataSet = createDataset(0xA0000);
        if(dataSet == NULL)
        {
            fprintf(stderr, "Could not allocate.\n");
            exit(1);
        }
        REG(ADDR_LED) = 0x02;
        fprintf(stderr, "Recording samples...\n");
        captureSamples(dataSet);
        fprintf(stderr, "Analyzing samples...\n");
        REG(ADDR_LED) = 0x01;
    } else
    {
        fprintf(stderr, "Using dataset %s\n", argv[1]);
        dataSet = loadDataset(argv[1]);
    }

    if(dataSet == NULL)
    {
        fprintf(stderr, "Could not load dataset!\n");
        return -1;
    }


    int64_t maxL1 = 0;
    int peakIndex = 0;

    for(int i=0; i<dataSet->size; i++)
    {

        sample[0] = getSample(dataSet, 0, i);
        sample[1] = getSample(dataSet, 1, i);
        sample[2] = getSample(dataSet, 2, i);
        sample[3] = getSample(dataSet, 3, i);

        //  Update spectrogram
        spectrum.update(sample);

        int channel = 0;

        for (int kidx = 0 ; kidx < nKBands ; kidx ++)
        {
            int64_t L1 = (int64_t)fixed::magL1(spectrum.getAmplitudeForBinIndex(1, channel));

            if(L1 > maxL1)
            {
                maxL1 = L1;
                peakIndex = i;
            }
        }
    }

    cerr << "Found DFT peak at sample " << peakIndex << endl;

    int pingStart = 0;

    pingStart = blockTrigger(dataSet, 0, peakIndex) + 20;

    if(pingStart == -1)
        return -1;

    spectrum.purge();

    SparseSDFTSpectrum<128, nChannels, nKBands> spectrum2(kBands);

    cerr << "Feeding sample " << pingStart << "into direction finder" << endl;

    for(int i=pingStart; i<pingStart+128; i++)
    {
        sample[0] = getSample(dataSet, 0, i);
        sample[1] = getSample(dataSet, 1, i);
        sample[2] = getSample(dataSet, 2, i);
        sample[3] = getSample(dataSet, 3, i);
        spectrum.update(sample);
        spectrum2.update(sample);
    }

    const complex<int64_t> &ch0 = spectrum2.getAmplitudeForBinIndex(0, 0);
    MatrixN tdoas(3, 1);
    for (int channel = 1 ; channel < nChannels ; channel ++)
    {
        const complex<int64_t> &ch = spectrum2.getAmplitudeForBinIndex(0, channel);
        tdoas[channel - 1][0] = fixed::phaseBetween(ch0, ch);
    }
    MatrixN direction = hydroStructure * tdoas;
    Vector3 directionVector;

    for (int i = 0 ; i < 3 ; i ++)
        directionVector[i] = direction[i][0];

    directionVector.normalise();
    cerr << "Direction to pinger is: " << directionVector << endl;

    float yaw = 180/M_PI*atan2(directionVector.y, directionVector.x);
	
    cerr << "Yaw is "<<yaw<<endl;

    cerr << "Sending samples...";

    for(int i=pingStart; i<pingStart+128; i++)
    {
        for(int j=0; j<4; j++)
        {
            putchar(getSample(dataSet, j, i) & 0xFF);
            putchar((getSample(dataSet, j, i) >> 8) & 0xFF);

//             putchar(getSample(dataSet, j, i) & 0xFF);
//             putchar((getSample(dataSet, j, i) >> 8) && 0xFF);
        }
    }
    cerr << "Done" << endl;


    return 0;
}
