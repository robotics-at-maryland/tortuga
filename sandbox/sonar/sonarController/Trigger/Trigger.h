/*
 *  Trigger.h
 *  sonarController
 *
 *  Created by Leo Singer on 12/15/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef TRIGGER_H
#define TRIGGER_H


#include "Sonar.h"


#include <stdio.h>


namespace ram {
namespace sonar {


class Trigger {
public:
	Trigger(int nchannels, adcsampleindex_t delay, adcsampleindex_t holdoff, adcsampleindex_t pulsewidth);
	~Trigger();
	void update(adcdata_t *sample);
	bool getState() const;
protected:
	int nchannels;
	adcsampleindex_t delay, holdoff, pulsewidth;
};


} // namespace sonar
} // namespace ram


#endif
