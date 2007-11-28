/*
 *  SonarSequencer.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/27/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include "SonarSequencer.h"

SonarSequencer::SonarSequencer() {
}

SonarSequencer::~SonarSequencer() {
}

SonarChunk &SonarSequencer::newChunk(adcsampleindex_t sampleIndex) {
	SonarChunk *sc = new SonarChunk(sampleIndex);
	chunks.push_back(*sc);
	return *sc;
}
