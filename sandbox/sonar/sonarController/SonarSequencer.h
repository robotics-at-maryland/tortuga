/*
 *  SonarSequencer.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/27/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Store, catalog, and analyze chunks of acoustic data
 *
 */

#ifndef SONARSEQUENCER_H
#define SONARSEQUENCER_H

#include "Sonar.h"
#include "SonarChunk.h"

#include <list>

class SonarSequencer {
public:
	SonarSequencer(int numSensors);
	~SonarSequencer();
	SonarChunk *newChunk(int channel, adcsampleindex_t sampleIndex);
	bool addSample(int channel, adcdata_t datum, adcmath_t fourieramp);
private:
	int numSensors;
	std::list<SonarChunk> chunks;
	SonarChunk **currentChunks;
};

#endif
