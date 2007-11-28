/*
 *  SonarSequencer.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/27/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef SONARSEQUENCER_H
#define SONARSEQUENCER_H

#include "Sonar.h"
#include "SonarChunk.h"

#include <list>

class SonarSequencer {
public:
	SonarSequencer();
	~SonarSequencer();
	SonarChunk &newChunk(adcsampleindex_t sampleIndex);
private:
	std::list<SonarChunk> chunks;
};

#endif
