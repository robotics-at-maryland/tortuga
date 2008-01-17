/*
 *  SimpleSimpleSlidingDFT.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 12/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include "Trigger.h"


namespace ram {
namespace sonar {


Trigger::Trigger(int nchannels, adcsampleindex_t delay, adcsampleindex_t holdoff, adcsampleindex_t pulsewidth) 
	: nchannels(nchannels), delay(delay), holdoff(holdoff), pulsewidth(pulsewidth) {}


Trigger::~Trigger() {}


void Trigger::update(adcdata_t *sample) {}


bool getState(int channel)
{
	return false;
}


} // namespace sonar
} // namespace ram
