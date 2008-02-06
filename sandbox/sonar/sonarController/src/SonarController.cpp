/*
 *  SonarController.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/25/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "SonarController.h"


#include "TDOA.h"


#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


namespace ram {
namespace sonar {

		
void init(int ns)
{
	nchannels = ns;
	samprate = SAMPRATE;
	targetfreq = 30000;
	int nearestperiod = round(samprate / targetfreq);
	int numPeriods = 6;
	int windowlength = nearestperiod * numPeriods;
	
	dft = new CONTROLLER_DFT_IMPL(ns, numPeriods, windowlength);
	threshold = dft->getUnity() * 0.1;
	
	/**	Minimum number of samples to wait before listening for the next set of
	 *	pings.
	 */
	
	minSamplesBetweenPings = 
		(NOMINAL_PING_DELAY - ((float) MAXIMUM_SPEED * NOMINAL_PING_DELAY 
		+ 3 * MAX_SENSOR_SEPARATION)/ SPEED_OF_SOUND) * samprate;
	
	/** Maximum number of samples to listen past minSamplesBetweenPings before 
	 *	going back to sleep
	 */
	
	maxSamplesToWaitForFirstPing = 
		((float) 2 * MAXIMUM_SPEED * NOMINAL_PING_DELAY 
		 + 2 * MAX_SENSOR_SEPARATION) / SPEED_OF_SOUND * samprate;
	
	/** Maximum number of samples past the rising edge of the last receipt of 
	 *	the current ping to wait for the next rising edge on the next hydrophone
	 */
	
	maxSamplesTDOA = 1.25 * MAX_SENSOR_SEPARATION / SPEED_OF_SOUND * samprate;
	
	/** Set up buffers */
	
	setupWindow();
	
	sampleIndex = indexOfLastRisingEdge = indexOfLastWake = 0;
	sonarchannelstate = new sonarchannelstate_t[nchannels];
	
	
	sleepingChannelCount = ns;
	listeningChannelCount = captureChannelCount = 0;
	for (int i = 0 ; i < nchannels ; i ++)
		sonarchannelstate[i] = SONAR_CHANNEL_SLEEPING;
	sonarstate = SONAR_SLEEPING;
}


void destroy()
{
	delete [] sample;
	delete [] sonarchannelstate;
	delete [] currentChunks;
	SonarChunk::emptyPool();
}


void receiveSample(adcdata_t *newdata)
{
	sampleIndex ++;
	if (getState() == SONAR_SLEEPING)
		if (sleepTimeIsUp())
			wake();
	
	if (getState() == SONAR_LISTENING)
	{
		memcpy(sample, newdata, sizeof(*sample) * nchannels);
		dft->update(sample);
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			switch (getChannelState(channel))
			{
				case SONAR_CHANNEL_LISTENING:
					if (exceedsThreshold(channel))
					{
						startCapture(channel);
						captureSample(channel);
					}
					break;
				case SONAR_CHANNEL_CAPTURING:
					if (exceedsThreshold(channel))
						captureSample(channel);
					else
						stopCapture(channel);
					break;
			}
		}
		if (listeningChannelCount + captureChannelCount == 0 || 
			(captureChannelCount == 0 && listenTimeIsUp()))
			sleep();
	}
}


sonarstate_t getState()
{
	return sonarstate;
}


sonarchannelstate_t getChannelState(int channel)
{
	return sonarchannelstate[channel];
}


void go()
{
	wake();
}


namespace /* internal */ {

	
void setupWindow() {
	sample = new adcdata_t[nchannels];
	currentChunks = new SonarChunk*[nchannels];
	purge();
}


void purge()
{
	memset(sample, 0, sizeof(*sample) * nchannels);
}


bool sleepTimeIsUp()
{
	assert(getState() == SONAR_SLEEPING);
	return (sampleIndex - indexOfLastWake) > minSamplesBetweenPings;
}


bool listenTimeIsUp()
{
	assert(getState() == SONAR_LISTENING);
	if (listeningChannelCount == nchannels 
		&& (sampleIndex - indexOfLastWake) > maxSamplesToWaitForFirstPing)
		return true;
	else if (listeningChannelCount < nchannels && (sampleIndex - indexOfLastRisingEdge) > maxSamplesTDOA)
		return true;
	else
		return false;
}


void wake()
{
	assert(getState() == SONAR_SLEEPING);
	assert(printf("Wake up at sample %d\n",sampleIndex) || true);
	for (int channel = 0 ; channel < nchannels ; channel ++)
		wakeChannel(channel);
	indexOfLastWake = sampleIndex;
	sonarstate = SONAR_LISTENING;
}


void sleep()
{
	assert(getState() == SONAR_LISTENING);
	assert(printf("Sleep at sample %d\n",sampleIndex) || true);
	for (int channel = 0 ; channel < nchannels ; channel ++)
		stopCapture(channel);
	sonarstate = SONAR_SLEEPING;
}


bool exceedsThreshold(int channel)
{
	return dft->getMagL1(channel) > threshold;
}


void wakeChannel(int channel)
{
	assert(getChannelState(channel) == SONAR_CHANNEL_SLEEPING);
	assert(printf("Waking channel %d\n", channel) || true);
	sonarchannelstate[channel] = SONAR_CHANNEL_LISTENING;
	sleepingChannelCount --;
	listeningChannelCount ++;
}


void sleepChannel(int channel)
{
	stopCapture(channel);
}


void startCapture(int channel)
{
	assert(getChannelState(channel) == SONAR_CHANNEL_LISTENING);
	assert(printf("Starting capture on channel %d at sample %d\n", 
				  channel, sampleIndex) || true);
	currentChunks[channel] = SonarChunk::newInstance();
	currentChunks[channel]->startIndex = sampleIndex;
	sonarchannelstate[channel] = SONAR_CHANNEL_CAPTURING;
	listeningChannelCount --;
	captureChannelCount ++;
	indexOfLastRisingEdge = sampleIndex;
}


void stopCapture(int channel)
{
	switch (getChannelState(channel))
	{
		case SONAR_CHANNEL_CAPTURING:
			assert(printf("Sleeping channel %d with %d samples captured\n", 
						  channel, currentChunks[channel]->size()) || true);
			currentChunks[channel]->setFourierComponents(dft->getReal(channel), dft->getImag(channel));
			oldChunks.push_back(currentChunks[channel]);
			sonarchannelstate[channel] = SONAR_CHANNEL_SLEEPING;
			captureChannelCount --;
			sleepingChannelCount ++;
			break;
			
		case SONAR_CHANNEL_LISTENING:
			assert(printf("Sleeping channel %d with no samples captured\n", 
						  channel) || true);
			sonarchannelstate[channel] = SONAR_CHANNEL_SLEEPING;
			listeningChannelCount --;
			sleepingChannelCount ++;
			break;
	}
}


void captureSample(int channel)
{
	assert(getChannelState(channel) == SONAR_CHANNEL_CAPTURING);
	if (!currentChunks[channel]->append(sample[channel]))
	{
		assert(printf("Channel %d full\n", channel) || true);
		stopCapture(channel);
	}
}


void analyzeChunks()
{
	for (int i = 0 ; i < oldChunks.size() - 1 ; i ++)
	{
		for (int j = i + 1 ; j < oldChunks.size() ; j ++)
		{
			adcsampleindex_t tdoac = tdoa_xcorr(*oldChunks[i] , *oldChunks[j]);
			printf("TDOA between chunks %d and %d of %d samples\n", i, j, tdoac);
		}
	}
}


} // namespace /* internal */


} // namespace sonar
} // namespace ram
