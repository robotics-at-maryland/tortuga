/*
 *  SonarController.h
 *  sonarController
 *
 *  Created by Lady 3Jane Marie-France Tessier-Ashpool on 11/25/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SONARCONTROLLER_H
#define SONARCONTROLLER_H

#include <stdint.h>

typedef int8_t adcdata_t;
typedef int32_t adcmath_t;

#define BITS_ADCCOEFF 8
#define PINGDURATION 1.3e-3

class SonarController {
public:
	SonarController(int numSensors);
	~SonarController();
	void receiveSample(adcdata_t*);
	adcmath_t getMag(int channel) const;
private:
	void setupCoefficients();
	void setupWindow();
	void purge();
	adcmath_t *coefreal, *coefimag;
	adcmath_t **windowreal, **windowimag;
	adcmath_t *sumreal, *sumimag, *mag;
	adcdata_t **buffer;
	int curidx;
	int bufidx;
	int numSensors;
	int numPeriods;
	int windowlength, nearestperiod;
	int bufferlength;
	int samprate;
	int targetfreq;
};

template<class T>
int8_t compare(T a, T b);

template<class T>
int8_t sign(T);

#endif
