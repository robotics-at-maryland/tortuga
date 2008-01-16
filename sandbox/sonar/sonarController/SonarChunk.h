/*
 *  SonarChunk.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Time-stamped container for ping samples.
 *  Also includes cross-correlation algorithm.
 *
 */


#ifndef SONARCHUNK_H
#define SONARCHUNK_H


#include "Sonar.h"


#include <list>


namespace ram {
namespace sonar {


class SonarChunk {
	
public:
	SonarChunk();
	~SonarChunk();
	static SonarChunk *newInstance();
	
	const static int capacity;
	adcsampleindex_t startIndex;
	
	bool append(adcdata_t);
	int size() const;
	adcdata_t getPeak() const;
	const adcdata_t &operator[](adcsampleindex_t i) const;
	void setFourierComponents(adcmath_t re, adcmath_t img);
	adcmath_t getFourierAmpReal() const;
	adcmath_t getFourierAmpImag() const;
	float getPhase() const;
	
	void purge();
	void recycle();
	
	static void emptyPool();
	static void emptyPool(int numToRemain);
	
private:
	int length;
	adcdata_t *sample;
	adcdata_t peak;
	adcmath_t fourierAmpReal, fourierAmpImag;
	float phase;
	static std::list<SonarChunk*> pool;
	
};


} // namespace sonar
} // namespace ram


#endif