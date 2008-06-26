/**
 * @file SonarChunk.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Time-stamped container for ping samples.
 * Also includes cross-correlation algorithm.
 *
 */


#ifndef _RAM_SONAR_SONARCHUNK_H
#define _RAM_SONAR_SONARCHUNK_H


#include "Sonar.h"
#include <list>


namespace ram {
namespace sonar {

/**
 * Provides long-term storage of sound sample data.
 * 
 * When you are done with a SonarChunk instance, recycle() rather than delete 
 * it.  recycle() reinitializes all the fields in the instance, returning it to
 * the clean state it was in when it was first created.  The clean instance is 
 * not deallocated but rather added to a pool of empty SonarChunks.
 * 
 * When you need a new SonarChunk instance, using newInstance() will draw a 
 * recycled SonarChunk from the pool rather than allocating a new one.
 */
class SonarChunk {
	
public:
	static SonarChunk *newInstance();
	
	const static adcsampleindex_t capacity;
	adcsampleindex_t startIndex;
	
	bool append(adcdata_t);
	adcsampleindex_t size() const;
	adcdata_t getPeak() const;
	const adcdata_t &operator[](adcsampleindex_t i) const;
	void setFourierComponents(adcmath_t re, adcmath_t img);
	adcmath_t getFourierAmpReal() const;
	adcmath_t getFourierAmpImag() const;
	float getPhase() const;
	
	void purge();
	void recycle();
	
	static void emptyPool();
	static void emptyPool(adcsampleindex_t numToRemain);
	
private:
	SonarChunk();
	~SonarChunk();
	adcsampleindex_t length;
	adcdata_t *sample;
	adcdata_t peak;
	adcmath_t fourierAmpReal, fourierAmpImag;
	float phase;
	static std::list<SonarChunk*> pool;
	
};


} // namespace sonar
} // namespace ram


#endif
