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

static const double hydroStructureArray[4][4] = 
{
	{0,      0.984, 0,     0},
	{0.492,  0.492, 0.696, 0},
	{-0.492, 0.492, 0.696, 0},
	{0,      0,     0,     1}
};

const MatrixN hydroStructure (*hydroStructureArray, 4, 4);

int64_t myAbs(int64_t x)
{
	if (x < 0)
		return -x;
	else
		return x;
}

int main(int argc, char *argv[])
{
	SparseSDFTSpectrum<512, nChannels, nKBands> spectrum(kBands);
	PulseTrigger<int64_t, 512, nChannels * nKBands> trigger;
	int64_t triggerVals[nChannels * nKBands];
	trigger.setThresholds(10000);
	
	adcdata_t sample[nChannels];
	
	size_t sampleIndex = 0;
	size_t samplesSinceLastPing = 0;
	while (fread(sample, sizeof(adcdata_t), nChannels, stdin) == (size_t)nChannels)
	{
		++sampleIndex;
		//	Update spectrogram
		spectrum.update(sample);
		
		for (int channel = 0 ; channel < nChannels ; channel ++)
		{
			for (int kidx = 0 ; kidx < nKBands ; kidx ++)
			{
				const complex<int64_t> &cmplx = spectrum.getAmplitudeForBinIndex(kidx, channel);
				int64_t L1 = (myAbs(cmplx.real()) + myAbs(cmplx.imag()));
				triggerVals[nChannels * kidx + channel] = L1;
			}
		}
		trigger.update(triggerVals);
		
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
			samplesSinceLastPing = 0;
			const complex<int64_t> &ch0 = spectrum.getAmplitudeForBinIndex(0, 0);
			complex<double> ch0Double(ch0.real(), ch0.imag());
			MatrixN tdoas(4, 1);
			for (int channel = 1 ; channel < nChannels ; channel ++)
			{
				const complex<int64_t> &ch = spectrum.getAmplitudeForBinIndex(0, channel);
				complex<double> chDouble(ch.real(), ch.imag());
				tdoas[channel - 1][0] = arg(chDouble);
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
