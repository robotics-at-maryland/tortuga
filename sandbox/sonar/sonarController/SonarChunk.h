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
	const static int capacity = 2048;
	adcsampleindex_t startIndex;
	bool append(adcdata_t);
	int size() const;
	adcdata_t getPeak() const;
	const adcdata_t &SonarChunk::operator[](adcsampleindex_t i) const;
	void purge();
private:
	int length;
	adcdata_t *sample;
	adcdata_t peak;
};

adcsampleindex_t timeOfMaxCrossCorrelation(const SonarChunk &a, 
										   const SonarChunk &b);

#endif