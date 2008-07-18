/**
 * @file Spectrum.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_SPECTRUM_H
#define _RAM_SPECTRUM_H


#include "../Sonar.h"
#include "../adctypes.h"
#include <complex>


namespace ram {
namespace sonar {


template<typename ADC>
class Spectrum {
public:
	virtual ~Spectrum() {}
	virtual void purge() =0;
	virtual void update(const typename ADC::SIGNED *) =0;
	virtual const std::complex<typename ADC::QUADRUPLE_WIDE::SIGNED> &getAmplitude(int k, int channel) const =0;
	virtual const std::complex<typename ADC::QUADRUPLE_WIDE::SIGNED> &operator() (int k, int channel) const 
	{ return getAmplitude(k, channel); }
};


} // namespace sonar
} // namespace ram


#endif
