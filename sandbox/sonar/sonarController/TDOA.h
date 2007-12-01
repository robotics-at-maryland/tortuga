/*
 *  TDOA.h
 *  sonarController
 *
 *  Created by Leo Singer on 11/30/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#ifndef TDOA_H
#define TDOA_H


#include "SonarChunk.h"


adcsampleindex_t tdoa_xcorr(const SonarChunk &a, const SonarChunk &b);
adcsampleindex_t tdoa_phase(const SonarChunk &a, const SonarChunk &b);
adcsampleindex_t tdoa_phaseonly(const SonarChunk &a, const SonarChunk &b);


#endif
