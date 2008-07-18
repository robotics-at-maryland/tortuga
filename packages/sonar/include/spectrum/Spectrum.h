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


template<int bitDepth>
class Spectrum {
public:
	virtual ~Spectrum() {}
	virtual void purge() =0;
	virtual void update(const typename adctype<bitDepth>::SIGNED *) =0;
	virtual const std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> &getAmplitude(int k, int channel) const =0;
	virtual const std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> &operator() (int k, int channel) const 
	{ return getAmplitude(k, channel); }
};


} // namespace sonar
} // namespace ram


#endif
