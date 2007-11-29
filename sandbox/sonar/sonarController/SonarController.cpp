/*
 *  SonarController.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/25/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "SonarController.h"


#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


SonarController::SonarController(int ns) : oldChunks()
{
	numSensors = ns;
	samprate = 1000000;
	targetfreq = 30000;
	nearestperiod = round(samprate / targetfreq);
	numPeriods = 6;
	windowlength = nearestperiod * numPeriods;
	threshold = ((1 << (BITS_ADCCOEFF + 8 * sizeof(adcdata_t) - 2)) 
				 * windowlength) >> 3;
	maxSamplesTDOA = 1.25 * MAX_TDOA * samprate;
	setupCoefficients();
	setupWindow();
	sampleCount = indexOfLastPing = 0;
	sonarchannelstate = new sonarchannelstate_t[numSensors];
	for (int i = 0 ; i < numSensors ; i ++)
		sonarchannelstate[i] = SONAR_CHANNEL_LISTENING;
	sonarstate = SONAR_LISTENING;
}


SonarController::~SonarController()
{
	delete [] coefreal;
	delete [] coefimag;
	delete [] sumreal;
	delete [] sumimag;
	delete [] mag;
	delete [] sample;
	for (int i = 0 ; i < numSensors ; i ++)
	{
		delete [] windowreal[i];
		delete [] windowimag[i];
	}
	delete [] windowreal;
	delete [] windowimag;
	delete [] sonarchannelstate;
	delete [] currentChunks;
}


void SonarController::setupCoefficients()
{
	coefreal = new adcmath_t[nearestperiod];
	coefimag = new adcmath_t[nearestperiod];
	int mag = 1 << (BITS_ADCCOEFF - 1);
	for (int i = 0 ; i < nearestperiod ; i++)
	{
		coefreal[i] = cosf(- 2 * M_PI * i / nearestperiod) * mag;
		coefimag[i] = sinf(- 2 * M_PI * i / nearestperiod) * mag;
	}
}


void SonarController::setupWindow() {
	windowreal = new adcmath_t*[numSensors];
	windowimag = new adcmath_t*[numSensors];
	sumreal = new adcmath_t[numSensors];
	sumimag = new adcmath_t[numSensors];
	mag = new adcmath_t[numSensors];
	sample = new adcdata_t[numSensors];
	for (int i = 0 ; i < numSensors ; i ++)
	{
		windowreal[i] = new adcmath_t[windowlength];
		windowimag[i] = new adcmath_t[windowlength];
	}
	currentChunks = new SonarChunk*[numSensors];
	purge();
}


void SonarController::purge()
{
	memset(sumreal, 0, sizeof(*sumreal) * numSensors);
	memset(sumimag, 0, sizeof(*sumimag) * numSensors);
	memset(mag, 0, sizeof(*mag) * numSensors);
	memset(sample, 0, sizeof(*sample) * numSensors);
	for (int i = 0 ; i < numSensors ; i ++)
	{
		memset(windowreal[i], 0, sizeof(**windowreal) * windowlength);
		memset(windowimag[i], 0, sizeof(**windowimag) * windowlength);
	}
	curidx = 0;
}


void SonarController::receiveSample(adcdata_t *newdata)
{
	sampleCount ++;
	if (getState() == SONAR_LISTENING)
	{
		memcpy(sample, newdata, sizeof(*sample) *  numSensors);
		int awakeChannelCount = 0, captureChannelCount = 0;
		updateSlidingDFT();
		for (int channel = 0 ; channel < numSensors ; channel ++)
		{
			switch (getChannelState(channel))
			{
				case SONAR_CHANNEL_LISTENING:
					if (exceedsThreshold(channel))
					{
						startCapture(channel);
						captureSample(channel);
						captureChannelCount ++;
					}
					awakeChannelCount ++;
					break;
				case SONAR_CHANNEL_CAPTURING:
					if (exceedsThreshold(channel))
					{
						captureSample(channel);
						awakeChannelCount ++;
						captureChannelCount ++;
					}
					else
					{
						stopCapture(channel);
					}
					break;
			}
		}
		if (awakeChannelCount == 0 || (captureChannelCount == 0 && listenTimeIsUp()))
		{
			goToSleep();
		}
	}
}


void SonarController::updateSlidingDFT()
{
	curidx = (curidx + 1) % windowlength;
	int coefidx = curidx % nearestperiod;
	int firstidx = (curidx + 1) % windowlength;
	for (int i = 0 ; i < numSensors ; i ++)
	{
		windowreal[i][curidx] = coefreal[coefidx] * sample[i];
		windowimag[i][curidx] = coefimag[coefidx] * sample[i];
		sumreal[i] += windowreal[i][curidx] - windowreal[i][firstidx];
		sumimag[i] += windowimag[i][curidx] - windowimag[i][firstidx];
		mag[i] = abs(sumreal[i]) + abs(sumimag[i]);
	}
}


bool SonarController::listenTimeIsUp() const
{
	return (sampleCount - indexOfLastPing) > maxSamplesTDOA;
}


void SonarController::goToSleep()
{
	assert(printf("Sleep\n") || true);
	for (int channel = 0 ; channel < numSensors ; channel ++)
	{
		stopCapture(channel);
	}
	sonarstate = SONAR_SLEEPING;
	analyzeChunks();
}


bool SonarController::exceedsThreshold(int channel) const
{
	return mag[channel] > threshold;
}


void SonarController::startCapture(int channel)
{
	assert(getChannelState(channel) == SONAR_CHANNEL_LISTENING);
	assert(printf("Starting capture on channel %d\n", channel) || true);
	currentChunks[channel] = new SonarChunk(sampleCount);
	sonarchannelstate[channel] = SONAR_CHANNEL_CAPTURING;
	indexOfLastPing = sampleCount;
}


void SonarController::stopCapture(int channel)
{
	if (getChannelState(channel) == SONAR_CHANNEL_CAPTURING)
	{
		assert(printf("Stopping capture on channel %d\n", channel) || true);
		oldChunks.push_back(*currentChunks[channel]);
		sonarchannelstate[channel] = SONAR_CHANNEL_SLEEPING;
	}
}


void SonarController::captureSample(int channel)
{
	assert(getChannelState(channel) == SONAR_CHANNEL_CAPTURING);
	if (!currentChunks[channel]->append(sample[channel]))
		stopCapture(channel);
}


void SonarController::analyzeChunks()
{
	for (int i = 0 ; i < oldChunks.size() - 1 ; i ++)
	{
		for (int j = i + 1 ; j < oldChunks.size() ; j ++)
		{
			adcsampleindex_t tomc = timeOfMaxCrossCorrelation(oldChunks[i] , oldChunks[j]);
			printf("TDOA between chunks %d and %d of %d samples\n", i, j, tomc);
		}
	}
}


adcmath_t SonarController::getMag(int channel) const
{
	return mag[channel];
}


sonarstate_t SonarController::getState() const
{
	return sonarstate;
}


sonarchannelstate_t SonarController::getChannelState(int channel) const
{
	return sonarchannelstate[channel];
}
