/*
 *  SonarChunk.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef SONARCHUNK_H
#define SONARCHUNK_H

#include "Sonar.h"

class SonarChunk {
public:
	SonarChunk(adcsampleindex_t startIndex);
	~SonarChunk();
	adcdata_t *sample;
	int length;
	const static int maxlength = 2048;
	adcsampleindex_t startIndex;
	bool append(adcdata_t);
};

adcsampleindex_t timeOfMaxCrossCorrelation(const SonarChunk &a, 
										   const SonarChunk &b);

#endif