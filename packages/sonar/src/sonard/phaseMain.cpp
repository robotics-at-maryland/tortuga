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
//static const int kBandOffCenterAmount = 10;
//static const int nKBands = 3;		//	Number of frequency bands to examine
//static const int kBands[] = {kBandOfInterest, kBandOfInterest - kBandOffCenterAmount, kBandOfInterest + kBandOffCenterAmount};

static const float holdoffTime = .1;//	Holdoff until looking for next ping (seconds)
static const size_t holdoffSamples = (size_t) (holdoffTime * fs);// Number of samples to holdoff


static const double hydroStructureArray[3][3] =
{
    {0,      0.984, 0,   },
    {0.492,  0.492, 0.696},
    {-0.492, 0.492, 0.696}
};

const MatrixN hydroStructure (*hydroStructureArray, 3, 3);

bool elemsLessThan(int64_t *V, int len, int64_t thresh)
{
	bool lessThan = true;
	for (int i = 0 ; i < len ; i ++)
		if (V[i] >= thresh)
			lessThan = false;
	return lessThan;
}

int main(int argc, char *argv[])
{
	SparseSDFTSpectrum<adctype<16>, 300, nChannels, 1> spectrum(&kBandOfInterest);
	
	const static int nQuietSamplesNeeded = 300;
	int nQuietSamples = 0;
	int64_t silenceThreshold = 3000;
	int16_t samples[nChannels];
	bool magsQuiet[nQuietSamplesNeeded];
	bzero(magsQuiet, sizeof(bool) * nQuietSamplesNeeded);
	int bufIdx = 0;
	
	unsigned long sampleIndex = 0;
	unsigned long samplesFromPingStart = 0;
	
	long toas[nChannels];
	for (int channel = 0 ; channel < nChannels ; channel ++)
		toas[channel] = -1;
	
	unsigned long holdoffCounter = holdoffSamples;
	
	bool isLoud = true;
	
	bool pingMaybeStarted = false;
	
	while (fread(samples, sizeof(int16_t), 4, stdin))
	{
		++sampleIndex;
		
		++bufIdx;
		if (bufIdx >= nQuietSamplesNeeded)
			bufIdx = 0;
		
		spectrum.update(samples);
		
		nQuietSamples -= magsQuiet[bufIdx];
		
		int64_t mags[nChannels];
		for (int channel = 0 ; channel < nChannels ; channel ++)
			mags[channel] = fixed::magL1(spectrum.getAmplitudeForBinIndex(0, channel));
		
		magsQuiet[bufIdx] = elemsLessThan(mags, nChannels, silenceThreshold);
		nQuietSamples += magsQuiet[bufIdx];
		
		bool wasLoud = isLoud;
		isLoud = (nQuietSamples < nQuietSamplesNeeded);
		
		if (!isLoud)
		{
			samplesFromPingStart = 0;
			pingMaybeStarted = false;
			for (int channel = 0 ; channel < nChannels ; channel ++)
				toas[channel] = -1;
		}
		if (holdoffCounter <= holdoffSamples)
			++holdoffCounter;
		pingMaybeStarted = pingMaybeStarted || (!wasLoud && isLoud);
		if(pingMaybeStarted && isLoud && holdoffCounter > holdoffSamples)
		{
			bool pingDetected = true;
			for (int channel = 0 ; channel < nChannels ; channel ++)
			{
				if (toas[channel] == -1)
				{
					//std::cout << "magch" << channel << ": " << mags[channel] << std::endl;
					if (mags[channel] > silenceThreshold)
					{
						toas[channel] = samplesFromPingStart;
					}
					else
						pingDetected = false;
				}
			}
			++samplesFromPingStart;
			if (pingDetected)
			{
				std::cout << "Ping at " << sampleIndex << std::endl;
				holdoffCounter = 0;
				samplesFromPingStart = 0;
				for (int channel = 0 ; channel < nChannels ; channel ++)
					toas[channel] = -1;
			}
		}
	}
}
