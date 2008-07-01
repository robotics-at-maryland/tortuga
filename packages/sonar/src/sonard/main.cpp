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
#include "fixed/fixed.h"
#include "math/include/MatrixN.h"
#include "math/include/Vector3.h"

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
	PulseTrigger<int64_t, 512, nChannels * nKBands> trigger;
	int64_t triggerVals[nChannels * nKBands];
	trigger.setThresholds(10000);
	
	adcdata_t sample[nChannels];
	
	size_t sampleIndex = 0;
	size_t samplesSinceLastPing = 0;

        // Read a single sample from each channel into the sample buffer "sample"
	while (fread(sample, sizeof(adcdata_t), nChannels, stdin) == (size_t)nChannels)
	{
		++sampleIndex;
		//	Update spectrogram
		spectrum.update(sample);
		
		for (int channel = 0 ; channel < nChannels ; channel ++)
			for (int kidx = 0 ; kidx < nKBands ; kidx ++)
				triggerVals[nChannels * kidx + channel]
				= fixed::magL1(spectrum.getAmplitudeForBinIndex(kidx, channel));
		trigger.update(triggerVals);

                // Determine is we have heard the ping
		bool pingDetected = true;
		if (samplesSinceLastPing < holdoffSamples)
			pingDetected = false;
		for (int channel = 0 ; channel < nChannels ; channel ++)
		{
			if (!trigger(nChannels * 0 + channel))
				pingDetected = false;
			if (trigger(nChannels * 1 + channel))
				pingDetected = false;
			if (trigger(nChannels * 2 + channel))
				pingDetected = false;
		}
		
		if (pingDetected)
		{
			// TODO: Break me into a class and test me
			samplesSinceLastPing = 0;
			const complex<int64_t> &ch0 = spectrum.getAmplitudeForBinIndex(0, 0);
			MatrixN tdoas(3, 1);
			for (int channel = 1 ; channel < nChannels ; channel ++)
			{
				const complex<int64_t> &ch = spectrum.getAmplitudeForBinIndex(0, channel);
				tdoas[channel - 1][0] = fixed::phaseBetween(ch0, ch);
			}
			tdoas[nChannels - 1][0] = 0;
			MatrixN direction = hydroStructure * tdoas;
			Vector3 directionVector;
			for (int i = 0 ; i < 3 ; i ++)
				directionVector[i] = direction[i][0];
			directionVector.normalise();
			cout << "Ping detected at sample " << sampleIndex << ":" << endl;
			cout << "Direction to pinger is: " << directionVector << endl;
		}
		else // !pingDetected
		{
			++samplesSinceLastPing;
		}
	}
	return 0;
}
