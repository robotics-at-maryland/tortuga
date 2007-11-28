/*
 *  SonarController.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/25/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef SONARCONTROLLER_H
#define SONARCONTROLLER_H

#include <stdint.h>

#include "Sonar.h"
#include "SonarSequencer.h"

class SonarController {
public:
	SonarController(int numSensors);
	~SonarController();
	void receiveSample(adcdata_t*);
	adcmath_t getMag(int channel) const;
	SonarSequencer sc;
private:
	void setupCoefficients();
	void setupWindow();
	void purge();
	adcmath_t *coefreal, *coefimag;
	adcmath_t **windowreal, **windowimag;
	adcmath_t *sumreal, *sumimag, *mag;
	adcdata_t **buffer;
	adcmath_t threshold;
	bool *insidePulse;
	int *riseEdgeIndices;
	int curidx;
	int bufidx;
	int numSensors;
	int numPeriods;
	int windowlength, nearestperiod;
	int bufferlength;
	int samprate;
	int targetfreq;
};

template<class T>
int8_t compare(T a, T b);

template<class T>
int8_t sign(T);

#endif
