/*
 *  SimpleSlidingDFT.h
 *  sonarController
 *
 *  Created by Leo Singer on 12/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef SIMPLESLIDINGDFT_H
#define SIMPLESLIDINGDFT_H


#include "Sonar.h"
#include "SlidingDFT.h"


namespace ram {
namespace sonar {


class SimpleSlidingDFT : public SlidingDFT {

public:
	SimpleSlidingDFT(int nchannels, int k, int N);
	~SimpleSlidingDFT();
	virtual void purge();
	virtual void update(adcdata_t * sample);
	virtual adcmath_t getMagL1(int channel) const;
	virtual adcmath_t getReal(int channel) const;
	virtual adcmath_t getImag(int channel) const;
	
private:
	void setupCoefficients();
	void setupWindow();
	
	adcdata_t *coefreal, *coefimag;
	adcmath_t **windowreal, **windowimag;
	adcmath_t *sumreal, *sumimag, *mag;
	
	int curidx;
};


} // namespace sonar
} // namespace ram


#endif
