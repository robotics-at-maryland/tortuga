/**
 * @file DSO.h
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 * Designed to mimic some of the behaviors of a digital storage oscilloscope
 * (DSO), this class detects pings and calculates their time delays on arrival
 * using cross correlation.
 *
 */


#ifndef _RAM_SONAR_DSO_H
#define _RAM_SONAR_DSO_H


#include "Sonar.h"
#include "SonarChunk.h"
#include "dft/TiledSlidingDFT.h"
#include "SampleDelay.h"


#include <vector>


namespace ram {
namespace sonar {


class DSO {
	
	public:
	DSO(int nchannels, int k, int N, int presample);
	~DSO();
	void update(adcdata_t *sample);
	
	//	Reconstruct the sample data from triggered, stored SonarChunks
	void reconstruct_adcdata(adcdata_t *out, adcsampleindex_t until) const;
	
	//	Sorry for the nondescript method name.  This method goes through 
	//  oldChunks and correlates SoundChunks that belong to a single ping event,
	//	then computes the necessary cross-correlations.
	void thinkAboutIt();
	
	private:
	bool *triggers;
	TiledSlidingDFT dft;
	SampleDelay delayQueue;
	int nchannels, k, N;
	adcmath_t threshold;
	std::vector<SonarChunk*> *oldChunks;
	SonarChunk **currentChunks;
	adcsampleindex_t sample_index;
	
	void localize(SonarChunk **);
	
};


} // namespace sonar
} // namespace ram


#endif
