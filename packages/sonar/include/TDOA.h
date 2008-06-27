/**
 * @file TDOA.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Calculate time delay on arrival using a number of different techniques.
 * 
 */


#ifndef _RAM_SONAR_TDOA_H
#define _RAM_SONAR_TDOA_H


#include "SonarChunk.h"


namespace ram {
namespace sonar {


adcsampleindex_t tdoa_xcorr(const SonarChunk &a, const SonarChunk &b);
adcsampleindex_t tdoa_phase(const SonarChunk &a, const SonarChunk &b);
adcsampleindex_t tdoa_phaseonly(const SonarChunk &a, const SonarChunk &b);


} // namespace sonar
} // namespace ram


#endif
