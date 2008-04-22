/*
 *  SampleDelay.h
 *  sonarController
 *
 *  Created by Leo Singer on 1/8/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *  
 *  Circular queue for imposing a fixed-length delay on a time series.
 *  The queue advances on writes but not on reads.  This ensures that multiple
 *  reads will not distort the time delay.
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
	size_t increment;
};


} // namespace sonar
} // namespace ram


#endif
