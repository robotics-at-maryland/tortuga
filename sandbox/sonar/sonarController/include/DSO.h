/*
 *  DSO.h
 *  sonarController
 *
 *  Created by Leo Singer on 1/18/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#ifndef DSO_H
#define DSO_H


#include "Sonar.h"
#include "SonarChunk.h"
#include "SimpleSlidingDFT.h"
#include "SampleDelay.h"


#include <vector>


namespace ram {
namespace sonar {


class DSO {
	
	public:
	DSO(int nchannels, int k, int N, int presample);
	~DSO();
	void update(adcdata_t *sample);
	void reconstruct_adcdata(adcdata_t *out, adcsampleindex_t until) const;
	
	private:
	bool *triggers;
	SimpleSlidingDFT dft;
	SampleDelay delayQueue;
	int nchannels, k, N;
	adcmath_t threshold;
	std::vector<SonarChunk*> *oldChunks;
	SonarChunk **currentChunks;
	adcsampleindex_t sample_index;
	
};


} // namespace sonar
} // namespace ram


#endif
