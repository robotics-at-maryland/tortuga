/*
 *  SonarChunk.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Time-stamped container for ping samples.
 *  Also includes cross-correlation algorithm.
 *
 */


#ifndef SONARCHUNK_H
#define SONARCHUNK_H


#include "Sonar.h"


class SonarChunk {
	
public:
	SonarChunk(adcsampleindex_t startIndex);
	~SonarChunk();
	adcsampleindex_t startIndex;
	bool append(adcdata_t);
	int size() const;
	const static int capacity = 2048;
	adcdata_t getPeak() const;
	const adcdata_t &operator[](adcsampleindex_t i) const;
	void setFourierComponents(adcmath_t re, adcmath_t img);
	adcmath_t getFourierAmpReal() const;
	adcmath_t getFourierAmpImag() const;
	float getPhase() const;
	void purge();
	
private:
	int length;
	adcdata_t *sample;
	adcdata_t peak;
	adcmath_t fourierAmpReal, fourierAmpImag;
	float phase;
	
};


#endif