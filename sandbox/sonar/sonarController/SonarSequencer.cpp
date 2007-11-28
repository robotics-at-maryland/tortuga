/*
 *  SonarSequencer.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/27/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include "SonarSequencer.h"

SonarSequencer::SonarSequencer(int ns) {
	numSensors = ns;
	currentChunks = new SonarChunk*[numSensors];
	for (int i = 0 ; i < numSensors ; i ++)
		newChunk(i, 0);
}

SonarSequencer::~SonarSequencer() {
	delete [] currentChunks;
}

SonarChunk *SonarSequencer::newChunk(int channel,adcsampleindex_t sampleIndex) {
	currentChunks[channel] = new SonarChunk(sampleIndex);
	chunks.push_back(*currentChunks[channel]);
	return currentChunks[channel];
}

bool addSample(int channel, adcdata_t datum, adcmath_t fourieramp) {
	//	insert code here
    return true;
}
