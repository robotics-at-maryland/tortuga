/*
 *  SonarController.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/25/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Acquire samples from ADCs, compute a single-band, sliding DFT, and 
 *  detect edges.
 *
 */


#ifndef SONARCONTROLLER_H
#define SONARCONTROLLER_H


#include <stdint.h>
#include <list>


#include "Sonar.h"
#include "SonarChunk.h"


typedef enum sonarstate_e {
	SONAR_INIT, SONAR_DISABLED, SONAR_TRAINING, SONAR_SLEEPING, SONAR_LISTENING, SONAR_IDLE}
sonarstate_t;


typedef enum sonarchannelstate_e {
	SONAR_CHANNEL_SLEEPING, SONAR_CHANNEL_LISTENING, SONAR_CHANNEL_CAPTURING}
sonarchannelstate_t;


class SonarController
{
	
public:
	SonarController(int numSensors);
	~SonarController();
	void receiveSample(adcdata_t*);
	adcmath_t getMag(int channel) const;
	sonarstate_t getState() const;
	sonarchannelstate_t getChannelState(int channel) const;
	
private:
	void setstate(sonarstate_t);
	void setupCoefficients();
	void setupWindow();
	void purge();
	
	void updateSlidingDFT();
	bool listenTimeIsUp() const;
	void goToSleep();
	bool exceedsThreshold(int channel) const;
	void startCapture(int channel);
	void stopCapture(int channel);
	void captureSample(int channel);
	
	void analyzeChunks();
	
	adcmath_t *coefreal, *coefimag;
	adcmath_t **windowreal, **windowimag;
	adcmath_t *sumreal, *sumimag, *mag;
	adcdata_t *sample; 
	SonarChunk **currentChunks;
	std::list<SonarChunk> oldChunks;
	
	adcmath_t threshold;
	int curidx;
	int bufidx;
	int numSensors;
	int numPeriods;
	int windowlength, nearestperiod;
	int samprate;
	int targetfreq;
	adcsampleindex_t sampleCount;
	sonarstate_t sonarstate;
	sonarchannelstate_t  *sonarchannelstate;
	
};


#endif
