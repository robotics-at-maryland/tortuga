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
#include <iostream>
#include <string.h>

SonarController::SonarController(int numSensors) {
	this->numSensors = numSensors;
	samprate = 1000000;
	targetfreq = 30000;
	bufferlength = samprate * PINGDURATION * 3;
	nearestperiod = round(samprate / targetfreq);
	numPeriods = 6;
	windowlength = nearestperiod * numPeriods;
	threshold = ((1 << (BITS_ADCCOEFF + 8 * sizeof(adcdata_t) - 2)) 
				 * windowlength) >> 3;
	setupCoefficients();
	setupWindow();
}

SonarController::~SonarController() {
	delete [] coefreal;
	delete [] coefimag;
	delete [] sumreal;
	delete [] sumimag;
	delete [] mag;
	delete [] insidePulse;
	for (int i = 0 ; i < numSensors ; i ++) {
		delete [] windowreal[i];
		delete [] windowimag[i];
	}
	delete [] windowreal;
	delete [] windowimag;
}

void SonarController::setupCoefficients() {
	coefreal = new adcmath_t[nearestperiod];
	coefimag = new adcmath_t[nearestperiod];
	int mag = 1 << (BITS_ADCCOEFF - 1);
	for (int i = 0 ; i < nearestperiod ; i++) {
		coefreal[i] = cosf(- 2 * M_PI * i / nearestperiod) * mag;
		coefimag[i] = sinf(- 2 * M_PI * i / nearestperiod) * mag;
	}
}

void SonarController::setupWindow() {
	buffer = new adcdata_t*[numSensors];
	windowreal = new adcmath_t*[numSensors];
	windowimag = new adcmath_t*[numSensors];
	sumreal = new adcmath_t[numSensors];
	sumimag = new adcmath_t[numSensors];
	mag = new adcmath_t[numSensors];
	insidePulse = new bool[numSensors];
	for (int i = 0 ; i < numSensors ; i ++) {
		buffer[i] = new adcdata_t[bufferlength];
		windowreal[i] = new adcmath_t[windowlength];
		windowimag[i] = new adcmath_t[windowlength];
	}
	purge();
}

void SonarController::purge() {
	memset(sumreal, 0, sizeof(*sumreal) * numSensors);
	memset(sumimag, 0, sizeof(*sumimag) * numSensors);
	memset(mag, 0, sizeof(*mag) * numSensors);
	memset(insidePulse, 0, sizeof(*insidePulse) * numSensors);
	for (int i = 0 ; i < numSensors ; i ++) {
		memset(windowreal[i], 0, sizeof(**windowreal) * windowlength);
		memset(windowimag[i], 0, sizeof(**windowimag) * windowlength);
		memset(buffer[i], 0, sizeof(**buffer) * bufferlength);
	}
	curidx = bufidx = 0;
}

void SonarController::receiveSample(adcdata_t *sample) {
	curidx = (curidx + 1) % windowlength;
	bufidx = (bufidx + 1) % bufferlength;
	int firstidx = (curidx + 1) % windowlength;
	int coefidx = curidx % nearestperiod;
	for (int i = 0 ; i < numSensors ; i ++) {
		windowreal[i][curidx] = coefreal[coefidx] * sample[i];
		windowimag[i][curidx] = coefimag[coefidx] * sample[i];
		sumreal[i] += windowreal[i][curidx] - windowreal[i][firstidx];
		sumimag[i] += windowimag[i][curidx] - windowimag[i][firstidx];
		mag[i] = abs(sumreal[i]) + abs(sumimag[i]);
		buffer[i][bufidx] = sample[i];
		if (insidePulse[i]) {
			if (buffer[i][bufidx] < threshold) {
				insidePulse[i] = false;
			}
		} else {
			if (buffer[i][bufidx] > threshold) {
				insidePulse[i] = true;
			}
		}
	}
}

adcmath_t SonarController::getMag(int channel) const {
	return mag[channel];
}

template<class T>
int8_t compare(T a, T b) {
	if (a > b)
		return 1;
	if (b < a)
		return -1;
	return 0;
}

template<class T>
int8_t sign(T a) {
	return compare(a, 0);
}
