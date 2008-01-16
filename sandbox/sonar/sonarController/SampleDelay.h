/*
 *  SampleDelay.h
 *  sonarController
 *
 *  Created by Leo Singer on 1/8/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#ifndef SAMPLEDELAY_H
#define SAMPLEDELAY_H


#include "Sonar.h"


#include <stdlib.h>


namespace ram {
namespace sonar {


class SampleDelay {
public:
	SampleDelay(int nchannels, int numSamples);
	~SampleDelay();
	void writeSample(adcdata_t *);
	adcdata_t * readSample();
	void purge();
private:
	int buflen;
	adcdata_t *buf, *bufptr, *bufend;
	int increment;
};


} // namespace sonar
} // namespace ram


#endif
